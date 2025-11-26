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

    /** Sets the block to a specific location. */
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

        //body->setCanSleep(false);
        body->setAwake(true);

        body->calculateDerivedData();
    }

    /**
     * Calculates and sets the mass and inertia tensor of this block,
     * assuming it has the given constant density.
     */
    void calculateMassProperties(elevate::real invDensity)
    {
        // Check for infinite mass
        if (invDensity <= 0)
        {
            // Just set zeros for both mass and inertia tensor
            body->setInverseMass(0);
            body->setInverseInertiaTensor(elevate::Matrix3());
        }
        else
        {
            // Otherwise we need to calculate the mass
            elevate::real volume = halfSize.magnitude() * 2.0;
            elevate::real mass = volume / invDensity;

            body->setMass(mass);

            // And calculate the inertia tensor from the mass and size
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

    /**
     * Performs the division of the given block into four, writing the
     * eight new blocks into the given blocks array. The blocks array can be
     * a pointer to the same location as the target pointer: since the
     * original block is always deleted, this effectively reuses its storage.
     * The algorithm is structured to allow this reuse.
     */
    void divideBlock(const elevate::Contact& contact,
        Block* target, Block* blocks, Cube** cubes)
    {

        // Find out if we're block one or two in the contact structure, and
        // therefore what the contact normal is.
        elevate::Vector3 normal = contact.contactNormal;
        elevate::RigidBody* body = contact.body[0];
        if (body != target->body)
        {
            normal.invert();
            body = contact.body[1];
        }

        // Work out where on the body (in body coordinates) the contact is
        // and its direction.
        elevate::Vector3 point = body->getPointInLocalSpace(contact.contactPoint);
        normal = body->getDirectionInLocalSpace(normal);

        // Work out the centre of the split: this is the point coordinates
        // for each of the axes perpendicular to the normal, and 0 for the
        // axis along the normal.
        point = point - normal * (point * normal);

        // Take a copy of the half size, so we can create the new blocks.
        elevate::Vector3 size = target->halfSize;

        // Take a copy also of the body's other data.
        elevate::RigidBody tempBody;
        tempBody.setPosition(body->getPosition());
        tempBody.setOrientation(body->getOrientation());
        tempBody.setVelocity(body->getVelocity());
        tempBody.setRotation(body->getRotation());
        tempBody.setLinearDamping(body->getLinearDamping());
        tempBody.setAngularDamping(body->getAngularDamping());
        tempBody.setInverseInertiaTensor(body->getInverseInertiaTensor());
        tempBody.calculateDerivedData();

        // Remove the old block
        target->exists = false;

        // Work out the inverse density of the old block
        elevate::real invDensity =
            halfSize.magnitude() * 8 * body->getInverseMass();

        // Now split the block into eight.
        for (unsigned i = 0; i < 8; i++)
        {
            // Find the minimum and maximum extents of the new block
            // in old-block coordinates
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

            // Get the origin and half size of the block, in old-body
            // local coordinates.
            elevate::Vector3 halfSize = (max - min) * 0.5f;
            elevate::Vector3 newPos = halfSize + min;

            // Convert the origin to world coordinates.
            newPos = tempBody.getPointInWorldSpace(newPos);

            // Work out the direction to the impact.
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

            // Set the body's properties (we assume the block has a body
            // already that we're going to overwrite).
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

            // Finally calculate the mass and inertia tensor of the new block
            blocks[i].calculateMassProperties(invDensity);
        }
    }
};
