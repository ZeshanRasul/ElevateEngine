#include "Physics/Factory.h"
#include "GameObjects/Cube.h"
#include "GameObjects/Sphere.h"

namespace elevate
{
	// ------------------------------------------------------------
	// ShapeFactory
	// ------------------------------------------------------------

	CollisionPrimitive* ShapeFactory::CreateBoxShape(
		World& world,
		const Vector3& halfExtents)
	{
		return world.CreateBoxShape(halfExtents);
	}

	CollisionPrimitive* ShapeFactory::CreateSphereShape(
		World& world,
		real radius)
	{
		return world.CreateSphereShape(radius);
	}

	// ------------------------------------------------------------
	// SpawnFactory
	// ------------------------------------------------------------

	SpawnFactory::SpawnFactory(const SpawnContext& ctx)
		: m_Ctx(ctx)
	{
	}

	RigidBody* SpawnFactory::CreateRigidBody(
		const Transform& transform,
		real mass,
		CollisionPrimitive* shape)
	{
		RigidBody* body = m_Ctx.World->CreateRigidBody(transform, mass, shape);
		shape->body = body;
		return body;
	}

	std::string SpawnFactory::ResolveMaterialId(PhysicsMaterialId id) const
	{
		switch (id)
		{
		case PhysicsMaterialId::WoodCrate:      return "Mat_WoodCrate";
		case PhysicsMaterialId::Brick:          return "Mat_Brick";
		case PhysicsMaterialId::MetalBarrel:    return "Mat_MetalBarrel";
		case PhysicsMaterialId::ConcreteFloor:  return "Mat_ConcreteFloor";
		case PhysicsMaterialId::Wall:           return "Mat_Wall";
		case PhysicsMaterialId::Rubber:         return "Mat_Rubber";
		case PhysicsMaterialId::Bullet:         return "Mat_Bullet";
		case PhysicsMaterialId::Rocket:         return "Mat_Rocket";
		case PhysicsMaterialId::Grenade:        return "Mat_Grenade";
		case PhysicsMaterialId::Glass:          return "Mat_Glass";
		default:                                return "Mat_Default";
		}
	}

	PhysicsObject* SpawnFactory::CreatePhysicsObject(
		const std::string& name,
		const Transform& transform,
		CollisionPrimitive* shape,
		real mass,
		const std::string& meshId,
		const std::string& materialId,
		Shader* shader)
	{
		if (!m_Ctx.World)
			return nullptr;

		RigidBody* body = CreateRigidBody(transform, mass, shape);


		PhysicsObject* obj = new PhysicsObject();
		obj->body = body;
		obj->shape = shape;
		obj->meshId = meshId;
		obj->materialId = materialId;

		if (name == "Box")
		{
			Cube* cube = new Cube();
			obj->mesh = cube;
			obj->mesh->LoadMesh();
			obj->mesh->SetPosition(obj->body->getPosition());
			obj->mesh->SetScale(Vector3(transform.scale));
			obj->mesh->SetShader(shader);
		}


		if (name == "Sphere")
		{
			Sphere* sphere = new Sphere();
			obj->mesh = sphere;
			obj->mesh->LoadMesh();
			obj->mesh->SetPosition(obj->body->getPosition());
			obj->mesh->SetScale(elevate::Vector3(transform.scale)),
			obj->mesh->SetShader(shader);
		}


		return obj;
	}

	PhysicsObject* SpawnFactory::CreatePhysicsObject(
		const std::string& name,
		const Transform& transform,
		CollisionPrimitive* shape,
		real mass,
		const std::string& meshId,
		PhysicsMaterialId materialId,
		Shader* shader)
	{
		return CreatePhysicsObject(
			name,
			transform,
			shape,
			mass,
			meshId,
			ResolveMaterialId(materialId),
			shader);
	}

	// Primitive spawners

