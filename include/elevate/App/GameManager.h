#pragma once

#include "OpenGL/Renderer.h"
#include "OpenGL/RenderData.h"
#include "Window/Window.h"

#include "App/InputManager.h"

#include "App/Camera.h"
#include "GameObjects/Cube.h"
#include "GameObjects/Sphere.h"
#include "GameObjects/Line.h"
#include "GameObjects/Plane.h"
#include "Physics/Demos/AmmoRound.h"
#include "Physics/Demos/FloatingSphere.h"
#include "Physics/pfgen.h"
#include "Physics/pworld.h"
#include "Physics/body.h"
#include "Physics/World.h"

struct DebugLine
{
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 color;
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

    void fireRound() {};
	void setCurrentShotType(ShotType type) { currentShotType = type; }

    void update(float deltaTime);

    void render();

    void setUpDebugUI();
    void showDebugUI();
    void renderDebugUI();

    void setPushDir(float newDir) { pushDirX = newDir; }

private:
    void ShowCameraControlWindow(Camera& cam);
    void ShowLightControlWindow(DirLight& light);
    void ShowAmmoWindow();
	void ShowBuoyancyWindow();

    void RemoveDestroyedGameObjects();

    Renderer* renderer;
    Window* window;
    Camera* camera;

    InputManager* inputManager;

    std::vector<GameObject*> gameObjects;

    Shader ammoShader{};
	Shader cubeShader{};
	Shader lineShader{};

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
    AmmoRound ammo[ammoRounds];
    ShotType currentShotType = PISTOL;

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

    bool showBuoyanceDemo = true;

	bool cubeDemo = false;
	elevate::RigidBody* testBody = nullptr;
	elevate::RigidBody* testBody2 = nullptr;
    World* rbWorld;
	ForceRegistry rbRegistry;
	Gravity* rbGravity;

	bool sphereDemo = false;
	Sphere* sphere = nullptr;
	Sphere* sphere2 = nullptr;
	elevate::RigidBody* sphereBody = nullptr;
	elevate::RigidBody* sphereBody2 = nullptr;

    elevate::RigidBody* planeBody = nullptr;

    static const int MaxStackCubes = 5;

    elevate::CollisionBox* cStackBoxes[MaxStackCubes];
    elevate::RigidBody* stackBodies[MaxStackCubes];
    Cube* stackCubes[MaxStackCubes];
    int                    numStackCubes = 0;

    bool stackDemo = true;


    elevate::Contact contacts[256];
    elevate::CollisionData cData;
    elevate::ContactResolver resolver;
	elevate::CollisionSphere* cSpheres[2];
	elevate::CollisionSphere* cSphere0;
	elevate::CollisionSphere* cSphere1;
	elevate::CollisionBox* cBox0;
	elevate::CollisionBox* cBox1;
    elevate::CollisionPlane* cPlane;

    void generateContacts();

    std::vector<DebugLine> m_DebugLines;
    GLuint m_DebugLineVAO = 0;
    GLuint m_DebugLineVBO = 0;

    void buildContactDebugLines()
    {
        m_DebugLines.clear();

        for (unsigned i = 0; i < cData.contactCount; ++i)
        {
            const elevate::Contact& c = cData.contacts[i];

            // Contact point and normal, converted to glm
            glm::vec3 p(
                (float)c.contactPoint.x,
                (float)c.contactPoint.y,
                (float)c.contactPoint.z);

            glm::vec3 n(
                (float)c.contactNormal.x,
                (float)c.contactNormal.y,
                (float)c.contactNormal.z);

            // Scale for visibility (tweak as needed)
            float nLen = 2.0f;

            // Line showing the normal
            DebugLine normalLine;
            normalLine.p0 = p;
            normalLine.p1 = p + n * nLen;
            normalLine.color = glm::vec3(1.0f, 0.0f, 0.0f); // red
            m_DebugLines.push_back(normalLine);

            // Small cross at the contact point (3 axis-aligned mini lines)
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
        if (m_DebugLines.empty())
            return;

        // Build a CPU-side vertex array for all lines
        std::vector<float> vertexData;
        vertexData.reserve(m_DebugLines.size() * 2 * 6); // 2 vertices per line, 6 floats each

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

        // Assuming your line shader has view/projection uniforms
        lineShader.setMat4("view", view);
        lineShader.setMat4("projection", projection);

        glLineWidth(2.0f);

        glDrawArrays(GL_LINES, 0, (GLsizei)m_DebugLines.size() * 2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

};
