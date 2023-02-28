#pragma once

#include "export.h"
#include "Game.h"
#include "TriangleComponent.h"

class Pong;

class Ball : public TriangleComponent 
{
private:

	Pong* game;

public:
	float initialSpeed = 0.01f;
	float radius = 0.05f;
	bool isIntersect = false;
	float speed = 1.0f;
	float x;
	float y;

	DirectX::SimpleMath::Vector3 direction;
	
	void setPosition(float x1, float y1) {
		x = x1;
		y = y1;
	};
	
	Ball(Pong* game_, TriangleComponentParameters circle);

	void SetDirection();
	
	void Update(ID3D11DeviceContext* context) override;
};

class Racket : public TriangleComponent 
{
private:

	Pong* game;

public:
	Racket();
	Racket(Pong* game_, TriangleComponentParameters rect, bool isLeft_);

	float xPos;
	float yPos;
	bool isLeft;
	
	void setPosition(float x, float y) 
	{
		xPos = x;
		yPos = y;
	};

	void Update(ID3D11DeviceContext* context) override;
};

class Pong : public Game 
{
private:
	ID3D11DeviceContext* context;
	Ball* ball;
	Racket* racket1;
	Racket* racket2;
	
	int scoreLeft;
	int scoreRight;
	float velocity;
	
	void Init();
	
	TriangleComponentParameters PrepareRect(float xOff, float yOff, DirectX::XMFLOAT4 color);
	TriangleComponentParameters PrepareBall(float xOff, float yOff, DirectX::XMFLOAT4 color);
	
public:
	void Run();
	void Reset();
	void PrintScore();
	void Update() override;
	void onOverlap(Ball* ball, Racket* bar);
	
	Pong();
};