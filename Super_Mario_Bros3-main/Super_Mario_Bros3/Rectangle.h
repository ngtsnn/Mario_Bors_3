#pragma once
#include "GameObject.h"
#include <vector>

#define RECTANGLE_BBOX_WIDTH  16
#define RECTANGLE_BBOX_HEIGHT 16

class CRectangle : public CGameObject
{
public:
	CRectangle() { this->hasPlatformEffect = true; }
	//virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};