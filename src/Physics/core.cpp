#include "elevate/Physics/core.h"

using namespace elevate;

const Vector3 Vector3::GRAVITY = Vector3(0.0f, -9.81f, 0.0f);
const Vector3 Vector3::HIGH_GRAVITY = Vector3(0.0f, -19.62f, 0.0f);

Matrix3 Matrix3::linearInterpolate(const Matrix3& a, const Matrix3& b, real prop)
{
    Matrix3 result;
    for (unsigned i = 0; i < 9; i++) {
        result.data[i] = a.data[i] * (1 - prop) + b.data[i] * prop;
    }
    return result;
}
