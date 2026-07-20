#include "Hooks/Experiments/Experiments_FootColliders.hpp"
#include "Managers/HighHeel.hpp"
#include "Data/Transient.hpp"
#include "Config/Config.hpp"

using namespace GTS;

/*namespace Bones {
    bool PartitionUsesBone(RE::NiSkinInstance* skin,RE::NiSkinPartition::Partition& part,std::string_view targetName) {
        for (uint32_t i = 0; i < part.numBones; i++)
        {
            uint16_t skinBoneIndex = part.bones[i];

            auto bone = skin->bones[skinBoneIndex];

            if (!bone)
                continue;

            if (bone->name == targetName)
                return true;
        }

        return false;
    }
    bool AllowDraw(RE::NiSkinInstance* skin,RE::NiSkinPartition::Partition& part, bool right) {
        bool R = right && PartitionUsesBone(skin, part, "NPC R Foot [Rft ]");
        bool L = !right && PartitionUsesBone(skin, part, "NPC L Foot [Lft ]");
        return R || L;
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
    void DrawPartition(RE::Actor* actor, RE::NiSkinInstance* skin, RE::NiSkinPartition::Partition& part, RE::BSTriShape* shape, bool right) {
        if (!part.buffData || !part.triList)
            return;
        if (!Bones::AllowDraw(skin, part, right)) {
            return;
        }
        auto Data = Transient::GetActorData(actor);
        if (Data) {
            Data->Triangles.clear();
        }
        const float cutoff = HighHeelManager::GetInitialHeelHeight(actor) * 100.0f;
        auto foot_bone = find_node(actor, right ? "NPC R Foot [Rft ]" : "NPC L Foot [Lft ]");

        uint8_t* base = part.buffData->rawVertexData;
        uint32_t stride = part.vertexDesc.GetSize();
        int gSkinningPreset = GTS::Config::Advanced.iColliderLogic;

        uint32_t posOffset = part.vertexDesc.GetAttributeOffset(RE::BSGraphics::Vertex::VA_POSITION);
        uint32_t skinOffset =part.vertexDesc.GetAttributeOffset(RE::BSGraphics::Vertex::VA_SKINNING);


        auto ReadPos = [&](uint16_t i) -> RE::NiPoint3
        {
            float* p = reinterpret_cast<float*>(base + i * stride + posOffset);
            return RE::NiPoint3(p[0], p[1], p[2]);
        };


        auto ReadSkin = [&](uint16_t i)
        {
            VertexSkinData data{};

            uint8_t* ptr = base + i * stride + skinOffset;

            data.weights[0] = *reinterpret_cast<uint16_t*>(ptr + 0);
            data.weights[1] = *reinterpret_cast<uint16_t*>(ptr + 2);
            data.weights[2] = *reinterpret_cast<uint16_t*>(ptr + 4);
            data.weights[3] = *reinterpret_cast<uint16_t*>(ptr + 6);

            data.bones[0] = ptr[8];
            data.bones[1] = ptr[9];
            data.bones[2] = ptr[10];
            data.bones[3] = ptr[11];

            return data;
        };

        auto root = skin->skinData->rootParentToSkin;
        auto invRoot = root.Invert();

        auto ApplySkinning = [&](RE::NiPoint3 pos,const VertexSkinData& skinData) -> RE::NiPoint3 {
            RE::NiPoint3 result(0.0f,0.0f,0.0f);
            float totalWeight = skinData.weights[0] + skinData.weights[1] + skinData.weights[2] + skinData.weights[3];

            if(totalWeight <= 0.0f) return pos;

            for(int k = 0; k < 4; k++)
            {
                if(skinData.weights[k] == 0)    continue;

                float weight = skinData.weights[k] /totalWeight;

                uint8_t localBone = skinData.bones[k];
                if(localBone >= part.numBones)  continue;

                uint16_t skinBoneIndex = part.bones[localBone];

                if(!skin->boneWorldTransforms)  continue;

                auto boneWorld = skin->boneWorldTransforms[skinBoneIndex];
                if(!boneWorld)  continue;

                auto skinToBone = skin->skinData->boneData[skinBoneIndex].skinToBone;
                //
                // NiTransform multiplication
                //
                RE::NiPoint3 transformed = (*boneWorld) * (skinToBone * (invRoot * pos));

                result.x += transformed.x * weight;
                result.y += transformed.y * weight;
                result.z += transformed.z * weight;
            }
            return result;
        };
        auto IsVertexAboveFoot = [&](const VertexSkinData& skinData)
        {
            if (!foot_bone)
                return false;
            float limit = ((cutoff * 1.5f) + 20.0f) * root.scale;

            for(int k = 0; k < 4; k++)
            {
                if(skinData.weights[k] == 0)
                    continue;

                uint8_t localBone = skinData.bones[k];

                if(localBone >= part.numBones)
                    continue;

                uint16_t skinBoneIndex = part.bones[localBone];

                auto bone = skin->boneWorldTransforms[skinBoneIndex];

                if(!bone)
                    continue;


                float height = bone->translate.z -foot_bone->world.translate.z;
                if(height > limit) return true;
            }

            return false;
        };
        for(uint32_t t = 0; t < part.triangles; t++)
        {
            uint32_t index = t * 3;

            uint16_t ia =   part.vertexMap[part.triList[index]];
            uint16_t ib =   part.vertexMap[part.triList[index + 1]];
            uint16_t ic =   part.vertexMap[part.triList[index + 2]];

            if(IsVertexAboveFoot(ReadSkin(ia)) &&IsVertexAboveFoot(ReadSkin(ib)) && IsVertexAboveFoot(ReadSkin(ic)))
            {
                continue;
            }

            RE::NiPoint3 a =    ApplySkinning(ReadPos(ia),ReadSkin(ia));
            RE::NiPoint3 b =    ApplySkinning(ReadPos(ib),ReadSkin(ib));
            RE::NiPoint3 c =    ApplySkinning(ReadPos(ic),ReadSkin(ic));

            glm::vec3 ga(a.x, a.y, a.z);

            glm::vec3 gb(b.x,b.y,b.z);
            glm::vec3 gc(c.x,c.y,c.z);
            if (Data) {
                Data->Triangles.emplace_back(
                    FootTriangle{a,b,c,
                        {std::min({a.x,b.x,c.x}),std::min({a.y,b.y,c.y}),std::min({a.z,b.z,c.z})},
                        {std::max({a.x,b.x,c.x}),std::max({a.y,b.y,c.y}),std::max({a.z,b.z,c.z})}
                    }
                );
            }
            DebugDraw::DrawTriangle(ga,gb,gc,glm::mat4(1.0f),1000,{0,1.0f,1.0f,1},3.0f);
        }
    }
    void DebugShape(RE::Actor* actor, RE::BSTriShape* shape, bool right) {
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
                        DrawPartition(actor, geo.skinInstance.get(), partition->partitions[i], shape, right);
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
                //logger::info("{} BSTriShape: {}", skin ? "Skin" : "Armor", shape->name.c_str());
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
                    //logger::info("Root: {}", obj.name.c_str());
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
            DebugShape(actor, data, right);
        }
    }
}*/

