#include "Koopas.h"
#include "Mario.h"

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
			this->kickedCollisionStack = KOOPAS_KICKED_STACK;
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
		this->vx = this->vy = 0;
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
	this->isShellUp = false;
	this->shellingTimeStart = GetTickCount();
	this->y -= 10;
}

void CKoopas::ShellUp() {
	this->ShellDown();
	isShellUp = true;
}

void CKoopas::Render()
{
	int ani = 0;
	if (state == KOOPAS_STATE_BE_KICKED) {
		if (isShellUp) {
			ani = KOOPAS_ANI_SHELL_SPIN_UP + this->color;
		}
		else {
			ani = KOOPAS_ANI_SHELL_SPINNING + this->color;
		}
	}
	else if (state == KOOPAS_STATE_SHELL || state == KOOPAS_STATE_BE_HELD) {
		if (isShellUp) {
			ani = KOOPAS_ANI_SHELL_UP + this->color;
		}
		else {
			ani = KOOPAS_ANI_SHELL_DOWN + this->color;
		}
	}
	else if (state == KOOPAS_STATE_PATROL)
	{
		if (nx < 0) {
			if (hasWings) {
				ani = KOOPAS_ANI_FLYING_LEFT + this->color;
			}
			else {
				ani = KOOPAS_ANI_WALKING_LEFT + this->color;
			}
		}
		else if (nx > 0) {
			if (hasWings) {
				ani = KOOPAS_ANI_FLYING_RIGHT + this->color;
			}
			else {
				ani = KOOPAS_ANI_WALKING_RIGHT + this->color;
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
		this->vx = 0;
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
			this->y -= 3;
		}
	}

	if (dynamic_cast<CMario*>(collision->obj) && state == KOOPAS_STATE_SHELL) {
		CMario* player = dynamic_cast<CMario*>(collision->obj);
		float playerX, playerY, dir;
		player->GetPosition(playerX, playerY);
		dir = (this->x - playerX > 0) ? 1 : -1;
		this->BeKicked(dir);
		player->SetState(MARIO_STATE_KICK);
		/*koopas->SetSpeed(0.3f, -0.2f);
		koopas->SetState(KOOPAS_STATE_BE_HELD);*/
		return;
	}

	if (collision->nx != 0) {
		float colObjX, colObjY;
		collision->obj->GetPosition(colObjX, colObjY);
		if (dynamic_cast<LPENEMY>(collision->obj)) {
			LPENEMY enemy = dynamic_cast<LPENEMY>(collision->obj);
			enemy->Die();
		}
		else if (abs(colObjY - this->y) < KOOPAS_BBOX_HEIGHT && state == KOOPAS_STATE_BE_KICKED) {
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

void CKoopas::BeHeld() {
	SetState(KOOPAS_STATE_BE_HELD);
}