	PhysicsObject* SpawnFactory::SpawnBox(
		const Vector3& position,
		const Vector3& halfExtents,
		real mass,
		const std::string& meshId,
		PhysicsMaterialId materialId,
		Shader* shader)
	{
		Transform t{};
		t.position = position;
		t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		t.scale = { 1.0f, 1.0f, 1.0f };

		CollisionPrimitive* shape = ShapeFactory::CreateBoxShape(*m_Ctx.World, halfExtents);

		return CreatePhysicsObject(
			"Box",
			t,
			shape,
			mass,
			meshId,
			materialId,
			shader);
	}

	PhysicsObject* SpawnFactory::SpawnSphere(
		const Vector3& position,
		real radius,
		real mass,
		const std::string& meshId,
		PhysicsMaterialId materialId,
		Shader* shader)
	{
		Transform t{};
		t.position = position;
		t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		t.scale = { 1.0f, 1.0f, 1.0f };

		CollisionPrimitive* shape = ShapeFactory::CreateSphereShape(*m_Ctx.World, radius);

		return CreatePhysicsObject(
			"Sphere",
			t,
			shape,
			mass,
			meshId,
			materialId,
			shader);
	}

	//PhysicsObject* SpawnFactory::SpawnCapsule(
	//    const Vector3& position,
	//    real radius,
	//    real halfHeight,
	//    real mass,
	//    const std::string& meshId,
	//    PhysicsMaterialId materialId)
	//{
	//    Transform t{};
	//    t.position = position;
	//    t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	//    t.scale = { 1.0f, 1.0f, 1.0f };

	//    CollisionPrimitive* shape = ShapeFactory::CreateCapsuleShape(
	//        *m_Ctx.World, radius, halfHeight);

	//    return CreatePhysicsObject(
	//        "Capsule",
	//        t,
	//        shape,
	//        mass,
	//        meshId,
	//        materialId);
	//}

	//PhysicsObject* SpawnFactory::SpawnCylinder(
	//    const Vector3& position,
	//    real radius,
	//    real halfHeight,
	//    real mass,
	//    const std::string& meshId,
	//    PhysicsMaterialId materialId)
	//{
	//    Transform t{};
	//    t.position = position;
	//    t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	//    t.scale = { 1.0f, 1.0f, 1.0f };

	//    CollisionPrimitive* shape = ShapeFactory::CreateCylinderShape(
	//        *m_Ctx.World, radius, halfHeight);

	//    return CreatePhysicsObject(
	//        "Cylinder",
	//        t,
	//        shape,
	//        mass,
	//        meshId,
	//        materialId);
	//}

	// Domain specific spawners

	PhysicsObject* SpawnFactory::SpawnCrate(
		const Vector3& position,
		Shader* shader,
		const Vector3& size,
		real mass)
	{
		Vector3 halfExtents = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
		return SpawnBox(
			position,
			halfExtents,
			mass,
			"Mesh_Crate",
			PhysicsMaterialId::WoodCrate,
			shader);
	}

	//PhysicsObject* SpawnFactory::SpawnBrick(
	//	const Vector3& position,
	//	const Vector3& size,
	//	real mass,
	//	Shader* shader)
	//{
	//	Vector3 halfExtents = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
	//	return SpawnBox(
	//		position,
	//		halfExtents,
	//		mass,
	//		"Mesh_Brick",
	//		PhysicsMaterialId::Brick,
	//		shader);
	//}

	//PhysicsObject* SpawnFactory::SpawnBarrel(
	//    const Vector3& position,
	//    real radius,
	//    real height,
	//    real mass)
	//{
	//    real halfHeight = height * 0.5f;
	//    return SpawnCylinder(
	//        position,
	//        radius,
	//        halfHeight,
	//        mass,
	//        "Mesh_Barrel",
	//        PhysicsMaterialId::MetalBarrel);
	//}

	//PhysicsObject* SpawnFactory::SpawnBullet(
	//    const Vector3& startPos,
	//    const Vector3& direction,
	//    real speed,
	//    real radius,
	//    real mass)
	//{
	//    PhysicsObject* obj = SpawnSphere(
	//        startPos,
	//        radius,
	//        mass,
	//        "Mesh_Bullet",
	//        PhysicsMaterialId::Bullet);

