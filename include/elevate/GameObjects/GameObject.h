#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGL/Shader.h"
#include "OpenGL/RenderData.h"
#include "OpenGL/UniformBuffer.h"
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
    }

    virtual void Draw(glm::mat4 viewMat, glm::mat4 proj) {
        shader->use();
        drawObject(viewMat, proj);
    }

    virtual Shader* GetShader() const { return shader; }

	GameManager* GetGameManager() const { return mGameManager; }

    elevate::Vector3 GetPosition() const { return position; }
    void SetPosition(elevate::Vector3 pos) { position = pos; }

	elevate::Vector3 GetScale() const { return scale; }
	void SetScale(elevate::Vector3 scl) { scale = scl; }

    bool isDestroyed = false;

protected:
    virtual void drawObject(glm::mat4 viewMat, glm::mat4 proj) = 0;

    float yaw;
    elevate::Vector3 position;
    elevate::Vector3 scale;

    Shader* shader = nullptr;
    RenderData renderData;

    class GameManager* mGameManager = nullptr;

    UniformBuffer mUniformBuffer{};
};
