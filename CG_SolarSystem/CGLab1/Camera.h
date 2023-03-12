#pragma once

#include "export.h"
#include "InputDevice.h"

#define PI 3.1415926535

class Camera {
	private:
		InputDevice* inputDeviceCameraInstance;

	public:
		DirectX::SimpleMath::Matrix viewMatrix;
		DirectX::SimpleMath::Matrix projectionMatrix;
		DirectX::SimpleMath::Vector3 position;

		float yawAxis;
		float pitchAxis;
		float velocityMagnitude = 2.0f;
		float mouseSensetivity = 1.0f;

		Camera();

		void Initialize(
			DirectX::SimpleMath::Vector3 pos, 
			float yawVal, 
			float pitchVal, 
			int screenWidth, 
			int screenHeight, 
			InputDevice* inputeDeviceInstance
		);
		void Update(float deltaTime, int screenWidth, int screenHeight);
		void OnMouseMove(const MouseMoveEventArgs& args);
};