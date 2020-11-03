#pragma once
#include "NoCollisionObject.h"

CNoColObj::CNoColObj() {
	this->collisionState = NONE;
}

void CNoColObj::Render() {
	this->animation_set->at(0)->Render(this->x, this->y);
}

void CNoColObj::GetBoundingBox(float& l, float& t, float& r, float& b) {
	
}