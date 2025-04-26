#include "DirectXTex.h"

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

	strides = { 48 };
	offsets = { 0 };
}

int CatamariBall::init(const std::wstring& vertShaderPath, const std::wstring& pixShaderPath)
{
	MeshComponent::init(vertShaderPath, pixShaderPath);
	initTexturedObject("./models/ball.obj");
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

	addData.material = material;
	additionalBuffer = bufferManager->createBuffer(buffDesc, subresourceData);

	DirectX::ScratchImage si;
	DirectX::LoadFromDDSFile(L"./models/bt.dds", DirectX::DDS_FLAGS_NONE, nullptr, si);
	DirectX::CreateShaderResourceView(GE::getGameSubsystem()->getDevice(), si.GetImages(), si.GetImageCount(), si.GetMetadata(), &texture);
	createShadowVertexShader(L"./shaders/vertexShadowShader.hlsl");
	createShadowPixelShader(L"./shaders/pixelShadowShader.hlsl");
	return 0;
}

int CatamariBall::draw(float deltaTime)
{
	Matrix transformMatrix = scale * Matrix::CreateFromQuaternion(qRot) * Matrix::CreateTranslation(position) * GE::getCameraViewMatrix() * GE::getProjectionMatrix();
	addData.transformMatrix = transformMatrix.Transpose();
	addData.rotationMatrix = Matrix::CreateFromQuaternion(qRot).Transpose();
	addData.scaleMatrix = scale.Transpose();
	addData.translationMatrix = Matrix::CreateTranslation(position).Transpose();
	addData.viewMatrix = GE::getCameraViewMatrix().Transpose();
	addData.projectionMatrix = GE::getProjectionMatrix().Transpose();

	Vector4 camPos4;
	Vector3 camPos3 = GE::getCameraPosition();
	camPos4.x = camPos3.x;
	camPos4.y = camPos3.y;
	camPos4.z = camPos3.z;
	camPos4.w = 1.0f;
	addData.camPos = camPos4;
	addData.nearFar = GE::getNearFarPlanes();

	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(modelIndiciesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = modelVerticesBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->VSSetConstantBuffers(0, 1, &rawAdditionalBuffer);
	context->PSSetConstantBuffers(0, 1, &rawAdditionalBuffer);

	context->PSSetShaderResources(0, 1, &texture);
	ID3D11SamplerState* rawSampler = GE::getGameSubsystem()->getSamplerState().Get();
	context->PSSetSamplers(0, 1, &rawSampler);

	context->DrawIndexed(indexBufferData.size(), 0, 0);
	return 0;
}

int CatamariBall::drawShadow()
{
	addData.rotationMatrix = Matrix::CreateFromQuaternion(qRot).Transpose();
	addData.scaleMatrix = scale.Transpose();
	addData.translationMatrix = Matrix::CreateTranslation(position).Transpose();

	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetIndexBuffer(modelIndiciesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = modelVerticesBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->VSSetConstantBuffers(0, 1, &rawAdditionalBuffer);
	GE::getRenderSubsystem()->bindDefaultShaders();
	context->VSSetShader(vertexShadowShader, nullptr, 0);
	context->PSSetShader(pixelShadowShader, nullptr, 0);

	context->DrawIndexed(indexBufferData.size(), 0, 0);
	return 0;
}

int CatamariBall::update(float deltaTime)
{
	// gravity
	Vector3 downShiftVec = Vector3(0, -deltaTime, 0);
	position += downShiftVec;
	Vector3 camPos = GE::getCameraPosition();
	camPos += downShiftVec;
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
		camPos.y += diff;
		for (auto box : attachedObjects) {
			box->setPosition(box->getPosition() + Vector3(0, diff, 0));
		}
	}
	GE::setCameraPosition(camPos);

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

void CatamariBall::initTexturedObject(const std::string& modelPath)
{
	ModelImporter importer;
	importer.loadOBJ(modelPath, vertexBufferData, indexBufferData);
	for (auto& v : vertexBufferData) {
		v.tex.z = generateRandomFloat(0, DirectX::XM_2PI);
		v.tex.w = generateRandomFloat(0, DirectX::XM_2PI);
	}

	auto bufferManager = GE::getBufferManager();

	modelVerticesBuffer = bufferManager->createVertexBuffer(vertexBufferData);
	layout = bufferManager->createInputLayout_PosF4_NormF4_TexF4_AddF4(vertexByteCode);
	modelIndiciesBuffer = bufferManager->createIndexBuffer(indexBufferData);
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
			attachedBox->rotateAttached(Matrix::CreateFromAxisAngle(rotationAxis, rotationAngle));
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
