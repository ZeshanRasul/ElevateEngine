#pragma once

#pragma once

#include <string>
#include <vector>

#include "Physics/core.h"
#include "Physics/CollideNarrow.h"
#include "Physics/body.h"
#include "Physics/contacts.h"
#include "Physics/World.h"
#include "Physics/bone.h"
#include "Physics/joints.h"
#include "GameObjects/GameObject.h"


namespace elevate
{
	struct PhysicsObject
	{
		RigidBody* body = nullptr;
		CollisionPrimitive* shape = nullptr;
		std::string    meshId;
		std::string    materialId;
		std::string		name;
		GameObject* mesh = nullptr;
	};

	enum class PhysicsMaterialId
	{
		Default,
		WoodCrate,
		Brick,
		MetalBarrel,
		ConcreteFloor,
		Wall,
		Rubber,
		Bullet,
		Rocket,
		Grenade,
		Glass
	};

	struct Ragdoll
	{
		static const int NumBones = 12;
		static const int NumJoints = 11;

		Bone  bones[NumBones];
		Joint joints[NumJoints];
	};

	class ShapeFactory
	{
	public:
		static CollisionPrimitive* CreateBoxShape(
			World& world,
			const Vector3& halfExtents);

		static CollisionPrimitive* CreateSphereShape(
			World& world,
			real radius);

		static CollisionPrimitive* CreateCapsuleShape(
			World& world,
			real radius,
			real halfHeight);

		static CollisionPrimitive* CreateCylinderShape(
			World& world,
			real radius,
			real halfHeight);

		static CollisionPrimitive* CreateConvexHullShape(
			World& world,
			const std::vector<Vector3>& points);
	};

	struct SpawnContext
	{
		World* World = nullptr;
	};

	class SpawnFactory
	{
	public:
		explicit SpawnFactory(const SpawnContext& ctx);

		PhysicsObject* CreatePhysicsObject(
			const std::string& name,
			const Transform& transform,
			CollisionPrimitive* shape,
			real mass,
			const std::string& meshId,
			const std::string& materialId,
			Shader* shader);

		PhysicsObject* CreatePhysicsObject(
			const std::string& name,
			const Transform& transform,
			CollisionPrimitive* shape,
			real mass,
			const std::string& meshId,
			PhysicsMaterialId materialId,
			Shader* shader);

		PhysicsObject* SpawnBox(
			const Vector3& position,
			const Vector3& halfExtents,
			real mass,
			const std::string& meshId,
			PhysicsMaterialId materialId,
			Shader* shader);

		PhysicsObject* SpawnSphere(
			const Vector3& position,
			real radius,
			real mass,
			const std::string& meshId,
			PhysicsMaterialId materialId,
			Shader* shader);

		PhysicsObject* SpawnCapsule(
			const Vector3& position,
			real radius,
			real halfHeight,
			real mass,
			const std::string& meshId,
			PhysicsMaterialId materialId);

		PhysicsObject* SpawnCylinder(
			const Vector3& position,
			real radius,
			real halfHeight,
			real mass,
			const std::string& meshId,
			PhysicsMaterialId materialId);

		PhysicsObject* SpawnCrate(
			const Vector3& position,
			Shader* shader,
			const Vector3& size = { 1.0f, 1.0f, 1.0f },
			real mass = 10.0f);

		PhysicsObject* SpawnBrick(
			const Vector3& position,
			Shader* shader,
			real mass = 2.0f,
			const Vector3& size = { 0.25f, 0.06f, 0.12f }
		);

		PhysicsObject* SpawnBarrel(
			const Vector3& position,
			real radius = 0.4f,
			real height = 1.0f,
			real mass = 15.0f);

		PhysicsObject* SpawnBullet(
			const Vector3& startPos,
			const Vector3& direction,
			real speed,
			real radius = 0.05f,
			real mass = 0.1f);

		PhysicsObject* SpawnRocket(
			const Vector3& startPos,
			const Vector3& direction,
			real speed,
			real mass = 1.0f);

		PhysicsObject* SpawnGrenade(
			const Vector3& startPos,
			const Vector3& velocity,
			Shader* shader,
			real radius = 0.1f,
			real mass = 0.5f);

		PhysicsObject* CreateFloor(
			const Vector3& halfExtents,
			Shader* shader,
			const Vector3& center = { 0.0f, 0.0f, 0.0f });

		PhysicsObject* CreateWall(
			const Vector3& halfExtents,
			const Vector3& center,
			Shader* shader);

		void BuildCrateStack(
			const Vector3& basePos,
			int width,
			int height,
			int depth,
			const Vector3& crateSize,
			real massPerCrate,
			Shader* shader,
			std::vector<PhysicsObject*>& crates);

		void BuildBrickWall(
			const Vector3& basePos,
			int bricksWide,
			int bricksHigh,
			const Vector3& brickSize,
			real massPerBrick,
			bool staggered,
			std::vector<PhysicsObject*>& bricks,
			Shader* shader);

		void BuildDominoLine(
			const Vector3& startPos,
			const Vector3& direction,
			int count,
			const Vector3& size,
			real mass,
			real spacing,
			std::vector<PhysicsObject*>& dominoes);

		Ragdoll* CreateRagdoll(
			const Vector3& basePosition);

		void Explode(
			const Vector3& position,
			real radius,
			real impulseStrength);

	private:
		SpawnContext m_Ctx;

		RigidBody* CreateRigidBody(
			const Transform& transform,
			real mass,
			CollisionPrimitive* shape);

		std::string ResolveMaterialId(PhysicsMaterialId id) const;

		void ApplyRadialImpulse(
			RigidBody* body,
			const Vector3& explosionOrigin,
			real radius,
			real impulseStrength);
	};

}
