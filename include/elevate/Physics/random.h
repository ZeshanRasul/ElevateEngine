#pragma once
#include "Physics/core.h"

namespace elevate {

	class Random
	{
	public:

		unsigned rotl(unsigned n, unsigned r);
		
		unsigned rotr(unsigned n, unsigned r);

		Random();

		Random(unsigned seed);

		void seed(unsigned seed);

		unsigned randomBits();

		real randomReal();

		real randomReal(real scale);

		real randomReal(real min, real max);

		unsigned randomInt(unsigned max);

		real randomBinomial(real scale);

		Vector3 randomVector(real scale);

		Vector3 randomVector(const Vector3& scale);

		Vector3 randomVector(const Vector3& min, const Vector3& max);

		Vector3 randomXZVector(real scale);

		Quaternion randomQuaternion();

	private:
		int p1, p2;
		unsigned buffer[17];
	};

}