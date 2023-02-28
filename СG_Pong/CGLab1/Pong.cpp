#include "export.h"
#include "Game.h"
#include "Pong.h"
#include "Keys.h"
#include "TriangleComponent.h"
#include <random>
#include <math.h>

#define PI 3.14159265

Pong::Pong() 
{
	Init();
}

void Pong::Init() 
{
	ball = new Ball(this, this->PrepareBall(0.0f, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f }));
	ball->SetDirection();
	Components.push_back(ball);
	
	racket1 = new Racket(this, this->PrepareRect(0.8, 0.0, {1.0f, 0.0f, 0.0f, 1.0f}), true);
	racket1->setPosition(0.8, 0.0);
	Components.push_back(racket1);
	
	racket2 = new Racket(this, this->PrepareRect(-0.8, 0.0, { 0.0f, 0.0f, 1.0f, 1.0f }), false);
	racket2->setPosition(-0.8, 0.0);
	Components.push_back(racket2);
}

void Pong::PrintScore() 
{
	std::cout << scoreLeft << " : " << scoreRight << std::endl;

}

void Pong::Reset() 
{
	ball->constData = { 0, 0 };
	ball->setPosition(0.0, 0.0);
	racket1->constData = { 0, 0 };
	racket1->setPosition(0.8, 0.0);
	racket2->constData = { 0, 0 };
	racket2->setPosition(-0.8, 0.0);
	ball->SetDirection();
	ball->speed = 1.0f;
}

void Pong::Run() 
{
	Game::Run();
}

void Pong::onOverlap(Ball* _ball, Racket* _bar)
{
	if ((((_ball->x - _ball->radius) <= (_bar->xPos + 0.02)) && ((_ball->x + _ball->radius) >= (_bar->xPos - 0.02))) &&
		(((_ball->y - _ball->radius) <= (_bar->yPos + 0.12)) && ((_ball->y + _ball->radius) >= (_bar->yPos - 0.12))))
	{
		if (_ball->isIntersect == false)
		{
			_ball->isIntersect = true;
			_ball->speed += 0.05f;
			if (_bar->isLeft)
			{
				_ball->direction.x = sin(PI + (_bar->yPos - _ball->y) * PI) * _ball->speed;
				_ball->direction.y = cos(PI + (_bar->yPos - _ball->y) * PI) * _ball->speed;
			}
			else
			{
				_ball->direction.x = sin(-PI - (_bar->yPos - _ball->y) * PI) * _ball->speed;
				_ball->direction.y = cos(-PI - (_bar->yPos - _ball->y) * PI) * _ball->speed;
			}
		}
	}
	else if (_ball->isIntersect) _ball->isIntersect = false;
}

void Pong::Update() 
{

	for (int i = 0; i < Components.size(); i++) 
	{
		Components[i]->Update(Game::context);
	}
	
	if (ball->x >= 1.0 - 0.05) 
	{
		Reset();
		scoreLeft += 1;
		PrintScore();
	}
	
	if (ball->x <= -1.0 + 0.05) 
	{
		Reset();
		scoreRight += 1;
		PrintScore();
	}

	if ((((ball->x - ball->radius) <= (racket1->xPos + 0.02)) && ((ball->x + ball->radius) >= (racket1->xPos - 0.02))) &&
		(((ball->y - ball->radius) <= (racket1->yPos + 0.12)) && ((ball->y + ball->radius) >= (racket1->yPos - 0.12))))
	{
		if (ball->isIntersect == false)
		{
			ball->isIntersect = true;
			ball->direction.x = sin(-PI / 2 - (racket1->yPos - ball->y) * PI / 3 / 0.12) * ball->initialSpeed;
			ball->direction.y = cos(-PI / 2 - (racket1->yPos - ball->y) * PI / 3 / 0.12) * ball->initialSpeed;
			ball->speed += 0.05f;
		}
	}
	else if (ball->isIntersect) ball->isIntersect = false;
	
	if (ball->isIntersect == false)
	{
		if ((((ball->x - ball->radius) <= (racket2->xPos + 0.02)) && ((ball->x + ball->radius) >= (racket2->xPos - 0.02))) &&
			(((ball->y - ball->radius) <= (racket2->yPos + 0.12)) && ((ball->y + ball->radius) >= (racket2->yPos - 0.12))))
		{
			if (ball->isIntersect == false)
			{
				ball->isIntersect = true;
				ball->direction.x = sin(PI / 2 + (racket2->yPos - ball->y) * PI / 3 / 0.12) * ball->initialSpeed;
				ball->direction.y = cos(PI / 2 + (racket2->yPos - ball->y) * PI / 3 / 0.12) * ball->initialSpeed;
				ball->speed += 0.05f;
			}
		}
		else if (ball->isIntersect) ball->isIntersect = false;
	}
}

