#include "InstancingTest.h"
#include <memory>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

std::unique_ptr<InstancingTest> instancingTest;

void onFrame();

int main(int, char**)
{
    instancingTest = std::make_unique<InstancingTest>();
    instancingTest->init();

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(onFrame, 0, 1);
#else
    while (instancingTest->running())
        onFrame();
#endif

    instancingTest->shutdown();

    return 0;
}

void onFrame()
{
    instancingTest->update();
}
