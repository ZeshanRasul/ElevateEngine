#pragma once
#include "Physics/fgen.h"

class Aero : public RigidBodyForceGenerator
{
protected:
 

public:
   
    Aero(const Matrix3& tensor, const Vector3& position,
        const Vector3* windspeed);

    Aero() {};

    virtual void updateForce(RigidBody* body, real duration);

    Matrix3 tensor;

    Vector3 position;

    const Vector3* windspeed;

protected:
  
    void updateForceFromTensor(RigidBody* body, real duration,
        const Matrix3& tensor);
};

class AeroControl : public Aero
{
protected:
    

private:
   
    Matrix3 getTensor();

public:
    AeroControl() {};

    AeroControl(const Matrix3& base,
        const Matrix3& min, const Matrix3& max,
        const Vector3& position, const Vector3* windspeed);

    void setControl(real value);

    virtual void updateForce(RigidBody* body, real duration);
    Matrix3 maxTensor;

   
    Matrix3 minTensor;

  
    real controlSetting;
};

class AngledAero : public Aero
{
    Quaternion orientation;

public:
    
    AngledAero(const Matrix3& tensor, const Vector3& position,
        const Vector3* windspeed);

    void setOrientation(const Quaternion& quat);

    virtual void updateForce(RigidBody* body, real duration);
};