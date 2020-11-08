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
	this->nx = -1;
	SetState(GOOMBA_STATE_PATROLING);

}

CGoomba::CGoomba() {
	SetState(GOOMBA_STATE_PATROLING);
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

	// Enemy AI in here
	switch (this->state)
	{
	case GOOMBA_STATE_DIE:
		//y += GOOMBA_NORMAL_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		this->isDeath = true;
		this->collisionState = NONE;
		break;
	case GOOMBA_STATE_PATROLING:
		if (this->x > this->maxPatrolX) {
			this->vx = -GOOMBA_WALKING_SPEED;
			this->nx = -1;
		}
		else if (this->x < this->minPatrolX) {
			this->vx = GOOMBA_WALKING_SPEED;
			this->nx = 1;
		}
		else {
			this->vx = GOOMBA_WALKING_SPEED * this->nx;
		}
		break;
	}
}

void CGoomba::OnCollisionEnter(LPCOLLISIONEVENT collision) {
	if (dynamic_cast<LPENEMY>(collision->obj)) {
		if (collision->ny > 0) {
			this->y -= 3;
		}
	}

	if (collision->nx != 0) {
		float colObjX, colObjY;
		collision->obj->GetPosition(colObjX, colObjY);
		if (abs(colObjY - this->y) < GOOMBA_NORMAL_BBOX_HEIGHT) {
			this->nx = -this->nx;
			this->vx = -this->vx;
		}
	}
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
	this->state = GOOMBA_STATE_PATROLING;
	this->nx = -1;
}



//# PARA-GOOMBA HERE
CParaGoomba::CParaGoomba() {
	this->hasWings = true;
	this->shortJumpStack = PARA_GOOMBA_SHORT_JUMP_STACK;
	this->patrolState = SHORT_JUMP;
}

CParaGoomba::CParaGoomba(float x, float y) {
	this->startX = x;
	this->startY = y;
	this->x = x;
	this->y = y;
	this->isDeath = false;
	this->isUsingGravity = true;
	this->collisionState = COLLISION;
	this->isStatic = false;
	SetState(GOOMBA_STATE_PATROLING);
	this->hasWings = true;
	this->shortJumpStack = PARA_GOOMBA_SHORT_JUMP_STACK;
	this->patrolState = SHORT_JUMP;
	this->statePatrolTimeStart = 0;
	this->nx = -1;
}

void CParaGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom){
	left = x;
	top = y;
	right = x + PARA_GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_NORMAL_BBOX_HEIGHT;
}

void CParaGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	//get collision
	CGameObject::Update(dt, coObjects);

	//lose wing => para goomba can not jump anymore
	if (!hasWings) {
		this->patrolState = WALKING;
	}
	else {
		//change patrol state
		if (patrolState == SHORT_JUMP) {
			if (GetTickCount() - statePatrolTimeStart > PARA_GOOMBA_SHORT_JUMP_TIME) {
				if (this->shortJumpStack > 0) {
					this->shortJumpStack--;
					this->vy = -PARA_GOOMBA_SHORT_JUMP_SPEED;
					statePatrolTimeStart = GetTickCount();
				}
				else {
					this->patrolState = HIGH_JUMP;
					this->vy = -PARA_GOOMBA_HIGH_JUMP_SPEED;
					this->shortJumpStack = PARA_GOOMBA_SHORT_JUMP_STACK;
					statePatrolTimeStart = GetTickCount();
				}
			}
		}
		else if (patrolState == HIGH_JUMP) {
			if (GetTickCount() - statePatrolTimeStart > PARA_GOOMBA_HIGH_JUMP_TIME) {
				this->patrolState = WALKING;
				statePatrolTimeStart = GetTickCount();
			}
		}
		else {
			if (GetTickCount() - statePatrolTimeStart > PARA_GOOMBA_WALKING_TIME) {
				this->patrolState = SHORT_JUMP;
				this->vy = -PARA_GOOMBA_SHORT_JUMP_SPEED;
				statePatrolTimeStart = GetTickCount();
			}
		}
	}

	

	switch (this->state) {
	case GOOMBA_STATE_DIE:
		//y += GOOMBA_NORMAL_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		this->isDeath = true;
		this->collisionState = NONE;
		break;
	case GOOMBA_STATE_PATROLING:
		if (this->x > this->maxPatrolX) {
			this->vx = -GOOMBA_WALKING_SPEED;
			this->nx = -1;
		}
		else if (this->x < this->minPatrolX) {
			this->vx = GOOMBA_WALKING_SPEED;
			this->nx = 1;
		}
		else {
			this->vx = GOOMBA_WALKING_SPEED * this->nx;
		}
		break; 
	}

}

void CParaGoomba::Render() {
	int ani = PARA_GOOMBA_ANI_NO_WING;
	if (isDeath) {
		ani = PARA_GOOMBA_ANI_DIE;
	}
	else if (patrolState != WALKING) {
		ani = PARA_GOOMBA_ANI_FLYING;
	} 
	else if (patrolState == WALKING && hasWings) {
		ani = PARA_GOOMBA_ANI_WING_WALKING;
	}

	//if goomba has wings, just draw askew a little bit because we don't care wing collision
	if (hasWings) {
		if (patrolState != WALKING) {
			animation_set->at(ani)->Render(x - 2, y - 8);
		}
		else {
			animation_set->at(ani)->Render(x - 2, y - 4);
		}
	}
	else
		animation_set->at(ani)->Render(x, y);
	//RenderBoundingBox();
}

void CParaGoomba::SetState(int state) {
	CGameObject::SetState(state);
}

void CParaGoomba::OnCollisionEnter(LPCOLLISIONEVENT collision) {
	if (dynamic_cast<LPENEMY>(collision->obj)) {
		if (collision->ny > 0) {
			this->y -= 10;
		}
	}

	if (collision->nx != 0) {
		float colObjX, colObjY;
		collision->obj->GetPosition(colObjX, colObjY);
		if (abs(colObjY - this->y) < GOOMBA_NORMAL_BBOX_HEIGHT) {
			this->nx = -this->nx;
			this->vx = -this->vx;
		}
	}
}

void CParaGoomba::OnTriggerEnter(LPCOLLISIONEVENT trigger) {

}

void CParaGoomba::Reset() {
	CEnemy::Reset();
	this->state = GOOMBA_STATE_PATROLING;
	this->nx = -1;
	this->hasWings = true;
	this->shortJumpStack = PARA_GOOMBA_SHORT_JUMP_STACK;
	this->patrolState = SHORT_JUMP;
	this->statePatrolTimeStart = 0;
}

