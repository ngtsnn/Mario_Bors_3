#include "Enemy.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Mario.h"

//#Enemy Section
CEnemy::CEnemy(float x, float y) {
	this->startX = x;
	this->startY = y;
	this->x = x;
	this->y = y;
	this->isUsingGravity = true;
	this->collisionState = COLLISION;
	this->isStatic = false;
}

CEnemy::CEnemy() {
	this->isUsingGravity = true;
	this->isStatic = false;
	this->collisionState = COLLISION;
}

void CEnemy::Reset() {
	this->x = startX;
	this->y = startY;
	this->collisionState = COLLISION;
}


//#EnemySpawner Section
CEnemySpawner::CEnemySpawner(float l, float t, float r, float b)
{
	x = l;
	y = t;
	this->width = r - l + 1;
	this->height = b - t + 1;
	this->isUsingGravity = false;
	this->collisionState = TRIGGER;
}

void CEnemySpawner::Render()
{
	//RenderBoundingBox();
}

void CEnemySpawner::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = this->x;
	t = this->y;
	r = this->x + width;
	b = this->y + height;
	this->isUsingGravity = false;
}

void CEnemySpawner::SpawnEnemy() {
	this->enemy->Reset();
}

void CEnemySpawner::OnTriggerEnter(LPCOLLISIONEVENT trigger) {
}