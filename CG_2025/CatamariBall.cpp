#include "CatamariBall.h"
#include "global.h" 

#include "CatamariBox.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector3;

CatamariBall::CatamariBall(DirectX::SimpleMath::Vector3 position, float radius, int stacks, int slices)
	: position(position), radius(radius) {

	indices.clear();
	points.clear();

	auto context = GE::getGameSubsystem()->getDeviceContext();

	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();
	addData.screenCoords.x = winWidth;
	addData.screenCoords.y = winHeight;

	//for (int i = 0; i <= stacks; ++i) {
	//	float phi = DirectX::XM_PI * i / stacks;
	//	for (int j = 0; j <= slices; ++j) {
	//		float theta = DirectX::XM_2PI * j / slices;

	//		float x = radius * sinf(phi) * cosf(theta);
	//		float y = radius * cosf(phi);
	//		float z = radius * sinf(phi) * sinf(theta);

	//		points.push_back({ x, y, z, 1.0f });
	//		points.push_back({ 0.5f, 1.0f, 1.0f, 1.0f });
	//	}
	//}

	//for (int i = 0; i < stacks; ++i) {
	//	for (int j = 0; j < slices; ++j) {
	//		int first = i * (slices + 1) + j;
	//		int second = first + slices + 1;

	//		indices.push_back(first);
	//		indices.push_back(second);
	//		indices.push_back(first + 1);

	//		indices.push_back(second);
	//		indices.push_back(second + 1);
	//		indices.push_back(first + 1);
	//	}
	//}

	//strides = { 32 };
	//offsets = { 0 };
	strides = { 48 };
	offsets = { 0 };
}

int CatamariBall::init(const std::wstring& vertShaderPath, const std::wstring& pixShaderPath)
{
	MeshComponent::init(vertShaderPath, pixShaderPath);
	initTexturedObject("./models/ballt.obj");
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

int CatamariBall::draw(float deltaTime)
{
	Matrix transformMatrix = scale * Matrix::CreateFromQuaternion(qRot) * Matrix::CreateTranslation(position) * GE::getCameraViewMatrix() * GE::getProjectionMatrix();
	addData.transformMatrix = transformMatrix.Transpose();

	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(modelIndiciesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = modelVerticesBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->VSSetConstantBuffers(0, 1, &rawAdditionalBuffer);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->DrawIndexed(indexBufferData.size(), 0, 0);
	return 0;
}

int CatamariBall::update(float deltaTime)
{
	// gravity
	Vector3 downShiftVec = Vector3(0, -deltaTime, 0);
	position += downShiftVec;
	for (int i = 0; i < attachedObjects.size(); i++) {
		CatamariBox* box = attachedObjects[i];
		box->setPosition(box->getPosition() + downShiftVec);
	}

	float lowerPointY = position.y - radius;
	for (auto box : attachedObjects) {
		float boxMinY = box->getLowestPoint().y;
		if (boxMinY < lowerPointY) {
			lowerPointY = boxMinY;
		}
	}

	////// because ground plane is (0, -1, 0)
	if (lowerPointY < -1) {
		float diff = -(lowerPointY + 1);
		//shift all attached and ball up
		position.y += diff;
		for (auto box : attachedObjects) {
			box->setPosition(box->getPosition() + Vector3(0, diff, 0));
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

void CatamariBall::destroyResources()
{
	MeshComponent::destroyResources();
}

//TODO: remove?
void CatamariBall::setColor(DirectX::XMFLOAT4 clr)
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
	init(L"./shaders/planetShader.hlsl",
		L"./shaders/pixelShader.hlsl");
}

void CatamariBall::initTexturedObject(const std::string& modelPath)
{
	ModelImporter importer;
	importer.loadOBJ(modelPath, vertexBufferData, indexBufferData);
	auto bufferManager = GE::getBufferManager();

	modelVerticesBuffer = bufferManager->createVertexBuffer(vertexBufferData);
	layout = bufferManager->createInputLayout_PosF4_NormF4_TexF4(vertexByteCode);
	modelIndiciesBuffer = bufferManager->createIndexBuffer(indexBufferData);
	texturedModelLoaded = true;
}

void CatamariBall::moveBall(Vector3 shiftVec, Vector3 rotationAxis, float rotationAngle)
{
	position += shiftVec;
	//rotationAxis = Vector3::Transform(rotationAxis, qRot);
	float distance = shiftVec.Length();
	shiftVec.Normalize();
	if (distance > 0.00001f) {
		rotationAxis.Normalize();
		Quaternion globalRotation = Quaternion::CreateFromAxisAngle(rotationAxis, rotationAngle);
		Quaternion newRotation = qRot * globalRotation;
		newRotation.Normalize();
		qRot = newRotation;
		for (auto attachedBox : attachedObjects) {
			attachedBox->rotateAttached(Matrix::CreateFromAxisAngle(-rotationAxis, rotationAngle));
		}
	}
}

void CatamariBall::attachObject(CatamariBox* box)
{
	attachedObjects.push_back(box);
}

float CatamariBall::getRadius() const
{
	return radius;
}

Vector3 CatamariBall::getPosition() const
{
	return position;
}

void CatamariBall::setPosition(Vector3 pos)
{
	position = pos;
}

Quaternion CatamariBall::getRotation() const
{
	return qRot;
}

void CatamariBall::setRotation(Quaternion rotation)
{
	qRot = rotation;
}
