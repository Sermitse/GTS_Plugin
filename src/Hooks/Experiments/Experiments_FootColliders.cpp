#include "Hooks/Experiments/Experiments_FootColliders.hpp"
#include "Data/Transient.hpp"

using namespace GTS;

namespace Convert {
    glm::mat4 ConvertNiTransformToGLM(const RE::NiTransform& transform) {
        glm::mat4 result(1.0f);

        // rotation (transpose)
        result[0][0] = transform.rotate.entry[0][0];
        result[1][0] = transform.rotate.entry[0][1];
        result[2][0] = transform.rotate.entry[0][2];

        result[0][1] = transform.rotate.entry[1][0];
        result[1][1] = transform.rotate.entry[1][1];
        result[2][1] = transform.rotate.entry[1][2];

        result[0][2] = transform.rotate.entry[2][0];
        result[1][2] = transform.rotate.entry[2][1];
        result[2][2] = transform.rotate.entry[2][2];

        result[3][0] = transform.translate.x;
        result[3][1] = transform.translate.y;
        result[3][2] = transform.translate.z;

        result[0] *= transform.scale;
        result[1] *= transform.scale;
        result[2] *= transform.scale;

        return result;
    }

    glm::mat4 NiTransformToMatrix(const RE::NiTransform& t) {
        glm::mat4 m(1.0f);

        m[0][0] = t.rotate.entry[0][0] * t.scale;
        m[0][1] = t.rotate.entry[0][1] * t.scale;
        m[0][2] = t.rotate.entry[0][2] * t.scale;

        m[1][0] = t.rotate.entry[1][0] * t.scale;
        m[1][1] = t.rotate.entry[1][1] * t.scale;
        m[1][2] = t.rotate.entry[1][2] * t.scale;

        m[2][0] = t.rotate.entry[2][0] * t.scale;
        m[2][1] = t.rotate.entry[2][1] * t.scale;
        m[2][2] = t.rotate.entry[2][2] * t.scale;


        m[3][0] = t.translate.x;
        m[3][1] = t.translate.y;
        m[3][2] = t.translate.z;

        return m;
    }
}

namespace GetData {
    void ClearData(Actor* actor) {
        if (auto data = Transient::GetActorData(actor)) {
            data->FootwearInfo.last_armor_name.clear();
            data->FootwearInfo.ref_mesh_shapes.clear();
            data->FootwearInfo.real_shapes.clear();
        }
    }
    void RecordObjectNameData(std::string obj_name, Actor* actor) {
        if (auto data = Transient::GetActorData(actor)) {
            data->FootwearInfo.last_armor_name = obj_name;
        }
    }
    void RecordFoundShapes(RE::BSTriShape* shape, Actor* actor) {
        if (auto data = Transient::GetActorData(actor)) {
            data->FootwearInfo.ref_mesh_shapes.push_back(shape);
        }
    }
    void RecordRealShapes(RE::BSTriShape* shape, Actor* actor) {
        if (auto data = Transient::GetActorData(actor)) {
            data->FootwearInfo.real_shapes.push_back(shape);
        }
    }
    FootwearInformation GetFootwearInfo(Actor* actor) {
        if (auto data = Transient::GetActorData(actor)) {
            return data->FootwearInfo;
        }
        return FootwearInformation();
    }
}

