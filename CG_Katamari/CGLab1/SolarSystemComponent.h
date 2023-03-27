#pragma once

#include "export.h"
#include "Game.h"

class SolarSystemComponent : public Game
{
private: 
	void Init();
	
	TriangleComponentParameters PrepareRect(float xOff, float yOff);
	TriangleComponentParameters PrepareCircle(float xOff, float yOff);
	DirectX::SimpleMath::Vector3 rotationAxis = DirectX::SimpleMath::Vector3::Zero;

	InputDevice inputDeviceInstance;
	Camera* cameraInstanceP;

public:
	float angle = 0.0f;

	SolarSystemComponent();
	
	void Run();
	void Update() override;

	TriangleComponent* Ball;
	TriangleComponent* Planet;
	TriangleComponent* Sattelite;

	TriangleComponent CreateSphere(float radius, DirectX::SimpleMath::Vector4 color);
	TriangleComponent CreateSphere(float radius, DirectX::SimpleMath::Vector4 color, TriangleComponent* _parent);

	std::vector <TriangleComponent*> PlanetComponents;
};