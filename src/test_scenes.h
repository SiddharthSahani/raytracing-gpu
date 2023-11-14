
#pragma once

#include "src/rt_objects.h"


rt::clScene create_scene_1() {
    rt::Scene scene;

    scene.spheres[0] = {
        .position = {0.0f, 0.0f, 0.0f},
        .radius = 1.0f,
        .material_idx = 0
    };
    scene.spheres[1] = {
        .position = {0.0f, -6.0f, 0.0f},
        .radius = 5.0f,
        .material_idx = 1
    };

    scene.materials[0] = {
        .color = {0.2f, 0.9f, 0.8f}
    };
    scene.materials[1] = {
        .color = {1.0f, 0.0f, 1.0f}
    };

    scene.num_spheres = 2;

    scene.sky_color = {210.0f, 210.0f, 230.0f};
    scene.sky_color /= 255.0f;

    return rt::to_clScene(scene);
}


rt::clScene create_scene_2() {
    rt::Scene scene;

    scene.spheres[0] = {
        .position = {0.0f, 0.0f, 0.0f},
        .radius = 1.0f,
        .material_idx = 0
    };
    scene.spheres[1] = {
        .position = {0.0f, -6.0f, 0.0f},
        .radius = 5.0f,
        .material_idx = 1
    };

    scene.materials[0] = {
        .color = {0.0f, 0.2f, 0.8f}
    };
    scene.materials[1] = {
        .color = {0.0f, 1.0f, 1.0f}
    };

    scene.num_spheres = 2;

    scene.sky_color = {180.0f, 150.0f, 200.0f};
    scene.sky_color /= 255.0f;

    return rt::to_clScene(scene);
}


rt::clScene create_scene_3() {
    rt::Scene scene;

    scene.spheres[0] = {
        .position = {0.0f, 0.0f, 0.0f},
        .radius = 1.0f,
        .material_idx = 0
    };
    scene.spheres[1] = {
        .position = {2.0f, 0.0f, 0.0f},
        .radius = 1.0f,
        .material_idx = 1
    };
    scene.spheres[2] = {
        .position = {0.0f, -101.0f, 0.0f},
        .radius = 100.0f,
        .material_idx = 2
    };

    scene.materials[0] = {
        .color = {1.0f, 0.0f, 1.0f}
    };
    scene.materials[1] = {
        .color = {1.0f, 0.0f, 0.0f}
    };
    scene.materials[2] = {
        .color = {0.3f, 0.8f, 0.3f}
    };

    scene.num_spheres = 3;

    scene.sky_color = {225.0f, 225.0f, 255.0f};
    scene.sky_color /= 255.0f;

    return rt::to_clScene(scene);
}