namespace {
    void DrawPartition(RE::NiSkinInstance* skin, RE::NiSkinPartition::Partition& part, RE::BSTriShape* shape, bool right) {
        if (!part.buffData || !part.triList)
            return;


        uint8_t* base = part.buffData->rawVertexData;
        uint32_t stride = part.vertexDesc.GetSize();


        uint32_t posOffset = part.vertexDesc.GetAttributeOffset(
            RE::BSGraphics::Vertex::VA_POSITION
        );

        uint32_t skinOffset = part.vertexDesc.GetAttributeOffset(
            RE::BSGraphics::Vertex::VA_SKINNING
        );
        
        if (skin->bones) {}
       
        auto ReadPos = [&](uint16_t i)
        {
            float* p = reinterpret_cast<float*>(
                base + i * stride + posOffset
            );

            return glm::vec3(
                p[0],
                p[1],
                p[2]
            );
        };


        auto ReadSkin = [&](uint16_t i)
        {
            return *reinterpret_cast<VertexSkinData*>(
                base + i * stride + skinOffset
            );
        };

        auto ApplySkinning = [&](glm::vec3 pos, const VertexSkinData& skinData) {
            glm::vec4 result(0.0f);
            float totalWeight = 0.0f;


            glm::mat4 root =
                Convert::ConvertNiTransformToGLM(
                    skin->skinData->rootParentToSkin
                );


            for (int k = 0; k < 4; k++)
            {
                float weight = skinData.weights[k] / 65535.0f;

                if(weight <= 0.0f)
                    continue;


                uint8_t localBone = skinData.bones[k];

                if(localBone >= part.numBones)
                    continue;
                uint16_t skinBoneIndex = part.bones[localBone];

                 logger::info(
                    "partition bone {} -> skinBone {}",
                    localBone,
                    skinBoneIndex
                );
                logger::info(
                    "skin bones {} boneData count {}",
                    skin->skinData->bones,
                    skin->skinData->bones
                );
                auto boneNode = skin->bones[skinBoneIndex];

                if (!boneNode)
                    continue;
                auto& boneData = skin->skinData->boneData[skinBoneIndex];//skin->skinData->boneData[localBone];
                logger::info(
                    "Bone {} world {} {} {}",
                    skinBoneIndex,
                    boneNode->world.translate.x,
                    boneNode->world.translate.y,
                    boneNode->world.translate.z
                    );

                glm::mat4 boneWorld = Convert::ConvertNiTransformToGLM(boneNode->world);
                glm::mat4 skinToBone = Convert::ConvertNiTransformToGLM(skin->skinData->boneData[skinBoneIndex].skinToBone);


                glm::vec4 transformed = 
                    root
                    * boneWorld 
                    *skinToBone 
                    * glm::vec4(pos,1.0f);
                result += transformed * weight;
                totalWeight += weight;

                logger::info("weights {} {} {} {} sum {}",
                    skinData.weights[0],
                    skinData.weights[1],
                    skinData.weights[2],
                    skinData.weights[3],
                totalWeight);
            }
            if(totalWeight > 0.0f) {
                result /= totalWeight;
            }
            return glm::vec3(result);
        };



        for(uint32_t t = 0; t < part.triangles; t++)
        {
            uint32_t index = t * 3;


            uint16_t ia = part.vertexMap[part.triList[index]];
            uint16_t ib = part.vertexMap[part.triList[index+1]];
            uint16_t ic = part.vertexMap[part.triList[index+2]];


            auto a = ApplySkinning(
                ReadPos(ia),
                ReadSkin(ia)
            );

            auto b = ApplySkinning(
                ReadPos(ib),
                ReadSkin(ib)
            );

            auto c = ApplySkinning(
                ReadPos(ic),
                ReadSkin(ic)
            );


            DebugDraw::DrawTriangle(
                a,
                b,
                c,
                glm::mat4(1.0f),
                8000,
                {0, 1.0f, 1.0f,1},
                3.0f
            );
        }
    }
    void DebugShape(RE::BSTriShape* shape, bool right) {
        auto& triData = shape->GetTrishapeRuntimeData();
        logger::info("{} vertices={} triangles={}",shape->name.c_str(),triData.vertexCount,triData.triangleCount);
        auto& geo = shape->GetGeometryRuntimeData();
        logger::info("RendererData {}",geo.rendererData ? "YES" : "NO");
        if (geo.skinInstance) {
            logger::info("SkinInstance true");
            auto Partition = geo.skinInstance.get()->skinPartition;
            if (Partition) {
                auto partition = Partition.get();
                logger::info("skinPartition true");
                if (partition) {
                    logger::info("SkinInstance.get() true");

                    for (uint32_t i = 0; i < partition->numPartitions; i++)
                    {
                        DrawPartition(geo.skinInstance.get(), partition->partitions[i], shape, right);
                    }
                }
            }
        } 
    }
}

