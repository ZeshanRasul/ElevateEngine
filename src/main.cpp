#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "App/App.h"

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 960;

int main()
{
    App app(SCREEN_WIDTH, SCREEN_HEIGHT);

    app.run();

    return 0;
}
