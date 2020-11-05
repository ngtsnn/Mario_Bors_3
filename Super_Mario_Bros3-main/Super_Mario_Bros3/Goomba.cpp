#include "Goomba.h"
#include "Mario.h"

CGoomba::CGoomba(float x, float y)
{
	this->startX = x;
	this->startY = y;
	this->x = x;
	this->y = y;
	this->isDeath = false;
	this->isUsingGravity = true;
	this->collisionState = COLLISION;
	this->isStatic = false;
	SetState(GOOMBA_STATE_WALKING);
}

CGoomba::CGoomba() {
	SetState(GOOMBA_STATE_WALKING);
	this->nx = -1;
	this->isDeath = false;
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + GOOMBA_NORMAL_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_NORMAL_BBOX_HEIGHT;
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 

	// Enemy AI in here
	switch (this->state)
	{
	case GOOMBA_STATE_DIE:
		y += GOOMBA_NORMAL_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		this->isDeath = true;
		vx = 0;
		vy = 0;
		this->collisionState = NONE;
		break;
	case GOOMBA_STATE_WALKING:
		if (this->vx == 0) {
			this->vx = -GOOMBA_WALKING_SPEED;
		}
		if (this->x > this->maxPatrolX) {
			this->vx = -GOOMBA_WALKING_SPEED;
		}
		else if (this->x < this->minPatrolX) {
			this->vx = GOOMBA_WALKING_SPEED;
		}
		break;
	}
}

void CGoomba::OnCollisionEnter(LPCOLLISIONEVENT collision) {
	
}

void CGoomba::OnTriggerEnter(LPCOLLISIONEVENT collision) {

}

void CGoomba::Render()
{
	int ani = GOOMBA_NORMAL_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE) {
		ani = GOOMBA_NORMAL_ANI_DIE;
	}

	animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	
}

void CGoomba::Reset() {
	CEnemy::Reset();
	this->state = GOOMBA_STATE_WALKING;
}

