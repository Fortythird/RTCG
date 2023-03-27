#include "export.h"
#include "Camera.h"

Camera::Camera(DirectX::SimpleMath::Vector3* _parentPos) 
{
	parentPos = _parentPos;
}

void Camera::Initialize(
	DirectX::SimpleMath::Vector3 pos, 
	float yawVal, 
	float pitchVal, 
	int screenWidth, 
	int screenHeight, 
	InputDevice* inputeDeviceInstance) 
{
	inputDeviceCameraInstance = inputeDeviceInstance;
	viewMatrix = DirectX::SimpleMath::Matrix::Identity;

	projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		((float) PI) / 2.0f,
		(float) screenWidth / screenHeight, 
		0.1f,
		100.0f
	); 
	
	yawAxis = yawVal;
	pitchAxis = pitchVal;
	position = pos;
	
	if (inputDeviceCameraInstance != nullptr) {
		inputDeviceCameraInstance->MouseMove.AddRaw(this, &Camera::OnMouseMove);
	}
}

void Camera::Update(float deltaTime, int screenWidth, int screenHeight) 
{	
	position = {
		parentPos->x + 3.0f * static_cast<float>(cos(orbitPos)),
		parentPos->y + 3.0f * static_cast<float>(sin(orbitPos)),
		parentPos->z + 2.0f
	};
	
	camDirection = { parentPos->x - position.x, parentPos->y - position.y , 0.0f};

	viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(position, *parentPos, DirectX::SimpleMath::Vector3(0, 0, 1));
	projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		(float)PI / 2.0f,
		(float)screenWidth / screenHeight,
		0.1f,
		100.0f
	);
}

void Camera::OnMouseMove(const MouseMoveEventArgs& args) 
{
	if (inputDeviceCameraInstance == nullptr) 
	{
		return;
	}
	orbitPos -= args.Offset.x * 0.01f * mouseSensetivity;
}