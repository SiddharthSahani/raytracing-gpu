
#include "src/raytracer/scene.h"
#include <json/json.hpp>
#include <fstream>

typedef json::JSON::Class JsonType;


auto defaultMaterial = rt::createEmissiveMaterial({1.0f, 0.8f, 0.8f}, 10.0f);


std::string readFile(const char* filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::ate);

    int fileSize = file.tellg();
    if (fileSize == -1) {
        // file doesnt exist
        printf("ERROR (`readFile`): Unable to read %s\n", filepath);
        fileSize = 0;
    }
    std::string fileContents(fileSize, '\0');

    file.seekg(0);
    file.read(&fileContents[0], fileSize);
    return fileContents;
}


std::vector<std::shared_ptr<rt::internal::Material>> parseMaterials(json::JSON& materialsJsonArr) {
    int matCount = materialsJsonArr.size();
    std::vector<std::shared_ptr<rt::internal::Material>> materials(matCount);

    for (int matLoopIdx = 0; matLoopIdx < matCount; matLoopIdx++) {
        json::JSON& jsonObj = materialsJsonArr[matLoopIdx];
        if (jsonObj.JSONType() != JsonType::Object) {
            printf("ERROR (`parseMaterials`): Material %d is not a json object\n", matLoopIdx);
            materials[matLoopIdx] = defaultMaterial;
            continue;
        }

        json::JSON& color = jsonObj["color"];
        json::JSON& smoothness = jsonObj["smoothness"];
        json::JSON& emissionColor = jsonObj["emissionColor"];
        json::JSON& emissionPower = jsonObj["emissionPower"];

        // diffuse material
        if (color.JSONType() == JsonType::Array) {
            // validating color (vec3)
            {
                // size of color array
                if (color.size() != 3) {
                    printf("ERROR (`parseMaterials`): Material %d ::color is not an array of size 3\n", matLoopIdx);
                    materials[matLoopIdx] = defaultMaterial;
                    continue;
                }
                // each element of color array
                for (int i = 0; i < 3; i++) {
                    if (color[i].JSONType() != JsonType::Floating) {
                        printf("ERROR (`parseMaterials`): Material %d ::color[%d] is not a floating point number\n", matLoopIdx, i);
                        materials[matLoopIdx] = defaultMaterial;
                        continue;
                    }
                }
            }
            // validating smoothness [0.0f - 1.0f]
            {
                // is a floating point number
                if (smoothness.JSONType() != JsonType::Floating) {
                    printf("ERROR (`parseMaterials`): Material %d ::smoothness is not a floating point number\n", matLoopIdx);
                    materials[matLoopIdx] = defaultMaterial;
                    continue;
                }
                // is between 0.0 and 1.0
                float smoothness_ = smoothness.ToFloat();
                if (smoothness_ < 0.0f || smoothness_ > 1.0f) {
                    printf("ERROR (`parseMaterials`): Material %d ::smoothness should be in range [0.0f - 1.0f]\n", matLoopIdx);
                    materials[matLoopIdx] = defaultMaterial;
                    continue;
                }
            }

            glm::vec3 color_ = {color[0].ToFloat(), color[1].ToFloat(), color[2].ToFloat()};
            float smoothness_ = smoothness.ToFloat();
            materials[matLoopIdx] = rt::createMaterial(color_, smoothness_);
        }
        // emissive material
        else if (emissionColor.JSONType() == JsonType::Array) {
            // validating emissionColor (vec3)
            {
                // size of emissionColor array
                if (emissionColor.size() != 3) {
                    printf("ERROR (`parseMaterials`): Material %d ::emissionColor is not an array of size 3\n", matLoopIdx);
                    materials[matLoopIdx] = defaultMaterial;
                    continue;
                }
                // each element of emissionColor array
                for (int i = 0; i < 3; i++) {
                    if (emissionColor[i].JSONType() != JsonType::Floating) {
                        printf("ERROR (`parseMaterials`): Material %d ::emissionColor[%d] is not a floating point number\n", matLoopIdx, i);
                        materials[matLoopIdx] = defaultMaterial;
                        continue;
                    }
                }
            }
            // validating emissionPower [0.0f - inf]
            {
                // is a floating point number
                if (emissionPower.JSONType() != JsonType::Floating) {
                    printf("ERROR (`parseMaterials`): Material %d ::emissionPower is not a floating point number\n", matLoopIdx);
                    materials[matLoopIdx] = defaultMaterial;
                    continue;
                }
                // is above 0.0f
                float emissionPower_ = emissionPower.ToFloat();
                if (emissionPower_ < 0.0f) {
                    printf("ERROR (`parseMaterials`): Material %d ::emissionPower is negative\n", matLoopIdx);
                    materials[matLoopIdx] = defaultMaterial;
                    continue;                    
                }

            }

            glm::vec3 emissionColor_ = {emissionColor[0].ToFloat(), emissionColor[1].ToFloat(), emissionColor[2].ToFloat()};
            float emissionPower_ = emissionPower.ToFloat();
            materials[matLoopIdx] = rt::createEmissiveMaterial(emissionColor_, emissionPower_);
        }
        // invalid material
        else {
            printf("ERROR (`parseMaterials`): Material %d, 'color' or 'emissionColor' should be an array\n", matLoopIdx);
            materials[matLoopIdx] = defaultMaterial;
            continue;
        }
    }

    return materials;
}


