#include "App/App.h"
#include "Tools/Logger.h"

GameManager* SceneManager::s_gameManager = nullptr;
SceneType SceneManager::s_currentScene = SceneType::Car;

App::App(unsigned int screenWidth, unsigned int screenHeight)
    : width(screenWidth), height(screenHeight)
{
    mWindow = new Window();

    if (!mWindow->init(screenWidth, screenHeight, "Elevate Engine")) {
        Logger::log(1, "%s error: Window init error\n", __FUNCTION__);
    }

    mGameManager = new GameManager(mWindow, screenWidth, screenHeight);

    SceneManager::Init(mGameManager);
    mGameManager->currentScene = SceneManager::s_currentScene
        ;
    SceneManager::LoadScene(mGameManager->currentScene);

}

App::~App()
{
    mWindow->cleanup();
    delete mGameManager;
}

void App::run()
{
        while (mWindow->isOpen()) {
        currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        mGameManager->setUpDebugUI();

        if(SceneManager::s_currentScene != mGameManager->currentScene)
        {
            mGameManager->ResetState();
            SceneManager::LoadScene(mGameManager->currentScene);
            SceneManager::s_currentScene = mGameManager->currentScene;
		}

        mGameManager->showDebugUI();
        mWindow->clear();

        mGameManager->update(deltaTime);
        mGameManager->setupCamera(width, height);
        mGameManager->setSceneData();
        mGameManager->render();

        mGameManager->renderDebugUI();
        mWindow->mainLoop();
    }
}