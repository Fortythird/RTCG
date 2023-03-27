#include "SolarSystemComponent.h"
#include <random>

SolarSystemComponent::SolarSystemComponent() 
{
	Init();
}

void SolarSystemComponent::Init()
{
	Ball = new TriangleComponent(CreateSphere(1.0, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f)));
	Components.push_back(Ball);	

	Planet = new TriangleComponent(CreateSphere(0.5, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f), Ball));
	Planet->pos = { 0.0, 3.0, 0.0 };
	Components.push_back(Planet);

	auto cameraInstance = new Camera(&Ball->pos);
	cameraInstance->Initialize(
		DirectX::SimpleMath::Vector3(0.0f, 5.0f, 0.0f),
		(1.57 / 2),
		(1.57 / 2),
		display.getScreenWidth(),
		display.getScreenHeight(),
		&inputDevice
	);

	camera.push_back(cameraInstance);

	inputDeviceInstance.Init(display.getHWND());

	cameraInstanceP = cameraInstance;

}

void SolarSystemComponent::Run() 
{
	Game::Run();
}

void SolarSystemComponent::Update() 
{
	Game::camera.at(0)->Update(
		Game::deltaTime, 
		Game::display.getScreenWidth(), 
		Game::display.getScreenHeight()
	);

	angle += 0.1f;
	
	Ball->Update(Game::context, Game::camera.at(0));
	Planet->Update(Game::context, Game::camera.at(0));

	DirectX::SimpleMath::Vector3 deltaPos = DirectX::SimpleMath::Vector3::Zero;

	if (cameraInstanceP->inputDeviceCameraInstance != nullptr)
	{
		if (cameraInstanceP->inputDeviceCameraInstance->IsKeyDown(Keys::W)) deltaPos += cameraInstanceP->camDirection;
		if (cameraInstanceP->inputDeviceCameraInstance->IsKeyDown(Keys::S)) deltaPos -= cameraInstanceP->camDirection;
		if (cameraInstanceP->inputDeviceCameraInstance->IsKeyDown(Keys::A)) deltaPos -= cameraInstanceP->camDirection.Cross(DirectX::SimpleMath::Vector3(0, 0, 1));
		if (cameraInstanceP->inputDeviceCameraInstance->IsKeyDown(Keys::D)) deltaPos += cameraInstanceP->camDirection.Cross(DirectX::SimpleMath::Vector3(0, 0, 1));
	}

	deltaPos.Normalize();
	deltaPos /= 20;

	Ball->pos += deltaPos;

	float distance = deltaPos.Length();
	float ang = distance;
	if (distance) 
	{
		DirectX::SimpleMath::Vector3 deltaRotationAxis = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f).Cross(deltaPos);
		deltaRotationAxis.Normalize();

		Ball->rotate *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(deltaRotationAxis, ang);
	}
}


