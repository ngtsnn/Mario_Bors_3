#pragma once
#include "Pipe.h"

void CPipe::Render() {
	this->animation_set->at(0)->Render(this->x, this->y);
}

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = this->x;
	t = this->y;
	r = this->x + PIPE_BBOX_HEIGHT;
	b = this->y + PIPE_BBOX_WIDTH;
}
