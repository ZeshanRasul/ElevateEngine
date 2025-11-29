#pragma once
#include <string>

#include "OpenGL/Renderer.h"
#include "OpenGL/RenderData.h"
#include "Window/Window.h"

#include "App/InputManager.h"

#include "App/Camera.h"
#include "GameObjects/Cube.h"
#include "GameObjects/Sphere.h"
#include "GameObjects/Line.h"
#include "GameObjects/Plane.h"
#include "Physics/Factory.h"
#include "Physics/Demos/FloatingSphere.h"
#include "Physics/pfgen.h"
#include "Physics/pworld.h"
#include "Physics/body.h"
#include "Physics/World.h"
#include "Physics/bone.h"
#include "Physics/joints.h"
#include "Physics/random.h"
#include "Physics/block.h"
#include "Physics/Aero.h"

enum class SceneType
{
    None,
    Empty,
    Car,
    Aeroplane,
    Ragdoll,
    FractureWall,
    DominoChain,
    DemoShowcase,
    Count
};

static const char* SceneTypeToString(SceneType type)
{
    switch (type)
    {
    case SceneType::Empty:        return "Empty";
    case SceneType::Car:          return "Car";
    case SceneType::Aeroplane:    return "Aeroplane";
    case SceneType::Ragdoll:      return "Ragdoll";
    case SceneType::FractureWall: return "Fracture Wall";
    case SceneType::DominoChain:  return "Domino Chain";
    case SceneType::DemoShowcase: return "Demo Showcase";
    default:                      return "None";
    }
}

struct DebugLine
{
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 color;
};

enum class AmmoType
{
    Pistol,
    Rifle,
    Rocket
};

struct AmmoRound
{
    Sphere* visual = nullptr;
    elevate::RigidBody* body = nullptr;
    elevate::CollisionSphere* coll = nullptr;
    bool                     active = false;
    float                    lifetime = 0.0f;
};

enum class ShapeType
{
    Box,
    Sphere,
    HalfSpace
};

enum class BodyType
{
    Unknown,
    StackCube,
    EnvBox,
    Block,
    Ammo,
    Bone,
    GroundPlane
};

struct CollisionBody
{
    ShapeType shapeType;
    BodyType  bodyType;
    void* owner;

    elevate::CollisionBox* box = nullptr;
    elevate::CollisionSphere* sphere = nullptr;
    elevate::CollisionPlane* plane = nullptr;
};

struct AircraftParts {
    elevate::Vector3 offset;
    Cube* mesh;
    CollisionBox* coll;
};

struct CarVisuals {
	elevate::Vector3 offset;
	GameObject* mesh;
};

struct Car {
    elevate::RigidBody* body;
	elevate::CollisionBox* chassis;
    GameObject* chassisMesh;

    struct Wheel {
        elevate::CollisionSphere* coll;
		elevate::Vector3 offset;

		GameObject* mesh;

        bool isFront;
        bool isDriven;
    };

	Wheel wheels[4];

    float throttle;
    float brake;
    float steerAngle;
};

class GameManager {
public:
    GameManager(Window* window, unsigned int width, unsigned int height);

    ~GameManager() {
        delete camera;
        for (auto it = gameObjects.begin(); it != gameObjects.end(); ) {
            if (*it) {
                delete* it;
            }
            it = gameObjects.erase(it);
        }
        delete inputManager;
    }

    void setupCamera(unsigned int width, unsigned int height);
    void setSceneData();

    glm::mat4 getView() const { return view; }
    glm::mat4 getProjection() const { return projection; }


    void update(float deltaTime);

    void render();

    void setUpDebugUI();
    void showDebugUI();
    void renderDebugUI();

    void setPushDir(float newDir) { pushDirX = newDir; }
    void fireRound(AmmoType type);
    void DoAllPairCollisions(std::vector<CollisionBody>& bodies, elevate::CollisionData& cData);

    void reset();
    void ResetState();
    void togglePause() { isPaused = !isPaused; }
    void ResetCar();

    void OnQPressed();
    void OnEPressed();
    void OnWPressed();
    void OnSPressed();
    void OnDPressed();
    void OnAPressed();
    void OnZPressed();
    void OnHPressed();
    void OnLeftClick();

    void AddToGameObjects(GameObject* obj) {
        gameObjects.push_back(obj);
    }

