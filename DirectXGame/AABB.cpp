#include "AABB.h"

void AABB::CalculateAABB(const Vector3& center, float radius, Vector3& outMin, Vector3& outMax) {
	outMin = {center.x - radius, center.y - radius, center.z - radius};
	outMax = {center.x + radius, center.y + radius, center.z + radius};
}

//void AABB::Iscollision(const float aabb1, const float aabb2) 
//{
//
//}
