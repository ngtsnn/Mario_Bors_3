#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Portal.h"
#include "Enemy.h"
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
	this->koopas = NULL;
	this->runningStack = 0;
	this->runningStartTime = 0;
	this->collisionState = COLLISION;
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	
	//Do the collisions
	CGameObject::Update(dt, coObjects);

	if (state == MARIO_STATE_DIE) {
		return;
	}
	
	//reset running stack
	if (abs(this->vx) <= MARIO_WALKING_SPEED) {
		this->runningStack = 0;
	}

	//reset untouchable state
	if (GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	//reset kicking
	if (GetTickCount() - kickingStartTime > MARIO_KICKING_TIME)
	{
		kickingStartTime = 0;
		isKicking = false;
		SetState(MARIO_STATE_IDLE);
	}

	//reset tailing
	if (GetTickCount() - tailingStartTime > MARIO_TAILING_TIME)
	{
		isTailing = false;
		tailingStartTime = 0;
	}

	//reset jumping
	if (GetTickCount() - jumpingStartTime > MARIO_JUMP_TIME || this->vy > 0) {
		isJumping = false;
		jumpingStartTime = 0;
	}

	//holding //fix it later
	if (isHolding) {
		if (!canHold) {
			Release();
		}
		if (this->koopas) {
			if (this->koopas->GetState() == KOOPAS_STATE_PATROL) {
				LoseLevel();
				this->koopas = NULL;
				return;
			}
			float koopasX, koopasY;
			if (level == MARIO_LEVEL_SMALL) {

				if (this->nx > 0) {
					koopasX = this->x + (MARIO_SMALL_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}
				else {
					koopasX = this->x - (MARIO_SMALL_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}
				
			} 
			else if (level == MARIO_LEVEL_BIG) {
				if (this->nx > 0) {
					koopasX = this->x + (MARIO_BIG_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}
				else {
					koopasX = this->x - (MARIO_BIG_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}

			}
			else if (level == MARIO_LEVEL_TAIL) {
				if (this->nx > 0) {
					koopasX = this->x + (MARIO_TAIL_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}
				else {
					koopasX = this->x - (MARIO_TAIL_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}

			}
			else if (level == MARIO_LEVEL_FIRE) {
				if (this->nx > 0) {
					koopasX = this->x + (MARIO_FIRE_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}
				else {
					koopasX = this->x - (MARIO_FIRE_BBOX_WIDTH - 5);
					koopasY = this->y - 5;
				}

			}
			this->koopas->SetPosition(koopasX, koopasY);
		}

	}
	

}

void CMario::OnCollisionEnter(LPCOLLISIONEVENT collisionEvent) {
	LPCOLLISIONEVENT e = collisionEvent;
	if (e->ny < 0) {
		isGrounded = true;
	}
	else {
		isGrounded = false;
	}

	if (dynamic_cast<LPENEMY>(e->obj)) {
		if (e->ny > 0) {
			this->y -= 5;
		}

	}

	if (dynamic_cast<CParaGoomba*>(e->obj)) // if e->obj is Goomba 
	{
		CParaGoomba* goomba = dynamic_cast<CParaGoomba*>(e->obj);

		//kill by tail (mario face the enemy can kill)
		if (this->isTailing && e->ny <= 0 && e->nx * this->nx < 0 && abs((long)(GetTickCount() - tailingStartTime - MARIO_TAILING_TIME / 2)) <= 60) {
			goomba->SetState(GOOMBA_STATE_DIE);
			goomba->SetSpeed(this->nx * GOOMBA_WALKING_SPEED * 2, -GOOMBA_DIE_DEFLECT_SPEED);
			return;
		}

		// jump on top >> kill Goomba and deflect a bit 
		else if (e->ny < 0)
		{
			if (goomba->HasWings())
			{
				goomba->LoseWings();
				vy = -MARIO_JUMP_DEFLECT_SPEED * 2;
				this->vx = this->nx * 0.15f;
			}
			else {
				goomba->SetState(GOOMBA_STATE_DIE);
				vy = -MARIO_JUMP_DEFLECT_SPEED;
			}
		}

		else if (e->nx != 0)
		{
			if (untouchable == 0)
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

	//normal goomba
	else if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
	{
		CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

		//kill by tail (mario face the enemy can kill)
		if (this->isTailing && e->ny <= 0 && e->nx * this->nx < 0 && abs((long)(GetTickCount() - tailingStartTime - MARIO_TAILING_TIME / 2)) <= 60) {
			goomba->SetState(GOOMBA_STATE_DIE);
			goomba->SetSpeed(this->nx * GOOMBA_WALKING_SPEED * 2, -GOOMBA_DIE_DEFLECT_SPEED);
			return;
		}

		// jump on top >> kill Goomba and deflect a bit 
		else if (e->ny < 0)
		{
			goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}

		else
		{
			this->LoseLevel();
		}
	} // if Goomba

	//Koopas
	else if (dynamic_cast<CKoopas*>(e->obj)) {
		CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);

		//kill by tail (mario face the enemy can kill)
		if (this->isTailing && e->ny <= 0 && e->nx * this->nx < 0 && abs((long)(GetTickCount() - tailingStartTime - MARIO_TAILING_TIME / 2)) <= 60) {
			koopas->ShellUp();
			koopas->SetSpeed(this->nx * KOOPAS_WALKING_SPEED * 2, -KOOPAS_DIE_DEFLECT_SPEED);
			return;
		}
		else if (e->ny < 0) {
			if (!koopas->IsShelling()) {
				if (koopas->HasWings()) {
					koopas->LoseWings();

				}
				else {
					koopas->ShellDown();
				}
				vy = -MARIO_JUMP_DEFLECT_SPEED;
				this->vx = this->nx * 0.1f;
				return;
			}
		}

		else {
			LoseLevel();
		}
	}//koopas

	else if (dynamic_cast<CPortal*>(e->obj))
	{
		CPortal* p = dynamic_cast<CPortal*>(e->obj);
		CGame::GetInstance()->SwitchScene(p->GetSceneId());
	}
}

void CMario::OnTriggerEnter(LPCOLLISIONEVENT triggerEvent) {
	if (dynamic_cast<CEnemySpawner*>(triggerEvent->obj)) {
		CEnemySpawner* spawner = dynamic_cast<CEnemySpawner*>(triggerEvent->obj);
		spawner->SpawnEnemy();
	}
}

void CMario::Render()
{
	int ani = -1;
	//DIE
	if (this->state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	//else if (vy != 0) {
	//	ani = MARIO_ANI_SMALL_JUMPING_RIGHT;
	//} //JUMP

	//HOLDING
	else if (isHolding) {
		if (level == MARIO_LEVEL_BIG) {
			if (nx < 0) {
				ani = MARIO_ANI_BIG_HOLDING_LEFT;
			}
			else {
				ani = MARIO_ANI_BIG_HOLDING_RIGHT;
			}
		}
		else if (level == MARIO_LEVEL_SMALL) {
			if (nx < 0) {
				ani = MARIO_ANI_SMALL_HOLDING_LEFT;
			}
			else {
				ani = MARIO_ANI_SMALL_HOLDING_RIGHT;
			}
		}
		else if (level == MARIO_LEVEL_TAIL) {
			if (nx < 0) {
				ani = MARIO_ANI_TAIL_HOLDING_LEFT;
			}
			else {
				ani = MARIO_ANI_TAIL_HOLDING_RIGHT;
			}
		}
		else if (level == MARIO_LEVEL_FIRE) {
			if (nx < 0) {
				ani = MARIO_ANI_FIRE_HOLDING_LEFT;
			}
			else {
				ani = MARIO_ANI_FIRE_HOLDING_RIGHT;
			}
		}
	} //HOLDING

	//TALING
	else if (isTailing) {
		if (this->nx < 0)
			ani = MARIO_ANI_TAIL_TAILING_LEFT;
		else
			ani = MARIO_ANI_TAIL_TAILING_RIGHT;
	} //TAILING

	//KICKING
	else if (isKicking) {
		if (level == MARIO_LEVEL_BIG) {
			if (nx < 0) {
				ani = MARIO_ANI_BIG_KICKING_LEFT;
			}
			else {
				ani = MARIO_ANI_BIG_KICKING_RIGHT;
			}
		} 
		else if (level == MARIO_LEVEL_SMALL) {
			if (nx < 0) {
				ani = MARIO_ANI_SMALL_KICKING_LEFT;
			}
			else {
				ani = MARIO_ANI_SMALL_KICKING_RIGHT;
			}
		}
		else if (level == MARIO_LEVEL_TAIL) {
			if (nx < 0) {
				ani = MARIO_ANI_TAIL_KICKING_LEFT;
			}
			else {
				ani = MARIO_ANI_TAIL_KICKING_RIGHT;
			}
		}
		else if (level == MARIO_LEVEL_FIRE) {
			if (nx < 0) {
				ani = MARIO_ANI_FIRE_KICKING_LEFT;
			}
			else {
				ani = MARIO_ANI_FIRE_KICKING_RIGHT;
			}
		}
	}

	//MOVING
	else {
		if (level == MARIO_LEVEL_BIG)
		{
			if (abs(vx) <= .02f)
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
					else if (vx > MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_BIG_RUNNING_RIGHT;
					}
					else {
						ani = MARIO_ANI_BIG_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_BIG_BRAKING_LEFT;
					}
					else if (vx < -MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_BIG_RUNNING_LEFT;
					}
					else {
						ani = MARIO_ANI_BIG_WALKING_LEFT;
					}
				}
			}
		} // BIG
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (abs(vx) <= .02f)
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
					else if (vx > MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_SMALL_RUNNING_RIGHT;
					}
					else {
						ani = MARIO_ANI_SMALL_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_SMALL_BRAKING_LEFT;
					}
					else if (vx < -MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_SMALL_RUNNING_LEFT;
					}
					else {
						ani = MARIO_ANI_SMALL_WALKING_LEFT;
					}
				}
			}
		} //SMALL
		else if (level == MARIO_LEVEL_TAIL)
		{
			if (abs(vx) <= .02f)
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
					else if (vx > MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_TAIL_RUNNING_RIGHT;
					}
					else {
						ani = MARIO_ANI_TAIL_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_TAIL_BRAKING_LEFT;
					}
					else if (vx < -MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_TAIL_RUNNING_LEFT;
					}
					else {
						ani = MARIO_ANI_TAIL_WALKING_LEFT;
					}
				}
			}
		} //TAIL
		else if (level == MARIO_LEVEL_FIRE)
		{
			if (abs(vx) <= .02f)
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
					else if (vx > MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_FIRE_RUNNING_RIGHT;
					}
					else {
						ani = MARIO_ANI_FIRE_WALKING_RIGHT;
					}
				}
				else {
					if (this->state == MARIO_STATE_BRAKING_LEFT) {
						ani = MARIO_ANI_FIRE_BRAKING_LEFT;
					}
					else if (vx < -MARIO_WALKING_SPEED) {
						ani = MARIO_ANI_FIRE_RUNNING_LEFT;
					}
					else {
						ani = MARIO_ANI_FIRE_WALKING_LEFT;
					}
				}
			}
		} //FIRE
	} //MOVING

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
		this->vx += (MARIO_ACCELERATION * dt * .2f);
		break;
	case MARIO_STATE_WALKING_RIGHT:
		//move faster in time
		this->vx += (MARIO_ACCELERATION * dt * .4f);	
		//clamp the velocity
		this->vx = this->Clamp(this->vx, -MARIO_WALKING_SPEED, MARIO_WALKING_SPEED);
		nx = 1;
		break;
	case MARIO_STATE_RUNNING_RIGHT:
		//move faster in time
		this->vx += (MARIO_ACCELERATION * dt);
		//clamp the velocity
		this->vx = this->Clamp(this->vx, -MARIO_RUNNING_SPEED, MARIO_RUNNING_SPEED);
		nx = 1;
		if (GetTickCount() - this->runningStartTime >= MARIO_RUNNING_STACK_TIME) {
			this->runningStack++;
			this->runningStartTime = GetTickCount();
		}
		break;
	case MARIO_STATE_BRAKING_RIGHT:
		this->vx += (-MARIO_ACCELERATION * dt * .5f);
		this->runningStartTime = 0;
		break;
	case MARIO_STATE_WALKING_LEFT:
		//move faster in time
		this->vx += (-MARIO_ACCELERATION * dt);
		//clamp the velocity
		this->vx = this->Clamp(this->vx, -MARIO_WALKING_SPEED, MARIO_WALKING_SPEED);
		nx = -1;
		break;
	case MARIO_STATE_RUNNING_LEFT:
		//move faster in time
		this->vx += (-MARIO_ACCELERATION * dt);
		//clamp the velocity
		this->vx = this->Clamp(this->vx, -MARIO_RUNNING_SPEED, MARIO_RUNNING_SPEED);
		nx = -1;
		break;
		if (GetTickCount() - this->runningStartTime >= MARIO_RUNNING_STACK_TIME) {
			this->runningStack++;
			this->runningStartTime = GetTickCount();
		}
		break;
	case MARIO_STATE_JUMP:
		//if (this->vy <= 0.03f && this->vy > -0.03f)
		if (this->isGrounded == true) {
			vy = -MARIO_JUMP_ACCELERATION * dt;
			this->isGrounded = false;
			isJumping = true;
			jumpingStartTime = GetTickCount();
		}
		else if (isJumping) {
			vy = -MARIO_JUMP_ACCELERATION * dt;
			
		}

		break;
	case MARIO_STATE_IDLE:
		this->vx -= this->vx * MARIO_MUY_FRICTION * dt * 1.2f;
		break;
	case MARIO_STATE_TURNING_TAIL:
		this->vx = 0;
		this->isTailing = true;
		this->tailingStartTime = GetTickCount();
		break;
	case MARIO_STATE_KICK:
		this->isKicking = true;
		this->vx = 0;
		this->kickingStartTime = GetTickCount();
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		this->runningStack = 0;
		this->runningStartTime = 0;
		this->collisionState = NONE;
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
	this->runningStack = 0;
	this->runningStartTime = 0;
	this->isUsingGravity = true;
	this->isStatic = false;
	this->koopas = NULL;
	this->collisionState = COLLISION;
}

