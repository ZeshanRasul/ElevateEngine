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
		obj->body->setPosition(transform.position);
		obj->body->setOrientation(transform.rotation);
		obj->shape = shape;
		obj->shape->body = body;
		obj->meshId = meshId;
		obj->materialId = materialId;
		obj->body->calculateDerivedData();
		obj->shape->calculateInternals();
		if (name == "Box" || name == "Floor" || name == "Wall")
		{
			Cube* cube = new Cube();
			obj->mesh = cube;
			obj->mesh->LoadMesh();
			obj->mesh->SetPosition(transform.position);
			obj->mesh->SetScale(Vector3(transform.scale));
			obj->mesh->SetShader(shader);
		}


		if (name == "Sphere")
		{
			Sphere* sphere = new Sphere();
			obj->mesh = sphere;
			obj->mesh->LoadMesh();
			obj->mesh->SetPosition(transform.position);
			obj->mesh->SetScale(transform.scale);
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
		t.scale = { halfExtents * 2};

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
		Vector3 halfExtents = { size.x, size.y, size.z};
		return SpawnBox(
			position,
			halfExtents,
			mass,
			"Mesh_Crate",
			PhysicsMaterialId::WoodCrate,
			shader);
	}

	PhysicsObject* SpawnFactory::SpawnBrick(
		const Vector3& position,
		Shader* shader,
		real mass,
		const Vector3& size)
	{
		Vector3 halfExtents = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
		return SpawnBox(
			position,
			halfExtents,
			mass,
			"Mesh_Brick",
			PhysicsMaterialId::Brick,
			shader);
	}

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


		return obj;
	}

	// Environment

	PhysicsObject* SpawnFactory::CreateFloor(
		const Vector3& halfExtents,
		Shader* shader,
		const Vector3& center)
	{
		// Infinite mass static body.
		Transform t{};
		t.position = center;
		t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		t.scale = { halfExtents * 2 };

		CollisionPrimitive* shape = ShapeFactory::CreateBoxShape(
			*m_Ctx.World, halfExtents);

		return CreatePhysicsObject(
			"Floor",
			t,
			shape,
			FLT_MAX, // static
			"Mesh_Floor",
			PhysicsMaterialId::ConcreteFloor,
			shader);
	}

	PhysicsObject* SpawnFactory::CreateWall(
		const Vector3& halfExtents,
		const Vector3& center,
		Shader* shader)
	{
		Transform t{};
		t.position = center;
		t.rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		t.scale = { halfExtents * 2 };

		CollisionPrimitive* shape = ShapeFactory::CreateBoxShape(
			*m_Ctx.World, halfExtents);

		return CreatePhysicsObject(
			"Wall",
			t,
			shape,
			FLT_MAX,
			"Mesh_Wall",
			PhysicsMaterialId::Wall,
			shader);
	}

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

	void SpawnFactory::BuildBrickWall(
		const Vector3& basePos,
		int bricksWide,
		int bricksHigh,
		const Vector3& brickSize,
		real massPerBrick,
		bool staggered,
		std::vector<PhysicsObject*>& bricks,
		Shader* shader)
	{
		Vector3 step =
		{
			brickSize.x,
			brickSize.y,
			brickSize.z
		};

		for (int y = 0; y < bricksHigh; ++y)
		{
			bool offsetRow = staggered && (y % 2 == 1);
			for (int x = 0; x < bricksWide; ++x)
			{
				float offset = offsetRow ? 0.5f : 0.0f;

				Vector3 pos =
				{
					basePos.x + ((x - bricksWide * 0.5f + 0.5f + offset) * step.x),
					basePos.y + y * step.y,
					basePos.z
				};

				bricks.push_back(SpawnBrick(pos, shader, massPerBrick, brickSize));
			}
		}
	}

	void SpawnFactory::BuildDominoLine(
		const Vector3& startPos,	
		const Vector3& direction,
		int count,
		const Vector3& size,
		real mass,
		real spacing,
		std::vector<PhysicsObject*>& dominoes)
	{
		Vector3 dirNorm = direction;
		// Assume direction is already normalized for now.

		for (int i = 0; i < count; ++i)
		{
			Vector3 pos =
			{
				startPos.x + dirNorm.x * spacing * i,
				startPos.y + dirNorm.y * spacing * i,
				startPos.z + dirNorm.z * spacing * i
			};

			// Domino is a tall thin box.
			Vector3 halfExtents =
			{
				size.x * 0.5f,
				size.y * 0.5f,
				size.z * 0.5f
			};

			dominoes.push_back(SpawnBox(
				pos,
				halfExtents,
				mass,
				"Mesh_Domino",
				PhysicsMaterialId::Default, nullptr));
		}
	}

	Ragdoll* SpawnFactory::CreateRagdoll(const Vector3& basePosition)
	{
		Ragdoll* ragdoll = new Ragdoll();

		// Helper to offset the original Millington positions by a base position.
		auto offsetPos = [&basePosition](float x, float y, float z) -> Vector3
			{
				return Vector3(
					basePosition.x + x,
					basePosition.y + y,
					basePosition.z + z
				);
			};

		Bone* bones = ragdoll->bones;
		Joint* joints = ragdoll->joints;

		// Right lower leg
		bones[0].setState(
			offsetPos(0.0f, 0.993f, -0.5f),
			Vector3(0.301f, 1.0f, 0.234f));

		// Right upper leg
		bones[1].setState(
			offsetPos(0.0f, 3.159f, -0.56f),
			Vector3(0.301f, 1.0f, 0.234f));

		// Left lower leg
		bones[2].setState(
			offsetPos(0.0f, 0.993f, 0.5f),
			Vector3(0.301f, 1.0f, 0.234f));

		// Left upper leg
		bones[3].setState(
			offsetPos(0.0f, 3.15f, 0.56f),
			Vector3(0.301f, 1.0f, 0.234f));

		// Waist / pelvis
		bones[4].setState(
			offsetPos(-0.054f, 4.683f, 0.013f),
			Vector3(0.415f, 0.392f, 0.690f));

		// Stomach / lower torso
		bones[5].setState(
			offsetPos(0.043f, 5.603f, 0.013f),
			Vector3(0.301f, 0.367f, 0.693f));

		// Chest / upper torso
		bones[6].setState(
			offsetPos(0.0f, 6.485f, 0.013f),
			Vector3(0.435f, 0.367f, 0.786f));

		// Head / neck
		bones[7].setState(
			offsetPos(0.0f, 7.759f, 0.013f),
			Vector3(0.45f, 0.598f, 0.421f));

		// Pin head so the ragdoll hangs
		bones[7].body->setMass(FLT_MAX);

		// Right upper arm
		bones[8].setState(
			offsetPos(0.0f, 5.946f, -1.066f),
			Vector3(0.267f, 0.888f, 0.207f));

		// Right lower arm
		bones[9].setState(
			offsetPos(0.0f, 4.024f, -1.066f),
			Vector3(0.267f, 0.888f, 0.207f));

		// Left upper arm
		bones[10].setState(
			offsetPos(0.0f, 5.946f, 1.066f),
			Vector3(0.267f, 0.888f, 0.207f));

		// Left lower arm
		bones[11].setState(
			offsetPos(0.0f, 4.024f, 1.066f),
			Vector3(0.267f, 0.888f, 0.207f));

		// Right Knee
		joints[0].set(
			bones[0].body, Vector3(0.0f, 1.07f, 0.0f),
			bones[1].body, Vector3(0.0f, -1.07f, 0.0f),
			0.15f
		);

		// Left Knee
		joints[1].set(
			bones[2].body, Vector3(0.0f, 1.07f, 0.0f),
			bones[3].body, Vector3(0.0f, -1.07f, 0.0f),
			0.15f
		);

		// Right elbow
		joints[2].set(
			bones[9].body, Vector3(0.0f, 0.96f, 0.0f),
			bones[8].body, Vector3(0.0f, -0.96f, 0.0f),
			0.15f
		);

		// Left elbow
		joints[3].set(
			bones[11].body, Vector3(0.0f, 0.96f, 0.0f),
			bones[10].body, Vector3(0.0f, -0.96f, 0.0f),
			0.15f
		);

		// Stomach to Waist
		joints[4].set(
			bones[4].body, Vector3(0.054f, 0.50f, 0.0f),
			bones[5].body, Vector3(-0.043f, -0.45f, 0.0f),
			0.15f
		);

		// Waist to Chest
		joints[5].set(
			bones[5].body, Vector3(-0.043f, 0.411f, 0.0f),
			bones[6].body, Vector3(0.0f, -0.411f, 0.0f),
			0.15f
		);

		// Chest to Head
		joints[6].set(
			bones[6].body, Vector3(0.0f, 0.521f, 0.0f),
			bones[7].body, Vector3(0.0f, -0.752f, 0.0f),
			0.15f
		);

		// Right hip
		joints[7].set(
			bones[1].body, Vector3(0.0f, 1.066f, 0.0f),
			bones[4].body, Vector3(0.0f, -0.458f, -0.5f),
			0.15f
		);

		// Left Hip
		joints[8].set(
			bones[3].body, Vector3(0.0f, 1.066f, 0.0f),
			bones[4].body, Vector3(0.0f, -0.458f, 0.5f),
			0.105f
		);

		// Right shoulder
		joints[9].set(
			bones[6].body, Vector3(0.0f, 0.367f, -0.8f),
			bones[8].body, Vector3(0.0f, 0.888f, 0.32f),
			0.15f
		);

		// Left shoulder
		joints[10].set(
			bones[6].body, Vector3(0.0f, 0.367f, 0.8f),
			bones[10].body, Vector3(0.0f, 0.888f, -0.32f),
			0.15f
		);

		return ragdoll;
	}

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
