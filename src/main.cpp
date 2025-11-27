#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "App/App.h"

const unsigned int SCREEN_WIDTH = 1920;
const unsigned int SCREEN_HEIGHT = 1080;

int main()
{
    App app(SCREEN_WIDTH, SCREEN_HEIGHT);

    app.run();

    return 0;
}