    std::vector<GameObject*>& GetGameObjects() {
        return gameObjects;
    }

    PhysicsObject* wall;
    PhysicsObject* wall2;
    PhysicsObject* wall3;
    PhysicsObject* wall4;
    PhysicsObject* floor;

    SpawnContext spawnContext;
    ShapeFactory* shapeFactory;
    SpawnFactory* spawnFactory;

    Shader ammoShader;
    Shader cubeShader;
    Shader lineShader;

    static const int MaxAmmoRounds = 32;

    AmmoRound ammoPool[MaxAmmoRounds];
    int       ammoCount = 0;

    RigidBody* carBody;
    CarPropulsion* carEngine;
    std::vector<CarVisuals> carParts;
    bool showCar = false;

    Car* car;

    bool showPlane = false;
    void ResetPlane();

    float car_throttle = 0.0f;
    bool fpsSandboxDemo = false;

    SceneType currentScene = SceneType::DemoShowcase;
    int selectedIndex = 7;


    bool hit;
    bool firstHit = true;
    bool ball_active;
    unsigned fracture_contact;
    Block blocks[9];
    Cube* cubes[9];
    Texture blockTexture;
    Texture brickTexture;
    Texture crateTexture;

    PhysicsObject* crate;
    PhysicsObject* ball;
    std::vector<PhysicsObject*> crates;
    std::vector<PhysicsObject*> bricks;
    std::vector<PhysicsObject*> dominoes;

    std::vector<Ragdoll*> ragdolls;

    static const int MaxEnvBoxes = 8;
    int               numEnvBoxes = 8;
    elevate::CollisionBox* envBoxes[MaxEnvBoxes];
    elevate::RigidBody* envBodies[MaxEnvBoxes];
    Cube* envCubes[MaxEnvBoxes];
    static const int MaxStackCubes = 5;

    elevate::CollisionBox* cStackBoxes[MaxStackCubes];
    elevate::RigidBody* stackBodies[MaxStackCubes];
    Cube* stackCubes[MaxStackCubes];
    int                    numStackCubes = 0;

    std::vector<std::vector<PhysicsObject*>> crateStacks;

private:
    void ShowCameraControlWindow(Camera& cam);
    void ShowLightControlWindow(DirLight& light);
    void ShowAmmoWindow();
    void ShowBuoyancyWindow();
    void ShowSpawnObjectWindow();
    void ShowEngineWindow();
    void ShowPerformanceWindow();
    void DrawPhysicsObjectsCombo();
    void CalculatePerformanceMetrics(float deltaTime);
    void RemoveDestroyedGameObjects();

    std::vector<SceneType> sceneTypes = {
        SceneType::Empty,
        SceneType::Car,
        SceneType::Aeroplane,
        SceneType::Ragdoll,
        SceneType::FractureWall,
        SceneType::DominoChain,
        SceneType::DemoShowcase
    };

    Renderer* renderer;
    Window* window;
    Camera* camera;

    InputManager* inputManager;
    std::vector<GameObject*> gameObjects;



    Cube* cube;
    Cube* cube2;
    Cube* cube3;
    Cube* cube4;
    Cube* waterCubeTop;
    Cube* waterCubeBottom;
    Sphere* pistolSphere;
    Sphere* artillerySphere;
    Sphere* fireballSphere;
    Sphere* laserSphere;
    Sphere* waterSphere;
    Sphere* springSphere;
    Sphere* springSphere2;
    Sphere* springSphere3;
    Plane* plane;

    elevate::ParticleWorld* pWorld;
    elevate::ParticleCable cable1;
    elevate::ParticleCable cable2;
    elevate::ParticleCable cable3;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    const static unsigned ammoRounds = 16;

    FloatingSphere* Sphere0;
    FloatingSphere* Sphere1;
    FloatingSphere* Sphere2;
    std::vector<FloatingSphere*> spheres;
    elevate::ParticleForceRegistry registry;
    elevate::ParticleAnchoredSpring* springFG;
    elevate::ParticleSpring* bungeeFG;
    elevate::ParticleSpring* bungeeFG1;
    elevate::ParticleGravity* gravityFG;
    elevate::ParticleGravity* pushForce;
    bool isHidden = false;
    float pushDirX = 0;

    Line* lineab;
    Line* linebc;
    Line* linecd;
    glm::vec3 anchorPos = glm::vec3(0.0f);
    std::vector<Line*> lines;