rt::Object parseSphere(json::JSON& jsonObj, int objIdx, bool& valid) {
    json::JSON& position = jsonObj["position"];
    json::JSON& radius = jsonObj["radius"];

    valid = true;

    // validating position (vec3)
    {
        // is an array of size 3
        if (position.JSONType() != JsonType::Array || position.size() != 3) {
            printf("ERROR (`parseSphere`): Object %d ::position is not an array of size 3\n", objIdx);
            valid = false;
        }
        // each element of position array
        for (int i = 0; i < 3; i++) {
            if (position[i].JSONType() != JsonType::Floating) {
                printf("ERROR (`parseSphere`): Object %d ::position[%d] is not a floating point number\n", objIdx, i);
                valid = false;
            }
        }
    }
    // validating radius [0.0f - inf]
    {
        // is floating point number
        if (radius.JSONType() != JsonType::Floating) {
            printf("ERROR (`parseSphere`): Object %d ::radius is not a floating point number\n", objIdx);
            valid = false;
        }
        // is not negative
        float radius_ = radius.ToFloat();
        if (radius_ < 0.0f) {
            printf("ERROR (`parseSphere`): Object %d ::radius is negative\n", objIdx);
            valid = false;
        }
    }

    if (!valid) {
        return {};
    }

    rt::internal::Sphere internal = {
        .position = {position[0].ToFloat(), position[1].ToFloat(), position[2].ToFloat(), 1.0f},
        .radius = radius.ToFloat()
    };

    rt::Object obj = {
        .internal = internal
    };
    return obj;
}


rt::Object parseTriangle(json::JSON& jsonObj, int objIdx, bool& valid) {
    json::JSON& v0 = jsonObj["v0"];
    json::JSON& v1 = jsonObj["v1"];
    json::JSON& v2 = jsonObj["v2"];

    valid = true;

    // validating v0 (vec3)
    {
        // is an array of size 3
        if (v0.JSONType() != JsonType::Array || v0.size() != 3) {
            printf("ERROR (`parseSphere`): Object %d ::v0 is not an array of size 3\n", objIdx);
            valid = false;
        }
        // each element of v0 array
        for (int i = 0; i < 3; i++) {
            if (v0[i].JSONType() != JsonType::Floating) {
                printf("ERROR (`parseSphere`): Object %d ::v0[%d] is not a floating point number\n", objIdx, i);
                valid = false;
            }
        }
    }
    // validating v1 (vec3)
    {
        // is an array of size 3
        if (v1.JSONType() != JsonType::Array || v1.size() != 3) {
            printf("ERROR (`parseSphere`): Object %d ::v1 is not an array of size 3\n", objIdx);
            valid = false;
        }
        // each element of v1 array
        for (int i = 0; i < 3; i++) {
            if (v1[i].JSONType() != JsonType::Floating) {
                printf("ERROR (`parseSphere`): Object %d ::v1[%d] is not a floating point number\n", objIdx, i);
                valid = false;
            }
        }
    }
    // validating v2 (vec3)
    {
        // is an array of size 3
        if (v2.JSONType() != JsonType::Array || v2.size() != 3) {
            printf("ERROR (`parseSphere`): Object %d ::v2 is not an array of size 3\n", objIdx);
            valid = false;
        }
        // each element of v2 array
        for (int i = 0; i < 3; i++) {
            if (v2[i].JSONType() != JsonType::Floating) {
                printf("ERROR (`parseSphere`): Object %d ::v2[%d] is not a floating point number\n", objIdx, i);
                valid = false;
            }
        }
    }

    if (!valid) {
        return {};
    }

    rt::internal::Triangle internal = {
        .v0 = {v0[0].ToFloat(), v0[1].ToFloat(), v0[2].ToFloat(), 1.0f},
        .v1 = {v1[0].ToFloat(), v1[1].ToFloat(), v1[2].ToFloat(), 1.0f},
        .v2 = {v2[0].ToFloat(), v2[1].ToFloat(), v2[2].ToFloat(), 1.0f}
    };

    rt::Object obj = {
        .internal = internal
    };
    return obj;
}


