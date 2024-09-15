#pragma once

#include "precision.h"

namespace elevate {

	constexpr float PI = 3.14159265359f;

	class Vector3 {
	public:
		real x;
		real y;
		real z;

	private:
		real pad;

	public:
		Vector3() : x(0), y(0), z(0) {};

		Vector3(real x, real y, real z) : x(x), y(y), z(z) {};

		const static Vector3 GRAVITY;
		const static Vector3 HIGH_GRAVITY;

		void operator+= (const Vector3& v) {
			x += v.x;
			y += v.y;
			z += v.z;
		}

		Vector3 operator+ (const Vector3& v) const {
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		void operator-= (const Vector3& v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		Vector3 operator- (const Vector3& v) const {
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		void operator*= (const real value) {
			x *= value;
			y *= value;
			z *= value;
		}

		Vector3 operator* (const real value) const {
			return Vector3(x * value, y * value, z * value);
		}

		Vector3 componentProduct (const Vector3& v) const {
			return Vector3(x * v.x, y * v.y, z * v.z);
		}

		void componentProductUpdate (const Vector3& v) {
			x *= v.x;
			y *= v.y;
			z *= v.z;
		}

		real scalarProduct(const Vector3& v) const {
			return x * v.x + y * v.y + z * v.z;
		}

		real operator* (const Vector3& v) const {
			return x * v.x + y * v.y + z * v.z;
		}

		Vector3 vectorProduct (const Vector3& v) const {
			return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}

		void operator%= (const Vector3& v) {
			*this = vectorProduct(v);
		}

		Vector3 operator% (const Vector3& v) const {
			return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}

		void invert() {
			x = -x;
			y = -y;
			z = -z;
		}

		real magnitude() const {
			return real_sqrt(x * x + y * y + z * z);
		}

		real squareMagnitude() const {
			return x * x + y * y + z * z;
		}

		void normalize() {
			real l = magnitude();
			if (l > 0) {
				(*this) *= ((real)1) / l;
			}
		}

		void addScaledVector(const Vector3& v, real scale) {
			x += v.x * scale;
			y += v.y * scale;
			z += v.z * scale;
		}

		void clear() {
			x = y = z = 0;
		}
	};
};