Racket::Racket() 
{
	xPos = 0.0f;
	yPos = 0.0f;
}

Racket::Racket(Pong* game_, TriangleComponentParameters rect, bool isLeft_) : TriangleComponent(rect), game(game_) 
{
	xPos = 0.0f;
	yPos = 0.0f;
	isLeft = isLeft_;
}

Ball::Ball(Pong* game_, TriangleComponentParameters circle) : TriangleComponent(circle), game(game_) 
{
	x = 0.0f;
	y = 0.0f;
}

void Racket::Update(ID3D11DeviceContext* context)
{
	if (isLeft == false) 
	{
		if (game->inputDevice.IsKeyDown(Keys::W)) 
			if (yPos < 1.0 - 0.12) constData.y += 0.02;

		if (game->inputDevice.IsKeyDown(Keys::S)) 
			if (yPos > -(1.0 - 0.12)) constData.y -= 0.02;
	}
	else
	{
		if (game->inputDevice.IsKeyDown(Keys::Up)) 
			if (yPos < 1.0 - 0.12) constData.y += 0.02;

		if (game->inputDevice.IsKeyDown(Keys::Down)) 
			if (yPos > -(1.0 - 0.12)) constData.y -= 0.02;
	}

	yPos = constData.y;
	D3D11_MAPPED_SUBRESOURCE res = {};
	context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	auto dataP = reinterpret_cast<float*>(res.pData);
	memcpy(dataP, &constData, sizeof(ConstData));

	context->Unmap(constBuffer, 0);
}

void Ball::SetDirection() 
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, 314);

	auto angle = uni(rng);
	if (angle == 0 || angle == PI) angle = PI / 2;

	direction.x = sin(angle) * initialSpeed;
	direction.y = cos(angle) * initialSpeed;
	direction.z = 0;
}

void Ball::Update(ID3D11DeviceContext* context) 
{
	x += direction.x * speed;
	y += direction.y * speed;

	if (y >= 1.0f - radius) direction.y = -direction.y;
	
	if (y <= -1.0f + radius) direction.y = -direction.y;
	
	constData.x = x;
	constData.y = y;
	
	D3D11_MAPPED_SUBRESOURCE res = {};
	context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	auto dataP = reinterpret_cast<float*>(res.pData);
	memcpy(dataP, &constData, sizeof(ConstData));

	context->Unmap(constBuffer, 0);
}

TriangleComponentParameters Pong::PrepareBall(float xOff, float yOff, DirectX::XMFLOAT4 color)
{
	float radius = 0.03f;
	
	TriangleComponentParameters ball;
	
	ball.numPoints = 22;
	ball.numIndeces = 30;
	ball.indeces = new int[30] { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 9, 0, 9, 10, 0, 10, 1 };
	ball.points = new DirectX::XMFLOAT4[ball.numPoints]
	{
		DirectX::XMFLOAT4(0 + xOff, 0 + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(0) + xOff, radius * sin(0) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(PI / 5) + xOff, radius * sin(PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(2 * PI / 5) + xOff, radius * sin(2 * PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(3 * PI / 5) + xOff, radius * sin(3 * PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(4 * PI / 5) + xOff, radius * sin(4 * PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(5 * PI / 5) + xOff, radius * sin(5 * PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(6 * PI / 5) + xOff, radius * sin(6 * PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(7 * PI / 5) + xOff, radius * sin(7 * PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(8 * PI / 5) + xOff, radius * sin(8 * PI / 5) + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(radius * cos(9 * PI / 5) + xOff, radius * sin(9 * PI / 5) + yOff, 0.5f, 1.0f),	color
	};

	return ball;
};

TriangleComponentParameters Pong::PrepareRect(float xOff, float yOff, DirectX::XMFLOAT4 color) 
{
	TriangleComponentParameters rect;
	
	rect.numPoints = 8;
	rect.numIndeces = 6;
	rect.indeces = new int[6] { 0, 1, 2, 1, 0, 3 };
	rect.points = new DirectX::XMFLOAT4[rect.numPoints]
	{
		DirectX::XMFLOAT4(0.02f + xOff, 0.12f + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(-0.02f + xOff, -0.12f + yOff, 0.5f, 1.0f),color,
		DirectX::XMFLOAT4(0.02f + xOff, -0.12f + yOff, 0.5f, 1.0f),	color,
		DirectX::XMFLOAT4(-0.02f + xOff, 0.12f + yOff, 0.5f, 1.0f),	color
	};

	return rect;
};