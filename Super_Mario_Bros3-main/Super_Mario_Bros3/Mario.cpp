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
}

void CMario::CalcPotentialCollisions(
		vector<LPGAMEOBJECT> *coObjects,
		vector<LPCOLLISIONEVENT> &coEvents) 
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (!coObjects->at(i)->HasCollision()) {
			continue;
		}

		LPCOLLISIONEVENT event = SweptAABBEx(coObjects->at(i));

		//check if mario will collide Rectangle in down, right, left side 
		//=> we skip this event. I will optimise it later.
		if (dynamic_cast<CRectangle*>(coObjects->at(i)) && (event->ny > 0 || event->nx != 0)) {
			delete event;
			continue;
		}

		if (event->t > 0 && event->t <= 1.0f)
		{
			coEvents.push_back(event);
		}
		else
		{
			delete event;
		}
	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nbx = 0, nby;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nbx, nby, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		/*if (rdx != 0 && rdx!=dx)
			x += nx*abs(rdx);*/ 

		// block every object first!
		x += min_tx * dx + nbx * 0.4f;
		y += min_ty * dy + nby * 0.4f;

		//check Mario is on the ground
		if (nby < 0) {
			this->isGrounded = true;
		}
		else {
			this->isGrounded = false;
		}

		if (nbx != 0) vx = 0;
		if (nby != 0) vy = 0;


		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

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
								level = MARIO_LEVEL_SMALL;
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
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
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

	RenderBoundingBox();
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
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		vy = -MARIO_JUMP_SPEED_Y;
		break;
	case MARIO_STATE_IDLE:
		this->vx -= this->vx * MARIO_MUY_FRICTION * dt;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
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
}

float CMario::Clamp(float value, float min, float max) {
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

