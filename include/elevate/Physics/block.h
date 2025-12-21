#pragma once
#include "core.h"
#include "Physics/body.h"
#include "Physics/CollideNarrow.h"
#include "GameObjects/Cube.h"

class Block : public elevate::CollisionBox
{
public:
    bool exists;
    Cube* visual;

    Block()
        :
        exists(false)
    {
        body = new elevate::RigidBody();
    }

    ~Block()
    {
        delete body;
    }

    void setState(const elevate::Vector3& position,
        const elevate::Quaternion& orientation,
        const elevate::Vector3& extents,
        const elevate::Vector3& velocity)
    {
        body->setPosition(position);
        body->setOrientation(orientation);
        body->setVelocity(velocity);
        body->setRotation(elevate::Vector3(0, 0, 0));
        halfSize = extents;

        elevate::real mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
        body->setMass(mass);

        elevate::Matrix3 tensor;
        tensor.setBlockInertiaTensor(halfSize, mass);
        body->setInertiaTensor(tensor);

        body->setLinearDamping(0.95f);
        body->setAngularDamping(0.8f);
        body->clearAccumulator();
        body->setAcceleration(elevate::Vector3(0, -10.0f, 0));

        body->setAwake(true);

        body->calculateDerivedData();
    }
    
    void calculateMassProperties(elevate::real invDensity)
    {
        if (invDensity <= 0)
        {
            body->setInverseMass(0);
            body->setInverseInertiaTensor(elevate::Matrix3());
        }
        else
        {
            elevate::real volume = halfSize.magnitude() * 2.0;
            elevate::real mass = volume / invDensity;

            body->setMass(mass);

            mass *= 0.333f;
            elevate::Matrix3 tensor;
            tensor.setInertiaTensorCoeffs(
                mass * halfSize.y * halfSize.y + halfSize.z * halfSize.z,
                mass * halfSize.y * halfSize.x + halfSize.z * halfSize.z,
                mass * halfSize.y * halfSize.x + halfSize.z * halfSize.y
            );
            body->setInertiaTensor(tensor);
        }

    }

    void divideBlock(const elevate::Contact& contact,
        Block* target, Block* blocks, Cube** cubes)
    {

        elevate::Vector3 normal = contact.contactNormal;
        elevate::RigidBody* body = contact.body[0];
        if (body != target->body)
        {
            normal.invert();
            body = contact.body[1];
        }

        elevate::Vector3 point = body->getPointInLocalSpace(contact.contactPoint);
        normal = body->getDirectionInLocalSpace(normal);

        point = point - normal * (point * normal);

        elevate::Vector3 size = target->halfSize;

        elevate::RigidBody tempBody;
        tempBody.setPosition(body->getPosition());
        tempBody.setOrientation(body->getOrientation());
        tempBody.setVelocity(body->getVelocity());
        tempBody.setRotation(body->getRotation());
        tempBody.setLinearDamping(body->getLinearDamping());
        tempBody.setAngularDamping(body->getAngularDamping());
        tempBody.setInverseInertiaTensor(body->getInverseInertiaTensor());
        tempBody.calculateDerivedData();

        target->exists = false;

        elevate::real invDensity =
            halfSize.magnitude() * 8 * body->getInverseMass();

        for (unsigned i = 0; i < 8; i++)
        {
            elevate::Vector3 min, max;
            if ((i & 1) == 0) {
                min.x = -size.x;
                max.x = point.x;
            }
            else {
                min.x = point.x;
                max.x = size.x;
            }
            if ((i & 2) == 0) {
                min.y = -size.y;
                max.y = point.y;
            }
            else {
                min.y = point.y;
                max.y = size.y;
            }
            if ((i & 4) == 0) {
                min.z = -size.z;
                max.z = point.z;
            }
            else {
                min.z = point.z;
                max.z = size.z;
            }

            elevate::Vector3 halfSize = (max - min) * 0.5f;
            elevate::Vector3 newPos = halfSize + min;

            newPos = tempBody.getPointInWorldSpace(newPos);

            elevate::Vector3 direction = newPos - contact.contactPoint;
            direction.normalize();

            blocks[i].visual = *cubes++;
            blocks[i].visual->SetScale(
               elevate::Vector3(
                    (float)(halfSize.x * 2.0f),
                    (float)(halfSize.y * 2.0f),
                    (float)(halfSize.z * 2.0f)
                )
			);

            blocks[i].body->setPosition(newPos);
            blocks[i].body->setVelocity(tempBody.getVelocity() + direction * 10.0f);
            blocks[i].body->setOrientation(tempBody.getOrientation());
            blocks[i].body->setRotation(tempBody.getRotation());
            blocks[i].body->setLinearDamping(tempBody.getLinearDamping());
            blocks[i].body->setAngularDamping(tempBody.getAngularDamping());
            blocks[i].body->setAwake(true);
            blocks[i].body->setAcceleration(elevate::Vector3::GRAVITY);
            blocks[i].body->clearAccumulator();
            blocks[i].body->calculateDerivedData();
            blocks[i].offset = elevate::Matrix4();
            blocks[i].exists = true;
            blocks[i].halfSize = halfSize;

            blocks[i].calculateMassProperties(invDensity);
        }
    }
};
