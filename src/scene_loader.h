
#pragma once

#include "src/raytracer/scene.h"


namespace rt {

Scene loadScene(const char* filepath, bool* success = nullptr);

}
