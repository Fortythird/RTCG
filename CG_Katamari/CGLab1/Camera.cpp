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

void Camera::Update(float deltaTime, int screenWidth, int screenHeight) {
	auto rotation = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(0, 1, 0);
	
	if (inputDeviceCameraInstance != nullptr) 
	{
		position = {
		parentPos->x + 3 * static_cast<float>(cos(orbitPos)),
		parentPos->y + 2,
		parentPos->z + 3 * static_cast<float>(sin(orbitPos))
		};

		/*auto velDirection = DirectX::SimpleMath::Vector3::Zero;

		if (inputDeviceCameraInstance->IsKeyDown(Keys::W))velDirection += DirectX::SimpleMath::Vector3(5.0f, 0.0f, 0.0f);
		if (inputDeviceCameraInstance->IsKeyDown(Keys::S)) velDirection += DirectX::SimpleMath::Vector3(-5.0f, 0.0f, 0.0f);
		if (inputDeviceCameraInstance->IsKeyDown(Keys::A)) velDirection += DirectX::SimpleMath::Vector3(0.0f, 0.0f, -5.0f);
		if (inputDeviceCameraInstance->IsKeyDown(Keys::D)) velDirection += DirectX::SimpleMath::Vector3(0.0f, 0.0f, 5.0f);
		if (inputDeviceCameraInstance->IsKeyDown(Keys::Space)) velDirection += DirectX::SimpleMath::Vector3(0.0f, 5.0f, 0.0f);
		if (inputDeviceCameraInstance->IsKeyDown(Keys::Q)) velDirection += DirectX::SimpleMath::Vector3(0.0f, -5.0f, 0.0f);

		velDirection.Normalize();

		auto velDir = rotation.Forward() * velDirection.x + DirectX::SimpleMath::Vector3::Up * velDirection.y + rotation.Right() * velDirection.z;
		if (velDir.Length() != 0) {
			velDir.Normalize();
		}
		position = position + velDir * velocityMagnitude * deltaTime;*/
	}
	
	camDirection = { parentPos->x - position.x, 0, parentPos->z - position.z };

	viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(position, /*position + rotation.Forward() */ camDirection, DirectX::SimpleMath::Vector3(0, 1, 0));
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
	//pitchAxis -= args.Offset.y * 0.003f * mouseSensetivity;

	//if (args.WheelDelta == 0) return;
	//if (args.WheelDelta > 0) velocityMagnitude += 1;
	//if (args.WheelDelta < 0) velocityMagnitude -= 1;
}