#include "Rectangle.h"

void CRectangle::Render() {
	this->animation_set->at(0)->Render(this->x, this->y);
}

void CRectangle::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = this->x;
	t = this->y;
	r = this->x + RECTANGLE_BBOX_HEIGHT;
	b = this->y + RECTANGLE_BBOX_WIDTH;
}