    float floatingSphereMass = 2.0f;
    float waterHeight = 10.0f;
    float maxDepth = 5.0f;
    float floatingSphereVolume = 0.1f;
    float waterDensity = 1000.0f;
    bool showBuoyanceDemo = false;

    bool isPaused = false;
    bool singleStep = false;
    bool enableGravity = true;

    bool cubeDemo = false;
    elevate::RigidBody* testBody = nullptr;
    elevate::RigidBody* testBody2 = nullptr;
    World* rbWorld;
    ForceRegistry rbRegistry;
    Gravity* rbGravity;
    float gravity[3] = { 0.0f, -9.81f, 0.0f };

    bool sphereDemo = false;
    Sphere* sphere = nullptr;
    Sphere* sphere2 = nullptr;
    elevate::RigidBody* sphereBody = nullptr;
    elevate::RigidBody* sphereBody2 = nullptr;

    elevate::RigidBody* planeBody = nullptr;

    elevate::Contact* contacts;
    elevate::CollisionData cData;
    elevate::ContactResolver resolver;
    elevate::CollisionSphere* cSpheres[2];
    elevate::CollisionSphere* cSphere0;
    elevate::CollisionSphere* cSphere1;
    elevate::CollisionBox* cBox0;
    elevate::CollisionBox* cBox1;
    elevate::CollisionPlane* cPlane;

    void generateContacts();

	bool showUI = true;


    Random random;

    std::vector<PhysicsObject*> runTimeBoxes;
    float boxMass = 1.0f;
    float boxSize[3] = { 1.0f, 1.0f, 1.0f };
    float boxColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    std::vector<PhysicsObject*> runTimeSpheres;
    float sphereRadius = 1.0f;
    float sphereMass = 1.0f;
    float sphereColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    int stackWidth = 3;
    int stackHeight = 3;
    int stackDepth = 3;
    float stackBoxSize[3] = { 1.0f, 1.0f, 1.0f };
    float stackCrateMass = 2.0f;
    int stackIndex = 0;

    int wallWidth = 5;
    int wallHeight = 5;
    int wallIndex = 0;
    float brickSizeInput[3] = {1.0f, 1.0f, 1.0f};
	bool isStaggered = true;
	float brickMass = 2.0f;
	std::vector<std::vector<PhysicsObject*>> runtimeWalls;

    float lineDirectionInput[3] = {1.0f, 1.0f, 1.0f};
    float dominoSizeInput[3] = {1.0f, 1.0f, 1.0f};
	int dominoCount = 10;
	float dominoMass = 2.0f;
	float dominoSpacing = 2.0f;
	int dominoLineIndex = 0;
	std::vector<std::vector<PhysicsObject*>> runtimeDominoLines;

    float blockPos[3] = { -50.0f, 25.0f, 50.0f };
    float blockScale[3] = { 15.0f, 15.0f, 15.0f };
    float blockMass = 85.0f;
	std::vector<Block> runtimeFractureBlocks;

    AeroControl left_wing;
    AeroControl right_wing;
    AeroControl rudder;
    Aero tail;
    RigidBody aircraft;
    elevate::Vector3 windspeed;
	std::vector<AircraftParts> aircraftParts;

    float left_wing_control;
    float right_wing_control;
    float rudder_control;

    float friction = 0.9f;
	float restitution = 0.01f;
	float tolerance = 0.01f;

	float groundPlaneOffset = 0.0f;

    float fps;
    float avgFps;
	float frameTimeMs;
    float frameCount;
	float timeElapsed;
    float fpsSum;
    int numFramesAvg = 100;
    float physicsTime;
    float physicsTimeSum;
    float physicsTimeAvg;

	std::vector<PhysicsObject*> allSceneObjects;
    int sceneObjectIndex;
	PhysicsObject* selectedSceneObject = nullptr;

    std::vector<DebugLine> m_DebugLines;
    GLuint m_DebugLineVAO = 0;
    GLuint m_DebugLineVBO = 0;

