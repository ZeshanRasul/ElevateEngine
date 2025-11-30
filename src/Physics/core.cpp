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

void elevate::Quaternion::fillMatrix(Matrix3& m)
{
	real ii = i * i;
	real jj = j * j;
	real kk = k * k;

	real ij = i * j;
	real ik = i * k;
	real jk = j * k;

	real ri = r * i;
	real rj = r * j;
	real rk = r * k;

	m.data[0] = 1 - 2 * (jj + kk);
	m.data[1] = 2 * (ij + rk);
	m.data[2] = 2 * (ik - rj);

	m.data[3] = 2 * (ij - rk);
	m.data[4] = 1 - 2 * (ii + kk);
	m.data[5] = 2 * (jk + ri);

	m.data[6] = 2 * (ik + rj);
	m.data[7] = 2 * (jk - ri);
	m.data[8] = 1 - 2 * (ii + jj);

}