float CMario::Clamp(float value, float min, float max) {
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

void CMario::Hold(CKoopas* koopas) {
	if (!this->koopas) {
		this->koopas = koopas;
	}

	float koopasX, koopasY;
	if (level == MARIO_LEVEL_SMALL) {
		if (this->nx > 0) {
			koopasX = this->x + (MARIO_SMALL_BBOX_WIDTH - 5);
			koopasY = this->y - 5;
		}
		else {
			koopasX = this->x - (MARIO_SMALL_BBOX_WIDTH - 5);
			koopasY = this->y - 5;
		}

	}
	else if (level == MARIO_LEVEL_BIG) {
		if (this->nx > 0) {
			koopasX = this->x + (MARIO_BIG_BBOX_WIDTH - 5);
			koopasY = this->y - 2;
		}
		else {
			koopasX = this->x - (MARIO_BIG_BBOX_WIDTH - 5);
			koopasY = this->y - 5;
		}

	}
	else if (level == MARIO_LEVEL_TAIL) {
		if (this->nx > 0) {
			koopasX = this->x + (MARIO_TAIL_BBOX_WIDTH - 5);
			koopasY = this->y - 5;
		}
		else {
			koopasX = this->x - (MARIO_TAIL_BBOX_WIDTH - 5);
			koopasY = this->y - 5;
		}

	}
	else if (level == MARIO_LEVEL_FIRE) {
		if (this->nx > 0) {
			koopasX = this->x + (MARIO_FIRE_BBOX_WIDTH - 5);
			koopasY = this->y - 5;
		}
		else {
			koopasX = this->x - (MARIO_FIRE_BBOX_WIDTH - 5);
			koopasY = this->y - 5;
		}

	}
	this->koopas->SetPosition(koopasX, koopasY);
	this->isHolding = true;

	
}

void CMario::Release() {
	this->koopas->BeKicked(this->nx);
	this->koopas = NULL;
	this->isHolding = false;
}

void CMario::Kick(int dir) {
	SetState(MARIO_STATE_KICK);
	this->nx = dir;
}

void CMario::LoseLevel() {
	if (untouchable == 0)
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

