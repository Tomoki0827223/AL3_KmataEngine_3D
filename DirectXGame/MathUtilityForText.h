#include "Vector3.h"

Vector3& operator+=(Vector3& lhv, const Vector3& rhv);

float easeInOutSine(float x1, float x2, float t);

float Lerp(float x1, float x2, float t);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);