TriangleComponent SolarSystemComponent::CreateSphere(float radius, DirectX::SimpleMath::Vector4 color, TriangleComponent* _parent)
{
	TriangleComponentParameters sphere;
	int parallels = 50;
	int meridians = 50;
	
	sphere.numPoints = parallels * meridians * 4 - 1 * 2 * meridians;
	DirectX::SimpleMath::Vector4* positions = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	
	sphere.numIndeces = parallels * meridians * 6 - 3 * 2 * meridians;
	sphere.numPoints *= 2;
	sphere.points = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	sphere.indeces = new int[sphere.numIndeces];
	
	int tempPos = 0;
	int tempInd = 0;
	
	DirectX::SimpleMath::Vector4 vertex1, vertex2, vertex3, vertex4;
	
	for (int i = 0; i < parallels; i++)
	{
		float teta1 = ((float)(i) / parallels) * PI;
		float teta2 = ((float)(i + 1) / parallels) * PI;
		
		for (int j = 0; j < meridians; j++)
		{
			float fi1 = ((float)(j) / meridians) * 2 * PI;
			float fi2 = ((float)(j + 1) / meridians) * 2 * PI;

			float x;
			float y;
			float z;
			
			x = radius * sin(teta1) * cos(fi1);
			y = radius * sin(teta1) * sin(fi1);
			z = radius * cos(teta1);
			vertex1 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = radius * sin(teta1) * cos(fi2);
			y = radius * sin(teta1) * sin(fi2);
			z = radius * cos(teta1);
			vertex2 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = radius * sin(teta2) * cos(fi2);
			y = radius * sin(teta2) * sin(fi2);
			z = radius * cos(teta2);
			vertex3 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = radius * sin(teta2) * cos(fi1);
			y = radius * sin(teta2) * sin(fi1);
			z = radius * cos(teta2);
			vertex4 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);

			if (i == 0)
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else if (i == parallels - 1)
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				tempPos++;

				sphere.indeces[tempInd] = tempPos - 1;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 4;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 2;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 1;
				tempInd++;
			}
		}
	}

	int temp = 0;

	for (int i = 0; i < sphere.numPoints; i += 2) 
	{
		sphere.points[i] = positions[temp];

		if ((i / 50) % 2 == 0) sphere.points[i + 1] = color;
		else sphere.points[i + 1] = DirectX::SimpleMath::Vector4(0.2f * color.x, 0.3f * color.y, 0.8f * color.z, 1.0f);

		temp++;
	}
	
	TriangleComponent Sphere1(sphere);

	//Sphere1.orbit = _orbit;
	Sphere1.parent = _parent;
	
	return Sphere1;

}

TriangleComponent SolarSystemComponent::CreateSphere(float radius, DirectX::SimpleMath::Vector4 color)
{
	TriangleComponentParameters sphere;
	int parallels = 50;
	int meridians = 50;
	
	sphere.numPoints = parallels * meridians * 4 - 1 * 2 * meridians;
	DirectX::SimpleMath::Vector4* positions = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	
	sphere.numIndeces = parallels * meridians * 6 - 3 * 2 * meridians;
	sphere.numPoints *= 2;
	sphere.points = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	sphere.indeces = new int[sphere.numIndeces];
	
	int tempPos = 0;
	int tempInd = 0;
	
	DirectX::SimpleMath::Vector4 vertex1, vertex2, vertex3, vertex4;
	
	for (int i = 0; i < parallels; i++)
	{
		float teta1 = ((float)(i) / parallels) * PI;
		float teta2 = ((float)(i + 1) / parallels) * PI;
		
		for (int j = 0; j < meridians; j++)
		{
			float fi1 = ((float)(j) / meridians) * 2 * PI;
			float fi2 = ((float)(j + 1) / meridians) * 2 * PI;

			float x;
			float y;
			float z;
			
			x = radius * sin(teta1) * cos(fi1);
			y = radius * sin(teta1) * sin(fi1);
			z = radius * cos(teta1);
			vertex1 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = radius * sin(teta1) * cos(fi2);
			y = radius * sin(teta1) * sin(fi2);
			z = radius * cos(teta1);
			vertex2 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = radius * sin(teta2) * cos(fi2);
			y = radius * sin(teta2) * sin(fi2);
			z = radius * cos(teta2);
			vertex3 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			
			x = radius * sin(teta2) * cos(fi1);
			y = radius * sin(teta2) * sin(fi1);
			z = radius * cos(teta2);
			vertex4 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);

			if (i == 0)
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else if (i == parallels - 1)
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else
			{
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				tempPos++;
				positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				tempPos++;

				sphere.indeces[tempInd] = tempPos - 1;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 4;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 2;
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 1;
				tempInd++;
			}
		}
	}

	int temp = 0;

	for (int i = 0; i < sphere.numPoints; i += 2) {
		sphere.points[i] = positions[temp];

		if ((i / 50) % 2 == 0) sphere.points[i + 1] = color;
		else sphere.points[i + 1] = DirectX::SimpleMath::Vector4(0.2f * color.x, 0.3f * color.y, 0.8f * color.z, 1.0f);

		temp++;
	}
	
	TriangleComponent Sphere1(sphere);
	
	return Sphere1;

}