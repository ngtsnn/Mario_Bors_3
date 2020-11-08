#pragma once

#include "GameObject.h"
#include "Enemy.h"

#define KOOPAS_COLOR_GREEN	0
#define KOOPAS_COLOR_RED	8

#define KOOPAS_WALKING_SPEED		0.04f
#define KOOPAS_SHELLING_TIME		6000
#define KOOPAS_KICKED_SPEED			0.3f
#define KOOPAS_KICKED_STACK			4
#define KOOPAS_JUMPING_TIME			1000
#define KOOPAS_JUMPING_SPEED		0.45f
#define KOOPAS_DIE_DEFLECT_SPEED	0.5f

#define KOOPAS_BBOX_WIDTH			16
#define KOOPAS_BBOX_HEIGHT			26
#define KOOPAS_BBOX_HEIGHT_SHELL	16

#define KOOPAS_STATE_PATROL			100
#define KOOPAS_STATE_SHELL			200
#define KOOPAS_STATE_BE_HELD		300
#define KOOPAS_STATE_BE_KICKED		400

#define KOOPAS_ANI_WALKING_LEFT			0
#define KOOPAS_ANI_WALKING_RIGHT		1
#define KOOPAS_ANI_FLYING_LEFT			2
#define KOOPAS_ANI_FLYING_RIGHT			3
#define KOOPAS_ANI_SHELL_DOWN			4
#define KOOPAS_ANI_SHELL_UP				5
#define KOOPAS_ANI_SHELL_SPINNING		6
#define KOOPAS_ANI_SHELL_SPIN_UP		7

class CKoopas : public CEnemy
{
protected:
	int color;
	bool isPara;

	bool isShelling;
	bool isShellUp;
	bool isKicked;
	int kickedCollisionStack;
	bool isBeingHeld;
	bool hasWings;
	PATROL_STATE patrolState;
	DWORD shellingTimeStart;
	DWORD jumpingTimeStart;
	
public:
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	CKoopas(float x, float y, int color, bool isPara);
	CKoopas();
	virtual void SetState(int state);
	virtual void OnCollisionEnter(LPCOLLISIONEVENT collision);
	virtual void OnTriggerEnter(LPCOLLISIONEVENT trigger);
	virtual void Reset();

	virtual bool IsPara() { return this->isPara; }
	virtual bool IsShelling() { return this->state == KOOPAS_STATE_SHELL; }
	virtual bool HasWings() { return this->hasWings; }
	virtual int GetColor() { return this->color; }

	virtual void ShellDown();
	virtual void ShellUp();
	virtual void LoseWings();
	virtual void BeKicked(int dir);
	virtual void BeHeld();
	virtual void Die() { CEnemy::Die(); collisionState = NONE; }
};