	//    if (obj && obj->body)
	//    {
	//        Vector3 vel =
	//        {
	//            direction.x * speed,
	//            direction.y * speed,
	//            direction.z * speed
	//        };
	//        obj->body->SetLinearVelocity(vel);
	//    }

	//    return obj;
	//}

	//PhysicsObject* SpawnFactory::SpawnRocket(
	//    const Vector3& startPos,
	//    const Vector3& direction,
	//    real speed,
	//    real mass)
	//{
	//    // Capsule or elongated box would work for collision.
	//    real radius = 0.1f;
	//    real halfHeight = 0.5f;

	//    PhysicsObject* obj = SpawnCapsule(
	//        startPos,
	//        radius,
	//        halfHeight,
	//        mass,
	//        "Mesh_Rocket",
	//        PhysicsMaterialId::Rocket);

	//    if (obj && obj->body)
	//    {
	//        Vector3 vel =
	//        {
	//            direction.x * speed,
	//            direction.y * speed,
	//            direction.z * speed
	//        };
	//        obj->body->SetLinearVelocity(vel);
	//    }

	//    return obj;
	//}

	PhysicsObject* SpawnFactory::SpawnGrenade(
		const Vector3& startPos,
		const Vector3& velocity,
		Shader* shader,
		real radius,
		real mass)
	{
		PhysicsObject* obj = SpawnSphere(
			startPos,
			radius,
			mass,
			"Mesh_Grenade",
			PhysicsMaterialId::Grenade,
			shader);

		if (obj && obj->body)
		{
			obj->body->setVelocity(velocity);
		}

		// You can store fuseTime somewhere central.

		return obj;
	}

	// Environment

	//PhysicsObject* SpawnFactory::CreateFloor(
	//	const Vector3& halfExtents,
	//	const Vector3& center)
	//{
	//	// Infinite mass static body.
	//	Transform t{};
	//	t.position = center;
	//	t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	//	t.scale = { 1.0f, 1.0f, 1.0f };

	//	CollisionPrimitive* shape = ShapeFactory::CreateBoxShape(
	//		*m_Ctx.World, halfExtents);

	//	return CreatePhysicsObject(
	//		"Floor",
	//		t,
	//		shape,
	//		0.0f, // static
	//		"Mesh_Floor",
	//		PhysicsMaterialId::ConcreteFloor);
	//}

	//PhysicsObject* SpawnFactory::CreateWall(
	//	const Vector3& halfExtents,
	//	const Vector3& center)
	//{
	//	Transform t{};
	//	t.position = center;
	//	t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	//	t.scale = { 1.0f, 1.0f, 1.0f };

	//	CollisionPrimitive* shape = ShapeFactory::CreateBoxShape(
	//		*m_Ctx.World, halfExtents);

	//	return CreatePhysicsObject(
	//		"Wall",
	//		t,
	//		shape,
	//		0.0f,
	//		"Mesh_Wall",
	//		PhysicsMaterialId::Wall);
	//}

	// Scene builders

