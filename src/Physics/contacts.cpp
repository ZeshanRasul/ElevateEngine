#include "Physics/contacts.h"

void elevate::Contact::calculateContactBasis()
{
	Vector3 contactTangent[2];
	if (real_abs(contactNormal.x) > real_abs(contactNormal.y))
	{
		const real s = (real)1.0 / real_sqrt(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);
		contactTangent[0].x = contactNormal.z * s;
		contactTangent[0].y = 0;
		contactTangent[0].z = -contactNormal.x * s;
		contactTangent[1].x = contactNormal.y * contactTangent[0].x;
		contactTangent[1].y = contactNormal.z * contactTangent[0].x - contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
	}
	else
	{
		const real s = (real)1.0 / real_sqrt(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);
		contactTangent[0].x = 0;
		contactTangent[0].y = -contactNormal.z * s;
		contactTangent[0].z = contactNormal.y * s;
		contactTangent[1].x = contactNormal.y * contactTangent[0].z - contactNormal.z * contactTangent[0].y;
		contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
		contactTangent[1].z = contactNormal.x * contactTangent[0].y;
	}

	contactToWorld.setComponents(
		contactNormal,
		contactTangent[0],
		contactTangent[1]
	);
}

inline elevate::Vector3 elevate::Contact::calculateFrictionlessImpulse(elevate::Matrix3* inverseInertiaTensor)
{
	Vector3 impulseContact;

	Vector3 deltaVelWorld = relativeContactPosition[0] % (contactNormal);
	deltaVelWorld = inverseInertiaTensor[0].transform(deltaVelWorld);
	deltaVelWorld = deltaVelWorld % (relativeContactPosition[0]);

	real deltaVelocity = deltaVelWorld * contactNormal;

	deltaVelocity += body[0]->getInverseMass();

	if (body[1])
	{
		Vector3 deltaVelWorld2 = relativeContactPosition[1] % (contactNormal);
		deltaVelWorld = inverseInertiaTensor[1].transform(deltaVelWorld2);
		deltaVelWorld2 = deltaVelWorld % (relativeContactPosition[1]);

		deltaVelocity += deltaVelWorld2 * contactNormal;
		deltaVelocity += body[1]->getInverseMass();
	}

	impulseContact.x = desiredDeltaVelocity / deltaVelocity;
	impulseContact.y = 0;
	impulseContact.z = 0;
	return impulseContact;


}

void elevate::Contact::applyPositionChange(Vector3 linearChange[2], Vector3 angularChange[2], real penetration)
{
	const real angularLimit = (real)0.2;
	real angularMove[2];
	real linearMove[2];

	real totalInertia = 0;
	real linearInertia[2];
	real angularInertia[2];
	for (unsigned i = 0; i < 2; i++)
	{
		if (body[i])
		{
			Matrix3 inverseInertiaTensor;
			body[i]->getInverseInertiaTensorWorld(&inverseInertiaTensor);
			Vector3 angularInertiaWorld = relativeContactPosition[i] % contactNormal;
			angularInertiaWorld = inverseInertiaTensor.transform(angularInertiaWorld);
			angularInertiaWorld = angularInertiaWorld % relativeContactPosition[i];
			angularInertia[i] = angularInertiaWorld * contactNormal;
			linearInertia[i] = body[i]->getInverseMass();
			totalInertia += linearInertia[i] + angularInertia[i];
		}
	}
	for (unsigned i = 0; i < 2; i++)
	{
		if (body[i])
		{
			real sign = (i == 0) ? (real)1 : (real)-1;
			angularChange[i] = Vector3(0, 0, 0);
			linearChange[i] = contactNormal * (sign * penetration * (linearInertia[i] / totalInertia));

			Vector3 projection = relativeContactPosition[i];
			projection.addScaledVector(contactNormal, -relativeContactPosition[i].scalarProduct(contactNormal));

			real maxMagnitude = angularLimit * projection.magnitude();

			if (angularMove[i] < -maxMagnitude)
			{
				real angularMove = sign * penetration * (angularInertia[i] / totalInertia);
				real totalMove = angularMove + linearMove[i];
				angularMove = -maxMagnitude;
				linearMove[i] = totalMove - angularMove;
			}
			else if (angularMove[i] > maxMagnitude)
			{
				real angularMove = sign * penetration * (angularInertia[i] / totalInertia);
				real totalMove = angularMove + linearMove[i];
				angularMove = maxMagnitude;
				linearMove[i] = totalMove - angularMove;
			}
			if (angularMove[i] == (real)0)
			{
				angularChange[i].clear();
			}
			else
			{
				Vector3 targetAngularDirection = relativeContactPosition[i].vectorProduct(contactNormal);
				Matrix3 inverseInertiaTensor;
				body[i]->getInverseInertiaTensorWorld(&inverseInertiaTensor);

				angularChange[i] = inverseInertiaTensor.transform(targetAngularDirection) * (angularMove[i] / angularInertia[i]);
			}

			linearChange[i] = contactNormal * linearMove[i];

			Vector3 pos = body[i]->getPosition();
			pos.addScaledVector(contactNormal, linearMove[i]);
			body[i]->setPosition(pos);

			Quaternion q;
			q = body[i]->getOrientation();
			q.addScaledVector(angularChange[i], ((real)1.0));
			body[i]->setOrientation(q);

			// We need to calculate the derived data for any body that is
			// asleep, so that the changes are reflected in the object's
			// data. Otherwise the resolution will not change the position
			// of the object, and the next collision detection round will
			// have the same penetration.
			if (!body[i]->getAwake()) body[i]->calculateDerivedData();

		}

	}
}