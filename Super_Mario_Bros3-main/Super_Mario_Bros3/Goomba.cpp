#include "Goomba.h"

CGoomba::CGoomba(float x, float y)
{
	SetState(GOOMBA_STATE_WALKING);
}

CGoomba::CGoomba() {
	SetState(GOOMBA_STATE_WALKING);
	this->nx = -1;
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_BBOX_HEIGHT;
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
		y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		this->collisionState = NONE;
		break;
	case GOOMBA_STATE_WALKING:
		if (this->vx == 0) {
			vx = -GOOMBA_WALKING_SPEED;
		}
		break;
	}
}

void CGoomba::OnCollisionEnter(LPCOLLISIONEVENT collision) {
	if (collision->nx != 0.0f)
		this->vx = -this->vx;
}

void CGoomba::OnTriggerEnter(LPCOLLISIONEVENT collision) {

}

void CGoomba::Render()
{
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE) {
		ani = GOOMBA_ANI_DIE;
	}

	animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	
}