std::vector<std::pair<rt::Object, int>> parseObjects(json::JSON& objectsJsonArr, int matCount) {
    int objCount = objectsJsonArr.size();
    std::vector<std::pair<rt::Object, int>> objects;

    for (int objLoopIdx = 0; objLoopIdx < objCount; objLoopIdx++) {
        json::JSON& jsonObj = objectsJsonArr[objLoopIdx];
        if (jsonObj.JSONType() != JsonType::Object) {
            printf("ERROR (`parseObjects`): Object %d is not a json object\n", objLoopIdx);
            continue;
        }

        json::JSON& type = jsonObj["type"];
        if (type.JSONType() != JsonType::String) {
            printf("ERROR (`parseObjects`): Object %d ::type is not a string\n", objLoopIdx);
            continue;
        }

        json::JSON& matIdx = jsonObj["matIdx"];
        if (matIdx.JSONType() != JsonType::Integral) {
            printf("ERROR (`parseObjects`): Object %d ::matIdx is not an integer\n", objLoopIdx);
            continue;
        }

        std::string type_ = type.ToString();
        int matIdx_ = matIdx.ToInt();
        bool valid;
        rt::Object obj;

        if (type_ == "sphere") { 
            obj = parseSphere(jsonObj, objLoopIdx, valid);
        } else if (type_ == "triangle") {
            obj = parseTriangle(jsonObj, objLoopIdx, valid);
        } else {
            printf("ERROR (`parseObjects`): Object %d ::type is unknown\n", objLoopIdx);
            valid = false;
        }

        if (matIdx_ < 0 || matIdx_ >=  matCount) {
            printf("ERROR (`parseObjects`): Object %d ::matIdx should be in range [0 - number of materials)\n", objLoopIdx);
            valid = false;
        }

        if (!valid) {
            continue;
        }

        objects.push_back({obj, matIdx_});
    }

    return objects;
}


namespace rt {

Scene loadScene(const char* filepath, bool* success) {
    std::string contents = readFile(filepath);
    json::JSON contentsJson = json::JSON::Load(contents);

    bool _success;
    if (!success) {
        success = &_success;
    }

    if (contentsJson.JSONType() != JsonType::Object) {
        printf("ERROR (`loadScene`): Invalid json\n");
        *success = false;
        return {};
    }

    json::JSON& materialsJsonArr = contentsJson["materials"];
    if (materialsJsonArr.JSONType() != JsonType::Array) {
        printf("ERROR (`loadScene`): Json must have key='materials' as an array\n");
        *success = false;
        return {};
    }

    json::JSON& objectsJsonArr = contentsJson["objects"];
    if (objectsJsonArr.JSONType() != JsonType::Array) {
        printf("ERROR (`loadScene`): Json must have key='objects' as an array\n");
        *success = false;
        return {};
    }

    json::JSON& backgroundColor = contentsJson["backgroundColor"];
    if (backgroundColor.JSONType() != JsonType::Array || backgroundColor.size() != 3) {
        printf("ERROR (`loadScene`): Json must have key='backgroundColor' as an array of size 3\n");
        *success = false;
        return {};
    }
    for (int i = 0; i < 3; i++) {
        if (backgroundColor[i].JSONType() != JsonType::Floating) {
            printf("ERROR (`loadScene`): ::backgroundColor[%d] is not a floating point number\n", i);
            *success = false;
            return {};
        }
        float color_ = backgroundColor[i].ToFloat();
        if (color_ < 0.0f || color_ > 1.0f) {
            printf("ERROR (`loadScene`): ::backgroundColor[%d] should be in range [0.0 - 1.0]\n", i);
            *success = false;
            return {};
        }
    }

    std::vector<std::shared_ptr<internal::Material>> materials = parseMaterials(materialsJsonArr);
    std::vector<std::pair<Object, int>> objects = parseObjects(objectsJsonArr, materials.size());

    *success = true;

    Scene scene;
    for (auto& pair : objects) {
        pair.first.material = materials[pair.second];
        scene.objects.push_back(pair.first);
    }
    scene.backgroundColor = {backgroundColor[0].ToFloat(), backgroundColor[1].ToFloat(), backgroundColor[2].ToFloat()};
    return scene;
}

}
