#pragma once
#include "GameObject.h"
#include "Enemy.h"

#define GOOMBA_WALKING_SPEED		0.05f

#define PARA_GOOMBA_SHORT_JUMP_TIME		300
#define PARA_GOOMBA_HIGH_JUMP_TIME		300
#define PARA_GOOMBA_WALKING_TIME		3000
#define PARA_GOOMBA_SHORT_JUMP_STACK	3
#define PARA_GOOMBA_SHORT_JUMP_SPEED	0.2f
#define PARA_GOOMBA_HIGH_JUMP_SPEED		0.4f
#define GOOMBA_DIE_DEFLECT_SPEED		0.5f

#define GOOMBA_NORMAL_BBOX_WIDTH				16
#define GOOMBA_NORMAL_BBOX_HEIGHT				15
#define PARA_GOOMBA_BBOX_WIDTH					20
#define PARA_GOOMBA_WING_UP_BBOX_HEIGHT			24
#define PARA_GOOMBA_WING_DOWN_BBOX_HEIGHT		20
#define GOOMBA_BBOX_HEIGHT_DIE					9

#define GOOMBA_STATE_PATROLING 100
#define GOOMBA_STATE_DIE 200

#define GOOMBA_NORMAL_ANI_WALKING		0
#define GOOMBA_NORMAL_ANI_DIE			1
#define PARA_GOOMBA_ANI_FLYING			2
#define PARA_GOOMBA_ANI_DIE				3	
#define PARA_GOOMBA_ANI_NO_WING			4
#define PARA_GOOMBA_ANI_WING_WALKING	5

class CGoomba : public CEnemy
{
	
public:
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	CGoomba(float x, float y);
	CGoomba();
	virtual void SetState(int state);
	virtual void OnCollisionEnter(LPCOLLISIONEVENT collision);
	virtual void OnTriggerEnter(LPCOLLISIONEVENT trigger);
	virtual void Reset();
};

enum PATROL_STATE {
	SHORT_JUMP = 1,
	HIGH_JUMP = 2,
	WALKING = 0
};

class CParaGoomba : public CGoomba {
protected:
	bool hasWings;

	PATROL_STATE patrolState;
	int shortJumpStack;

	DWORD statePatrolTimeStart;
	

public:
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	CParaGoomba(float x, float y);
	CParaGoomba();
	virtual void SetState(int state);
	virtual void LoseWings() { this->hasWings = false; this->y -= 10.0f; }
	virtual bool HasWings() { return this->hasWings; }
	virtual void OnCollisionEnter(LPCOLLISIONEVENT collision);
	virtual void OnTriggerEnter(LPCOLLISIONEVENT trigger);
	virtual void Reset();
};