    void buildContactDebugLines()
    {
        m_DebugLines.clear();

        for (unsigned i = 0; i < cData.contactCount; ++i)
        {
            const elevate::Contact& c = cData.contacts[i];

            glm::vec3 p(
                (float)c.contactPoint.x,
                (float)c.contactPoint.y,
                (float)c.contactPoint.z);

            glm::vec3 n(
                (float)c.contactNormal.x,
                (float)c.contactNormal.y,
                (float)c.contactNormal.z);

            float nLen = 2.0f;

            DebugLine normalLine;
            normalLine.p0 = p;
            normalLine.p1 = p + n * nLen;
            normalLine.color = glm::vec3(1.0f, 0.0f, 0.0f);
            m_DebugLines.push_back(normalLine);

            float s = 0.2f;

            DebugLine crossX;
            crossX.p0 = p + glm::vec3(-s, 0.0f, 0.0f);
            crossX.p1 = p + glm::vec3(s, 0.0f, 0.0f);
            crossX.color = glm::vec3(0.0f, 1.0f, 0.0f);
            m_DebugLines.push_back(crossX);

            DebugLine crossY;
            crossY.p0 = p + glm::vec3(0.0f, -s, 0.0f);
            crossY.p1 = p + glm::vec3(0.0f, s, 0.0f);
            crossY.color = glm::vec3(0.0f, 1.0f, 0.0f);
            m_DebugLines.push_back(crossY);

            DebugLine crossZ;
            crossZ.p0 = p + glm::vec3(0.0f, 0.0f, -s);
            crossZ.p1 = p + glm::vec3(0.0f, 0.0f, s);
            crossZ.color = glm::vec3(0.0f, 1.0f, 0.0f);
            m_DebugLines.push_back(crossZ);
        }
    }

    void drawDebugLines()
    {
        return;

        if (m_DebugLines.empty())
            return;

        std::vector<float> vertexData;
        vertexData.reserve(m_DebugLines.size() * 2 * 6); 

        for (const auto& line : m_DebugLines)
        {
            // First endpoint
            vertexData.push_back(line.p0.x);
            vertexData.push_back(line.p0.y);
            vertexData.push_back(line.p0.z);
            vertexData.push_back(line.color.x);
            vertexData.push_back(line.color.y);
            vertexData.push_back(line.color.z);

            // Second endpoint
            vertexData.push_back(line.p1.x);
            vertexData.push_back(line.p1.y);
            vertexData.push_back(line.p1.z);
            vertexData.push_back(line.color.x);
            vertexData.push_back(line.color.y);
            vertexData.push_back(line.color.z);
        }

        glBindVertexArray(m_DebugLineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_DebugLineVBO);

        glBufferData(GL_ARRAY_BUFFER,
            vertexData.size() * sizeof(float),
            vertexData.data(),
            GL_DYNAMIC_DRAW);

        lineShader.use();

        lineShader.setMat4("view", view);
        lineShader.setMat4("projection", projection);

        glLineWidth(2.0f);

        glDrawArrays(GL_LINES, 0, (GLsizei)m_DebugLines.size() * 2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    bool DispatchCollisionPair(
        const CollisionBody& a,
        const CollisionBody& b,
        elevate::CollisionData& cData)
    {
        using ST = ShapeType;

        if (!cData.hasMoreContacts()) return false;

        switch (a.shapeType)
        {
        case ST::Box:
            switch (b.shapeType)
            {
            case ST::Box:
                return elevate::CollisionDetector::boxAndBox(*a.box, *b.box, &cData);

            case ST::Sphere:
                return elevate::CollisionDetector::boxAndSphere(*a.box, *b.sphere, &cData);

            case ST::HalfSpace:
                return elevate::CollisionDetector::boxAndHalfSpace(*a.box, *b.plane, &cData);
            }
            break;

        case ST::Sphere:
            switch (b.shapeType)
            {
            case ST::Box:
                return elevate::CollisionDetector::boxAndSphere(*b.box, *a.sphere, &cData);

            case ST::Sphere:
                return elevate::CollisionDetector::sphereAndSphere(*a.sphere, *b.sphere, &cData);

            case ST::HalfSpace:
                return elevate::CollisionDetector::sphereAndHalfSpace(*a.sphere, *b.plane, &cData);
            }
            break;

        case ST::HalfSpace:
            switch (b.shapeType)
            {
            case ST::Box:
                return elevate::CollisionDetector::boxAndHalfSpace(*b.box, *a.plane, &cData);

            case ST::Sphere:
                return elevate::CollisionDetector::sphereAndHalfSpace(*b.sphere, *a.plane, &cData);

            case ST::HalfSpace:
                return false;
            }
            break;
        }

        return false;
    }

};
