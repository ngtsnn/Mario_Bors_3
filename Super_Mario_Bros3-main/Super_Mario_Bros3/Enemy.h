#pragma once

#include "GameObject.h"
#include "Game.h"

#define ENEMY_TYPE_GOOMBA		20
#define ENEMY_TYPE_PARA_GOOMBA	21
#define ENEMY_TYPE_KOOPAS		3

class CEnemy : public CGameObject {
protected: 
	float startX;
	float startY;

public:
	CEnemy(float x, float y);
	CEnemy();

	virtual void Reset();
};

typedef CEnemy* LPENEMY;

class CEnemySpawner : public CGameObject{
protected:
	int width;
	int height;

	LPENEMY enemy;
public:
	CEnemySpawner(float l, float t, float r, float b);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void AddEnemy(LPENEMY newEnemy) { this->enemy = newEnemy; }
	virtual void SpawnEnemy();
};