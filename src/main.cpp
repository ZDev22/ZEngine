#include "engine/app.hpp"

#if true // render the screen and tick the game (disable if it's a terminal game)

#include "engine/shaders.hpp"

int main() {
    compileShaders();
    App app{};
    app.run();
}

#else

int main() { Terminal(); }

#endif