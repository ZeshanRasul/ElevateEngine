#pragma once
#include "GameObjects/cube.h"

class Bone : public elevate::CollisionBox
{
public:
    Bone()
    {
        body = new elevate::RigidBody();
    }

    ~Bone()
    {
        delete body;
    }

    elevate::CollisionSphere& getCollisionSphere() const
    {
        elevate::CollisionSphere sphere;
        sphere.body = body;
        sphere.radius = halfSize.x;
        sphere.offset = elevate::Matrix4();
        if (halfSize.y < sphere.radius) sphere.radius = halfSize.y;
        if (halfSize.z < sphere.radius) sphere.radius = halfSize.z;
        sphere.calculateInternals();
        return sphere;
    }

    void setState(const elevate::Vector3& position,
        const elevate::Vector3& extents)
    {
        body->setPosition(position);
        body->setOrientation(elevate::Quaternion());
        body->setVelocity(elevate::Vector3());
        body->setRotation(elevate::Vector3());
        halfSize = extents;

        elevate::real mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
        body->setMass(mass);

        elevate::Matrix3 tensor;
        tensor.setBlockInertiaTensor(halfSize, mass);
        body->setInertiaTensor(tensor);

        body->setDamping(0.95f, 0.8f);
        body->clearAccumulator();
        body->setAcceleration(elevate::Vector3::GRAVITY);

        body->setCanSleep(false);
        body->setAwake(true);

        body->calculateDerivedData();
        calculateInternals();
    }

    Cube* visual = nullptr;
private:

};