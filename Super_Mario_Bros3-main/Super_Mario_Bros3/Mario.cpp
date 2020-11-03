#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Portal.h"

#include "Rectangle.h"

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);

	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
	this->nx = 1;
	this->isGrounded = false;
	this->isUsingGravity = true;
	this->isStatic = false;
	this->collisionState = COLLISION;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	//Do the collisions
	CGameObject::Update(dt, coObjects);

}

void CMario::OnCollisionEnter(LPCOLLISIONEVENT collisionEvent) {
	LPCOLLISIONEVENT e = collisionEvent;
	if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
	{
		CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

		// jump on top >> kill Goomba and deflect a bit 
		if (e->ny < 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				goomba->SetState(GOOMBA_STATE_DIE);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
			}
		}
		else if (e->nx != 0)
		{
			if (untouchable == 0)
			{
				if (goomba->GetState() != GOOMBA_STATE_DIE)
				{
					if (level > MARIO_LEVEL_SMALL)
					{
						this->level--;
						StartUntouchable();
					}
					else
						SetState(MARIO_STATE_DIE);
				}
			}
		}
	} // if Goomba
	else if (dynamic_cast<CPortal*>(e->obj))
	{
		CPortal* p = dynamic_cast<CPortal*>(e->obj);
		CGame::GetInstance()->SwitchScene(p->GetSceneId());
	}
}

void CMario::OnTriggerEnter(LPCOLLISIONEVENT triggerEvent) {

}

void CMario::Render()
{
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else
		if (level == MARIO_LEVEL_BIG)
		{
			if (abs(vx) <= .05f)
			{
				if (nx > 0)
					ani = MARIO_ANI_BIG_IDLE_RIGHT;
				else
					ani = MARIO_ANI_BIG_IDLE_LEFT;
			}
			else
			{
				if (vx > 0) {
					if (this->state == MARIO_STATE_BRAKING_RIGHT) {
						ani = MARIO_ANI_BIG_BRAKING_RIGHT;
					}
					else {
						ani = MARIO_ANI_BIG_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_BIG_BRAKING_LEFT;
					}
					else {
						ani = MARIO_ANI_BIG_WALKING_LEFT;
					}
				}
			}
		} // BIG
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (abs(vx) <= .05f)
			{
				if (nx > 0)
					ani = MARIO_ANI_SMALL_IDLE_RIGHT;
				else
					ani = MARIO_ANI_SMALL_IDLE_LEFT;
			}
			else
			{
				if (vx > 0) {
					if (this->state == MARIO_STATE_BRAKING_RIGHT) {
						ani = MARIO_ANI_SMALL_BRAKING_RIGHT;
					}
					else {
						ani = MARIO_ANI_SMALL_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_SMALL_BRAKING_LEFT;
					}
					else {
						ani = MARIO_ANI_SMALL_WALKING_LEFT;
					}
				}
			}
		} //SMALL
		else if (level == MARIO_LEVEL_TAIL)
		{
			if (abs(vx) <= .05f)
			{
				if (nx > 0)
					ani = MARIO_ANI_TAIL_IDLE_RIGHT;
				else
					ani = MARIO_ANI_TAIL_IDLE_LEFT;
			}
			else
			{
				if (vx > 0) {
					if (this->state == MARIO_STATE_BRAKING_RIGHT) {
						ani = MARIO_ANI_TAIL_BRAKING_RIGHT;
					}
					else {
						ani = MARIO_ANI_TAIL_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_TAIL_BRAKING_LEFT;
					}
					else {
						ani = MARIO_ANI_TAIL_WALKING_LEFT;
					}
				}
			}
		} //TAIL
		else if (level == MARIO_LEVEL_FIRE)
		{
			if (abs(vx) <= .05f)
			{
				if (nx > 0)
					ani = MARIO_ANI_FIRE_IDLE_RIGHT;
				else
					ani = MARIO_ANI_FIRE_IDLE_LEFT;
			}
			else
			{
				if (vx > 0) {
					if (this->state == MARIO_STATE_BRAKING_RIGHT) {
						ani = MARIO_ANI_FIRE_BRAKING_RIGHT;
					}
					else {
						ani = MARIO_ANI_FIRE_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_FIRE_BRAKING_LEFT;
					}
					else {
						ani = MARIO_ANI_FIRE_WALKING_LEFT;
					}
				}
			}
		} //FIRE

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(x, y, alpha);

	//RenderBoundingBox();
}

void CMario::SetLevel(int l) {
	if (this->level == MARIO_LEVEL_SMALL && l != MARIO_LEVEL_SMALL) {
		//make Mario be higher to avoid collison
		this->y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT + 3);
	}
	this->level = l;
}

void CMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case MARIO_STATE_BRAKING_LEFT:
		this->vx += (MARIO_ACCELERATION * dt * .09f);
		break;
	case MARIO_STATE_WALKING_RIGHT:
		//move faster in time
		this->vx += (MARIO_ACCELERATION * dt * .3f);	
		//clamp the velocity
		this->vx = this->Clamp(this->vx, -MARIO_WALKING_SPEED, MARIO_WALKING_SPEED);
		nx = 1;
		break;
	case MARIO_STATE_BRAKING_RIGHT:
		this->vx += (-MARIO_ACCELERATION * dt * .3f);
		break;
	case MARIO_STATE_WALKING_LEFT:
		//move faster in time
		this->vx += (-MARIO_ACCELERATION * dt);
		//clamp the velocity
		this->vx = this->Clamp(this->vx, -MARIO_WALKING_SPEED, MARIO_WALKING_SPEED);
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		//if (this->vy <= 0.03f && this->vy > -0.03f)
		//if (this->isGrounded == 1)
		vy = -MARIO_JUMP_SPEED_Y;
		break;
	case MARIO_STATE_IDLE:
		this->vx -= this->vx * MARIO_MUY_FRICTION * dt;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		this->collisionState = TRIGGER;
		break;
	}
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else if(this->level == MARIO_LEVEL_SMALL)
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
	else if(this->level == MARIO_LEVEL_TAIL) {
		right = this->x + MARIO_TAIL_BBOX_WIDTH;
		bottom = this->y + MARIO_TAIL_BBOX_HEIGHT;
	}
	else if (this->level == MARIO_LEVEL_FIRE) {
		right = this->x + MARIO_FIRE_BBOX_WIDTH;
		bottom = this->y + MARIO_FIRE_BBOX_HEIGHT;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_SMALL);
	SetPosition(start_x, start_y);
	this->nx = 1;
	SetSpeed(0, 0);
	this->isGrounded = false;
	this->isUsingGravity = true;
	this->isStatic = false;
	this->collisionState = COLLISION;
}

float CMario::Clamp(float value, float min, float max) {
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

