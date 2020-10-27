#pragma once
#include "GameObject.h"
#include <vector>

#define PIPE_BBOX_WIDTH		16.0f
#define PIPE_BBOX_HEIGHT	16.0f

class CPipe : public CGameObject
{
public:
	//virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};