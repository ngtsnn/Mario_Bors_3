#pragma once
#include "GameObject.h"
#define NOCOL_BBOX_WIDTH	16.0f
#define NOCOL_BBOX_HEIGHT	16.0f


class CNoColObj : public CGameObject
{
public:
	//void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};