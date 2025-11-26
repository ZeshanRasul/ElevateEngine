#pragma once
#include "Physics/contacts.h"

namespace elevate {

    class Joint : public ContactGenerator
    {
    public:
        RigidBody* body[2];

        Vector3 position[2];
   
        real error;

        void set(
            RigidBody* a, const Vector3& a_pos,
            RigidBody* b, const Vector3& b_pos,
            real error
        );

        unsigned addContact(Contact* contact, unsigned limit) const;
    };
}