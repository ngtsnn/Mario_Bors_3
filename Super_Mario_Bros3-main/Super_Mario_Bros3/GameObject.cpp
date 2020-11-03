#include <d3dx9.h>
#include <algorithm>


#include "Utils.h"
#include "Textures.h"
#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"

CGameObject::CGameObject()
{
	this->x = this->y = 0;
	this->vx = this->vy = 0;
	this->nx = 1;
	this->collisionState = COLLISION;
	this->isUsingGravity = false;
	this->hasPlatformEffect = false;
	this->isStatic = true;
}

void CGameObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	this->dt = dt;
	dx = vx * dt;
	dy = vy * dt;


	// Simple fall down
	if (this->isUsingGravity)
		this->vy += GRAVITY * dt;

	//If it's not collision object => just skip
	if (this->collisionState == NONE || this->isStatic) {
		this->x += dx;
		this->y += dy;
		return;
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	vector<LPCOLLISIONEVENT> trgEvents;
	vector<LPCOLLISIONEVENT> trgEventsResult;

	coEvents.clear();
	trgEvents.clear();

	// turn off collision when die 
	CalcPotentialCollisions(coObjects, coEvents, trgEvents);

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


		if (nbx != 0) vx = 0;
		if (nby != 0) vy = 0;

		for (int i = 0; i < coEventsResult.size(); i++) {
			//user can execute by using OnCollisionEnter
			this->OnCollisionEnter(coEventsResult[i]);
		}
	}

	if (trgEvents.size() != 0) {
		float min_tx, min_ty, nbx = 0, nby;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(trgEvents, trgEventsResult, min_tx, min_ty, nbx, nby, rdx, rdy);
		for (int i = 0; i < trgEventsResult.size(); i++) {
			//user can execute by using OnTriggerEnter
			this->OnTriggerEnter(trgEventsResult[i]);
		}
	}

	//clean up trigger events and collison events
	//for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	//for (UINT i = 0; i < coEvents.size(); i++) delete trgEvents[i];
}

/*
	Extension of original SweptAABB to deal with two moving objects
*/
LPCOLLISIONEVENT CGameObject::SweptAABBEx(LPGAMEOBJECT coO)
{

	float sl, st, sr, sb;		// static object bbox
	float ml, mt, mr, mb;		// moving object bbox
	float t, nx, ny;

	coO->GetBoundingBox(sl, st, sr, sb);

	// deal with moving object: m speed = original m speed - collide object speed
	float svx, svy;
	coO->GetSpeed(svx, svy);

	float sdx = svx * dt;
	float sdy = svy * dt;

	// (rdx, rdy) is RELATIVE movement distance/velocity 
	float rdx = this->dx - sdx;
	float rdy = this->dy - sdy;

	GetBoundingBox(ml, mt, mr, mb);

	CGame::SweptAABB(
		ml, mt, mr, mb,
		rdx, rdy,
		sl, st, sr, sb,
		t, nx, ny
	);

	CCollisionEvent* e = new CCollisionEvent(t, nx, ny, rdx, rdy, coO);
	return e;
}

/*
	Calculate potential collisions with the list of colliable objects

	coObjects: the list of colliable objects
	coEvents: list of potential collisions
*/
void CGameObject::CalcPotentialCollisions(
	vector<LPGAMEOBJECT>* coObjects,
	vector<LPCOLLISIONEVENT>& coEvents, vector<LPCOLLISIONEVENT>& trgEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (coObjects->at(i)->GetCollisionState() == NONE || this->collisionState == NONE) {
			continue;
		}

		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (coObjects->at(i)->hasPlatformEffect && (e->nx != 0 || e->ny >= 0)){
			delete e;
			continue;
		}

		if (e->t > 0 && e->t <= 1.0f) {
			if (coObjects->at(i)->GetCollisionState() == COLLISION && this->collisionState == COLLISION) {
				coEvents.push_back(e);
			}
			else if(coObjects->at(i)->GetCollisionState() == TRIGGER || this->collisionState == TRIGGER){
				trgEvents.push_back(e);
			}
		}
			
		else
			delete e;
	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CGameObject::FilterCollision(
	vector<LPCOLLISIONEVENT>& coEvents,
	vector<LPCOLLISIONEVENT>& coEventsResult,
	float& min_tx, float& min_ty,
	float& nx, float& ny, float& rdx, float& rdy)
{
	min_tx = 1.0f;
	min_ty = 1.0f;
	int min_ix = -1;
	int min_iy = -1;

	nx = 0.0f;
	ny = 0.0f;

	coEventsResult.clear();

	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];

		if (c->t < min_tx && c->nx != 0) {
			min_tx = c->t; nx = c->nx; min_ix = i; rdx = c->dx;
		}

		if (c->t < min_ty && c->ny != 0) {
			min_ty = c->t; ny = c->ny; min_iy = i; rdy = c->dy;
		}
	}

	if (min_ix >= 0) coEventsResult.push_back(coEvents[min_ix]);
	if (min_iy >= 0) coEventsResult.push_back(coEvents[min_iy]);
}


void CGameObject::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPDIRECT3DTEXTURE9 bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	CGame::GetInstance()->Draw(x, y, bbox, rect.left, rect.top, rect.right, rect.bottom, 32);
}


CGameObject::~CGameObject()
{

}