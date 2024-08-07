#include "MathUtilityForText.h"
#include "affine.h"
#include "math.h"
#include <cmath>
#include <numbers>

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	// TODO: return ステートメントをここに挿入します
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

float easeInOutSine(float x1, float x2, float t) {

	float easeT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, easeT);
}

float Lerp(float x1, float x2, float t) {
	return x1 + t * (x2 - x1); // 修正された補間公式
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { return Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t)); }