	void SpawnFactory::BuildCrateStack(
		const Vector3& basePos,
		int width,
		int height,
		int depth,
		const Vector3& crateSize,
		real massPerCrate,
		Shader* shader,
		std::vector<PhysicsObject*>& crates)
	{
		Vector3 step =
		{
			crateSize.x,
			crateSize.y,
			crateSize.z
		};

		int noCrates = width * height * depth;

		crates.reserve(noCrates);

		int i = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int z = 0; z < depth; ++z)
			{
				for (int x = 0; x < width; ++x)
				{
					Vector3 pos =
					{
						basePos.x + (x - width * 0.5f + 0.5f) * step.x,
						basePos.y + y * step.y,
						basePos.z + (z - depth * 0.5f + 0.5f) * step.z
					};

					crates.push_back(SpawnCrate(pos, shader, crateSize, massPerCrate));
				}
			}
		}
	}

	//void SpawnFactory::BuildBrickWall(
	//	const Vector3& basePos,
	//	int bricksWide,
	//	int bricksHigh,
	//	const Vector3& brickSize,
	//	real massPerBrick,
	//	bool staggered)
	//{
	//	Vector3 step =
	//	{
	//		brickSize.x,
	//		brickSize.y,
	//		brickSize.z
	//	};

	//	for (int y = 0; y < bricksHigh; ++y)
	//	{
	//		bool offsetRow = staggered && (y % 2 == 1);
	//		for (int x = 0; x < bricksWide; ++x)
	//		{
	//			float offset = offsetRow ? 0.5f : 0.0f;

	//			Vector3 pos =
	//			{
	//				basePos.x + ((x - bricksWide * 0.5f + 0.5f + offset) * step.x),
	//				basePos.y + y * step.y,
	//				basePos.z
	//			};

	//			SpawnBrick(pos, brickSize, massPerBrick);
	//		}
	//	}
	//}

	//void SpawnFactory::BuildDominoLine(
	//	const Vector3& startPos,
	//	const Vector3& direction,
	//	int count,
	//	const Vector3& size,
	//	real mass,
	//	real spacing)
	//{
	//	Vector3 dirNorm = direction;
	//	// Assume direction is already normalized for now.

	//	for (int i = 0; i < count; ++i)
	//	{
	//		Vector3 pos =
	//		{
	//			startPos.x + dirNorm.x * spacing * i,
	//			startPos.y + dirNorm.y * spacing * i,
	//			startPos.z + dirNorm.z * spacing * i
	//		};

	//		// Domino is a tall thin box.
	//		Vector3 halfExtents =
	//		{
	//			size.x * 0.5f,
	//			size.y * 0.5f,
	//			size.z * 0.5f
	//		};

	//		SpawnBox(
	//			pos,
	//			halfExtents,
	//			mass,
	//			"Mesh_Domino",
	//			PhysicsMaterialId::Default);
	//	}
	//}

	// Explosion helper

	//void SpawnFactory::Explode(
	//    const Vector3& position,
	//    real radius,
	//    real impulseStrength)
	//{
	//    if (!m_Ctx.World)
	//        return;

	//    // Iterate all bodies in the physics world.
	//    auto& bodies = m_Ctx.World->GetRigidBodies();

	//    for (RigidBody* body : bodies)
	//    {
	//        ApplyRadialImpulse(body, position, radius, impulseStrength);
	//    }
	//}

	//void SpawnFactory::ApplyRadialImpulse(
	//    RigidBody* body,
	//    const Vector3& explosionOrigin,
	//    real radius,
	//    real impulseStrength)
	//{
	//    if (!body)
	//        return;

	//    Vector3 bodyPos = body->GetPosition();

	//    Vector3 toBody =
	//    {
	//        bodyPos.x - explosionOrigin.x,
	//        bodyPos.y - explosionOrigin.y,
	//        bodyPos.z - explosionOrigin.z
	//    };

	//    real distSq = toBody.x * toBody.x +
	//        toBody.y * toBody.y +
	//        toBody.z * toBody.z;

	//    if (distSq > radius * radius)
	//        return;

	//    real dist = std::sqrt(distSq);
	//    if (dist < 0.0001f)
	//        dist = 0.0001f;

	//    real falloff = 1.0f - (dist / radius);

	//    Vector3 dir =
	//    {
	//        toBody.x / dist,
	//        toBody.y / dist,
	//        toBody.z / dist
	//    };

	//    Vector3 impulse =
	//    {
	//        dir.x * impulseStrength * falloff,
	//        dir.y * impulseStrength * falloff,
	//        dir.z * impulseStrength * falloff
	//    };

	//    body->ApplyImpulse(impulse);
	//}

} // namespace elevate