namespace ColliderShapes {
    void FindRuntimeShapes(RE::NiAVObject* object, RE::Actor* actor) {
        if (object && actor) {
            auto data = GetData::GetFootwearInfo(actor);
            if (auto* shape = netimmerse_cast<RE::BSTriShape*>(object)) {
                logger::info("Runtime Shape: {}", shape->name.c_str());
                for (auto shape_data: data.ref_mesh_shapes) {
                    if (shape_data->name == shape->name.c_str()) {
                        GetData::RecordRealShapes(shape, actor);
                        logger::info("Shape Matches");
                    }
                }
            }

            if (auto* node = object->AsNode())
            {
                for (auto& child : node->children) {
                    FindRuntimeShapes(child.get(), actor);
                }
            }
        }
    }
}

namespace GTS {
    void VisitGeometry(RE::NiAVObject* object, Actor* actor, bool skin) {
        if (object) {
            if (auto* shape = netimmerse_cast<RE::BSTriShape*>(object)) {
                logger::info("{} BSTriShape: {}", skin ? "Skin" : "Armor", shape->name.c_str());
                GetData::RecordFoundShapes(shape, actor);
            }
            if (auto* node = object->AsNode()) {
                for (auto& child : node->children) {
                    VisitGeometry(child.get(), actor, skin);
                }
            }
        }
    }

    void GetShapeFromWornArmor(Actor* actor) {
        auto armor = actor->GetWornArmor(BGSBipedObjectForm::BipedObjectSlot::kFeet);
        if (armor) {
            GetData::RecordObjectNameData(armor->GetFullName(), actor);
            for (auto* armors: armor->armorAddons) {
                if (!armors) continue;
                    actor->VisitArmorAddon(armor, armors, [&](bool firstPerson, RE::NiAVObject& obj) {
                    logger::info("Root: {}", obj.name.c_str());
                    VisitGeometry(&obj, actor, false);
                    return RE::BSVisit::BSVisitControl::kContinue;
                });
            }
        } else {
            auto skin = actor->GetSkin(BGSBipedObjectForm::BipedObjectSlot::kFeet);
            if (skin) {
                for (auto* skins: skin->armorAddons) {
                    if (!skins) continue;
                    actor->VisitArmorAddon(skin, skins, [&](bool firstPerson, RE::NiAVObject& obj) {
                        logger::info("Skin Root: {}", obj.name.c_str());
                        VisitGeometry(&obj, actor, true);
                        return RE::BSVisit::BSVisitControl::kContinue;
                    });
                }
            }
        }
        ColliderShapes::FindRuntimeShapes(actor->Get3D(), actor);
    }

    void ScanFootwearColliders(Actor* actor, bool right) {
        auto armor = actor->GetWornArmor(BGSBipedObjectForm::BipedObjectSlot::kFeet);

        if (armor) {
            if (armor->GetFullName() != GetData::GetFootwearInfo(actor).last_armor_name) {
                GetData::ClearData(actor);
                GetShapeFromWornArmor(actor);
            }
        } else {
            auto skin = actor->GetSkin(BGSBipedObjectForm::BipedObjectSlot::kFeet);
            if (skin->GetFullName() != GetData::GetFootwearInfo(actor).last_armor_name) {
                GetData::ClearData(actor);
                GetShapeFromWornArmor(actor);
            }
        } 

        for (auto data: GetData::GetFootwearInfo(actor).real_shapes) {
            logger::info("Debugging real shape");
            DebugShape(data, right);
        }
    }
}

