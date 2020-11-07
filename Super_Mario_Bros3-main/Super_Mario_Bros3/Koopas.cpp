#include "Koopas.h"

CKoopas::CKoopas()
{
	SetState(KOOPAS_STATE_PATROL);
	this->nx = -1;
	this->isDeath = false;
}

CKoopas::CKoopas(float x, float y, int color, bool isPara) {
	this->startX = x;
	this->startY = y;
	this->x = x;
	this->y = y;
	this->isDeath = false;
	this->isUsingGravity = true;
	this->collisionState = COLLISION;
	this->isStatic = false;
	SetState(KOOPAS_STATE_PATROL);
	this->nx = -1;
	this->kickedCollisionStack = KOOPAS_KICKED_STACK;
	this->isPara = isPara;
	this->color = color;
	if (isPara) {
		this->hasWings = true;
		this->patrolState = HIGH_JUMP;
	}
	else {
		this->hasWings = false;
		this->patrolState = WALKING;
	}


}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;

	if (state != KOOPAS_STATE_PATROL)
		bottom = y + KOOPAS_BBOX_HEIGHT_SHELL;
	else
		bottom = y + KOOPAS_BBOX_HEIGHT;
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	if (GetTickCount() - this->shellingTimeStart > KOOPAS_SHELLING_TIME) {
		if (isShelling) {
			isShelling = false;
			state = KOOPAS_STATE_PATROL;
			this->y -= (KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT_SHELL + 3);
		}
	}

	if (patrolState == HIGH_JUMP) {
		if (GetTickCount() - jumpingTimeStart > KOOPAS_JUMPING_TIME) {
			jumpingTimeStart = GetTickCount();
			this->vy = -KOOPAS_JUMPING_SPEED;
		}
	}
	
	switch (state) {
	case KOOPAS_STATE_BE_HELD:
		this->isBeingHeld = true;
		break;
	case KOOPAS_STATE_BE_KICKED:
		this->isKicked = true;
		this->vx = this->nx * KOOPAS_KICKED_SPEED;
		break;
	case KOOPAS_STATE_SHELL:
		this->isShelling = true;
		this->vx = 0;
		break;
	case KOOPAS_STATE_PATROL:
		if (this->x > this->maxPatrolX) {
			this->vx = -KOOPAS_WALKING_SPEED;
			this->nx = -1;
		}
		else if (this->x < this->minPatrolX) {
			this->vx = KOOPAS_WALKING_SPEED;
			this->nx = 1;
		}
		else {
			this->vx = KOOPAS_WALKING_SPEED * this->nx;
		}
		break;
	}

}

void CKoopas::ShellDown() 
{ 
	this->SetState(KOOPAS_STATE_SHELL); 
	this->shellingTimeStart = GetTickCount();
	this->y -= 10;
}

void CKoopas::ShellUp() {
	if (this->color == KOOPAS_COLOR_GREEN) {
		isShellUp = true;
	}
	this->ShellDown();
	
}

void CKoopas::Render()
{
	int ani = 0;
	if (state == KOOPAS_STATE_BE_KICKED) {
		if (color == KOOPAS_COLOR_GREEN) {
			ani = KOOPAS_GREEN_ANI_SHELL_SPINNING;
		}
		else if(color == KOOPAS_COLOR_RED){
			ani = KOOPAS_RED_ANI_SHELL_SPINNING;
		}
	}
	else if (state == KOOPAS_STATE_SHELL || state == KOOPAS_STATE_BE_HELD) {
		if (isShellUp) {
			ani = KOOPAS_GREEN_ANI_SHELL_UP;
		}
		else if (color == KOOPAS_COLOR_GREEN) {
			ani = KOOPAS_GREEN_ANI_SHELL_DOWN;
		}
		else if (color == KOOPAS_COLOR_RED) {
			ani = KOOPAS_RED_ANI_SHELL_DOWN;
		}
	}
	else if (state == KOOPAS_STATE_PATROL)
	{
		if (nx < 0) {
			if (hasWings) {
				if (color == KOOPAS_COLOR_GREEN) {
					ani = KOOPAS_GREEN_ANI_FLYING_LEFT;
				}
				else if (color == KOOPAS_COLOR_RED) {
					ani = KOOPAS_RED_ANI_FLYING_LEFT;
				}
			}
			else {
				if (color == KOOPAS_COLOR_GREEN) {
					ani = KOOPAS_GREEN_ANI_WALKING_LEFT;
				}
				else if (color == KOOPAS_COLOR_RED) {
					ani = KOOPAS_RED_ANI_WALKING_LEFT;
				}
			}
		}
		else if (nx > 0) {
			if (hasWings) {
				if (color == KOOPAS_COLOR_GREEN) {
					ani = KOOPAS_GREEN_ANI_FLYING_RIGHT;
				}
				else if (color == KOOPAS_COLOR_RED) {
					ani = KOOPAS_RED_ANI_FLYING_RIGHT;
				}
			}
			else {
				if (color == KOOPAS_COLOR_GREEN) {
					ani = KOOPAS_GREEN_ANI_WALKING_RIGHT;
				}
				else if (color == KOOPAS_COLOR_RED) {
					ani = KOOPAS_RED_ANI_WALKING_RIGHT;
				}
			}
		}
		
	}

	animation_set->at(ani)->Render(x, y);

	RenderBoundingBox();
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state) {
	case KOOPAS_STATE_BE_HELD:
		this->isBeingHeld = true;
		break;
	case KOOPAS_STATE_BE_KICKED:
		this->isKicked = true;
		this->vx = this->nx * KOOPAS_KICKED_SPEED;
		break;
	case KOOPAS_STATE_SHELL:
		this->isShelling = true;
		this->vx = 0;
		break;
	case KOOPAS_STATE_PATROL:
		if (this->x > this->maxPatrolX) {
			this->vx = -KOOPAS_WALKING_SPEED;
			this->nx = -1;
		}
		else if (this->x < this->minPatrolX) {
			this->vx = KOOPAS_WALKING_SPEED;
			this->nx = 1;
		}
		else {
			this->vx = KOOPAS_WALKING_SPEED * this->nx;
		}
		break;
	}
}

void CKoopas::OnCollisionEnter(LPCOLLISIONEVENT collision) {
	if (dynamic_cast<LPENEMY>(collision->obj)) {
		if (collision->ny > 0) {
			this->y -= 10;
		}
	}
	if (collision->nx != 0) {
		float colObjX, colObjY;
		collision->obj->GetPosition(colObjX, colObjY);
		if (abs(colObjY - this->y) < KOOPAS_BBOX_HEIGHT) {
			this->nx = -this->nx;
			this->vx = -this->vx;
			this->kickedCollisionStack--;
			if (kickedCollisionStack == 0) {
				isDeath = true;
				collisionState = NONE;
			}
		}
	}
}

void CKoopas::OnTriggerEnter(LPCOLLISIONEVENT trigger) {

}

void CKoopas::Reset() {
	CEnemy::Reset();
	this->nx = -1;
	this->kickedCollisionStack = KOOPAS_KICKED_STACK;
	if (isPara) {
		this->hasWings = true;
		this->patrolState = HIGH_JUMP;
	}
	else {
		this->hasWings = false;
		this->patrolState = WALKING;
	}
	SetState(KOOPAS_STATE_PATROL);
}

void CKoopas::LoseWings() {
	this->hasWings = false;
	this->patrolState = WALKING;
	this->y -= 10;
}

void CKoopas::BeKicked(int dir) {
	this->nx = dir;
	SetState(KOOPAS_STATE_BE_KICKED);
	//this->vx = nx * KOOPAS_KICKED_SPEED;
}