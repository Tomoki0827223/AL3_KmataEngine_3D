#pragma once

#include "Vector3.h"

class AABB {
public:
	Vector3 min;
	Vector3 max;
	void CalculateAABB(const Vector3& center, float radius, Vector3& outMin, Vector3& outMax);
	//void Iscollision(const float aabb1, const float aabb2);
};
