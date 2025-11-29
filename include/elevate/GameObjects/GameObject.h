#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGL/Shader.h"
#include "OpenGL/RenderData.h"
#include "OpenGL/UniformBuffer.h"
#include "OpenGL/Texture.h"
#include "Tools/Logger.h"

#include "Physics/core.h"

class GameObject {
public:
    GameObject(elevate::Vector3 pos, elevate::Vector3 scale, float yaw, Shader* shdr, class GameManager* gameMgr)
        : position(pos), scale(scale), yaw(yaw), shader(shdr), mGameManager(gameMgr)
    {
        size_t uniformMatrixBufferSize = 3 * sizeof(glm::mat4);
        mUniformBuffer.init(uniformMatrixBufferSize);
        Logger::log(1, "%s: matrix uniform buffer (size %i bytes) successfully created\n", __FUNCTION__, uniformMatrixBufferSize);

		rotationAxis = elevate::Vector3(0.0f, 1.0f, 0.0f);
		angle = 0.0f;
    }

    virtual void Draw(glm::mat4 viewMat, glm::mat4 proj) {
        shader->use();
        shader->setFloat("texTiling", texTiling);
        drawObject(viewMat, proj);
    }

    virtual void LoadMesh() = 0;

	void setGameManager(class GameManager* gameMgr) { mGameManager = gameMgr; }

    virtual Shader* GetShader() const { return shader; }

	GameManager* GetGameManager() const { return mGameManager; }

    elevate::Vector3 GetPosition() const { return position; }
    void SetPosition(elevate::Vector3 pos) { position = pos; }

	elevate::Vector3 GetScale() const { return scale; }
	void SetScale(elevate::Vector3 scl) { scale = scl; }

	elevate::Vector3 GetRotAxis() const { return rotationAxis; }
	void SetRotAxis(elevate::Vector3 rotAx) { rotationAxis = rotAx; }

	float GetAngle() const { return angle; }
	void SetAngle(float ang) { angle = ang; }

	glm::quat GetOrientation() const { return orientation; }
	void SetOrientation(glm::quat orient) { orientation = orient; }

    void SetRotationMatrix(glm::mat4 rotMat) {
        orientation = glm::quat_cast(rotMat);
	}
	Shader* GetShader() { return shader; }
	virtual void SetShader(Shader* shdr) { shader = shdr; }

    virtual void SetColor(glm::vec3 col) { color = col; }

    bool isDestroyed = false;

    void SetTexTiling(float tile) { texTiling = (GLfloat)tile; }

    glm::mat4 GetWorldMatrix() const {
        glm::mat4 modelMat = glm::mat4(1.0f);
        glm::vec3 posVec = glm::vec3(GetPosition().x, GetPosition().y, GetPosition().z);
        glm::vec3 scaleVec = glm::vec3(GetScale().x, GetScale().y, GetScale().z);
        modelMat = glm::translate(modelMat, posVec);
        modelMat *= glm::mat4_cast(orientation);
        modelMat = glm::scale(modelMat, scaleVec);
        return modelMat;
    }

protected:
    virtual void drawObject(glm::mat4 viewMat, glm::mat4 proj) = 0;

    float yaw;
    elevate::Vector3 position;
    elevate::Vector3 scale;
	elevate::Vector3 rotationAxis;
    float angle;
	glm::quat orientation;

    Shader* shader;
    RenderData renderData;
	Texture texture;
    class GameManager* mGameManager = nullptr;

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    GLfloat texTiling = 1.0f;

    UniformBuffer mUniformBuffer{};
};
