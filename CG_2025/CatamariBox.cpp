#include "CatamariBox.h"
#include "global.h"
#include "ModelImporter.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

CatamariBox::CatamariBox(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size)
	: position(position), size(size) {

	float halfWidth = size.x * 0.5f;
	float halfHeight = size.y * 0.5f;
	float halfDepth = size.z * 0.5f;

	std::vector<DirectX::SimpleMath::Vector4> vertices = {
		{ -halfWidth, -halfHeight,  halfDepth, 1.0f },
		{  halfWidth, -halfHeight,  halfDepth, 1.0f },
		{  halfWidth,  halfHeight,  halfDepth, 1.0f },
		{ -halfWidth,  halfHeight,  halfDepth, 1.0f },
		{ -halfWidth, -halfHeight, -halfDepth, 1.0f },
		{  halfWidth, -halfHeight, -halfDepth, 1.0f },
		{  halfWidth,  halfHeight, -halfDepth, 1.0f },
		{ -halfWidth,  halfHeight, -halfDepth, 1.0f }
	};

	for (const auto& vertex : vertices) {
		points.push_back(vertex);
		points.push_back({ 0.5f, 1.0f, 1.0f, 1.0f });
	}

	indices = {
		0, 1, 2,  0, 2, 3,
		4, 6, 5,  4, 7, 6,
		0, 4, 5,  0, 5, 1,
		3, 2, 6,  3, 6, 7,
		0, 3, 7,  0, 7, 4,
		1, 5, 6,  1, 6, 2
	};

	strides = { 32 };
	offsets = { 0 };
}

int CatamariBox::init(const std::wstring& vertShaderPath, const std::wstring& pixShaderPath)
{
	MeshComponent::init(vertShaderPath, pixShaderPath);
	auto bufferManager = GE::getBufferManager();

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = &addData;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC buffDesc = {};
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;
	buffDesc.ByteWidth = sizeof(addData);

	additionalBuffer = bufferManager->createBuffer(buffDesc, subresourceData);
	return 0;
}

int CatamariBox::draw(float deltaTime)
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());

	if (drawDebugCollider) {
		auto bb = getAABB();
		cornersPoints.clear();
		Vector3 corners[8];
		bb.GetCorners(corners);
		for (int i = 0; i < 8; i++) {
			const auto cp = corners[i];
			cornersPoints.push_back(Vector4(cp.x, cp.y, cp.z, 1.0f) + Vector4(position.x, position.y, position.z, 1.0));
			cornersPoints.push_back(Vector4(1.0, 0.0, 0.0, 1.0f));
		}
		//TODO: draw
	}

	if (texturedModelSet) {
		// TODO: draw obj model
		model->Draw(context, *states, Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position), GE::getCameraViewMatrix(), GE::getProjectionMatrix());
		return 0;
	}

	Matrix transformMatrix = Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position) * GE::getCameraViewMatrix() * GE::getProjectionMatrix();
	addData.transformMatrix = transformMatrix.Transpose();

	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->VSSetConstantBuffers(0, 1, &rawAdditionalBuffer);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->DrawIndexed(indices.size(), 0, 0);
	return 0;
}

int CatamariBox::update(float deltaTime)
{
	if (!attached) {
		position += Vector3(0, -deltaTime, 0);

		float lowerPointY = 1;
		float boxMinY = getLowestPoint().y;
		if (boxMinY < lowerPointY) {
			lowerPointY = boxMinY;
		}

		if (lowerPointY < -1) {
			float diff = -(lowerPointY + 1);
			position.y += diff;
		}
	}

	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->Map(rawAdditionalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &addData, sizeof(AdditionalData));
	context->Unmap(rawAdditionalBuffer, 0);
	return 0;
}

void CatamariBox::destroyResources()
{
	MeshComponent::destroyResources();
}

void CatamariBox::initTexturedObject(const std::wstring& modelPath)
{
	auto device = GE::getGameSubsystem()->getDevice();
	states = std::make_unique<CommonStates>(device);
	fxFactory = std::make_unique<EffectFactory>(device);
	model = Model::CreateFromCMO(device, modelPath.c_str(), *fxFactory);
	//ModelImporter importer;
	//importer.loadOBJ(modelPath, modelVertices, modelTexCoords, modelNormals, modelFaces);
	texturedModelSet = true;
}

bool CatamariBox::isAttached() const
{
	return attached;
}

void CatamariBox::setAttached(CatamariBall* ball, bool flag)
{
	this->ball = ball;
	attachedOffset = position - ball->getPosition();
	attached = flag;
}

Vector3 CatamariBox::getLowestPoint()
{
	// ���������� ������� ���� ������������ ������
	std::vector<Vector3> vertices = {
		Vector3(-size.x / 2, -size.y / 2, -size.z / 2),
		Vector3(size.x / 2, -size.y / 2, -size.z / 2),
		Vector3(size.x / 2, -size.y / 2, size.z / 2),
		Vector3(-size.x / 2, -size.y / 2, size.z / 2),
		Vector3(-size.x / 2, size.y / 2, -size.z / 2),
		Vector3(size.x / 2, size.y / 2, -size.z / 2),
		Vector3(size.x / 2, size.y / 2, size.z / 2),
		Vector3(-size.x / 2, size.y / 2, size.z / 2)
	};

	// ��������� ��������
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z);
	for (auto& vertex : vertices) {
		// ��������� �������� � ��������� � ������� ����������
		vertex = Vector3::Transform(vertex, rotationMatrix);
		vertex += position;
	}

	// ������� ������ �����
	float minY = vertices[0].y;
	int minIdx = 0;
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].y < minY) {
			minY = vertices[i].y;
			minIdx = i;
		}
	}


	// ���������� ������ �����
	return vertices[minIdx];
}

Vector3 CatamariBox::getPosition() const
{
	return position;
}

void CatamariBox::setPosition(Vector3 pos)
{
	position = pos;
}

void CatamariBox::setColor(DirectX::XMFLOAT4 clr)
{
	int size = points.size();
	for (int i = 0; i < size; i++) {
		if (i % 2 == 1) {
			auto& p = points[i];
			p.x = clr.x;
			p.y = clr.y;
			p.z = clr.z;
		}
	}
	init(L"./shaders/texVertexShader.hlsl",
		L"./shaders/texPixelShader.hlsl");
}

void CatamariBox::rotateAttached(Matrix rot)
{
	rotation *= Quaternion::CreateFromRotationMatrix(rot);
	Vector3 localPos = position - ball->getPosition();
	localPos = Vector3::Transform(localPos, rot);
	position = ball->getPosition() + localPos;
}

DirectX::BoundingBox CatamariBox::getAABB() const {
	return DirectX::BoundingBox(position, size);
}