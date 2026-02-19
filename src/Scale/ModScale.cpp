#include "Scale/ModScale.hpp"

using namespace GTS;

namespace {

	std::mutex scalesMutex;

	struct InitialScales {
        float model = 1.0f;
        float npc = 1.0f;

        InitialScales() {
			model = 1.0f;
			npc = 1.0f;
        }

        InitialScales(Actor* actor) {
            model = get_model_scale(actor) / game_getactorscale(actor);
            npc = get_npcnode_scale(actor);
        }

        InitialScales(float a_model, float a_npc) {
            model = a_model;
            npc = a_npc;
        }
    };


	// Global actor inital scales singleton
	std::unordered_map<RE::FormID, InitialScales>& GetInitialScales() {
		static std::unordered_map<RE::FormID, InitialScales> initScales;
		return initScales;
	}



	bool GetActorInitialScales(Actor* actor, InitialScales& out) {
		if (!actor) {
			logger::info("GetActorInitialScales: Actor Doesn't Exist");
			return false;
		}

		auto& initScales = GetInitialScales();
		const FormID id = actor->formID;

		std::lock_guard lock(scalesMutex);

		auto it = initScales.find(id);
		if (it != initScales.end()) {
			out = it->second;
			return true;
		}

		auto data = InitialScales(actor);

		auto [insertedIt, inserted] = initScales.emplace(id, std::move(data));
		out = insertedIt->second;
		return true;
	}

	void UpdateActorInitialScales(Actor* actor) {
		if (!actor) {
			logger::info("GetActorInitialScales: Actor Doesn't Exist");
			return;
		}

		auto& initScales = GetInitialScales();
		const FormID id = actor->formID;

		std::lock_guard lock(scalesMutex);

		auto it = initScales.find(id);
		if (it != initScales.end()) {
			return;
		}

		auto data = InitialScales(actor);
		initScales.emplace(id, std::move(data));

	}

	bool SetActorInitialScales(Actor* actor, const InitialScales& in){
		if (!actor) return false;

		auto& initScales = GetInitialScales();
		const auto id = actor->formID;

		std::lock_guard lock(scalesMutex);
		initScales[id] = in;
		return true;
	}

	void UpdateInitScale(Actor* actor) {
		UpdateActorInitialScales(actor);

	}
}

namespace GTS {
	// @ Sermit, do not call Get_Other_Scale, call get_natural_scale instead
	// get_natural_scale is much faster and safer as it uses the cache
	//
	// Get the current physical value for all nodes of the player
	// that we don't alter
	//
	// This one calls the NiNode stuff so should really be done
	// once per frame and cached
	//
	// This cache is stored in transient as `otherScales`
	float Get_Other_Scale(Actor* actor) {
		float ourScale = get_scale(actor);

		// Work with world scale to grab accumuated scales rather
		// than multiplying it ourselves
		std::string node_name = "NPC Root [Root]";
		auto node = find_node(actor, node_name, false);
		float allScale = 1.0f;
		if (node) {
			// Grab the world scale which includes all effects from root
			// to here (the lowest scalable node)
			allScale = node->world.scale;

			float worldScale = 1.0f;
			auto rootnode = actor->Get3D(false);
			if (rootnode) {
				auto worldNode = rootnode->parent;

				if (worldNode) {
					worldScale = worldNode->world.scale;

					allScale /= worldScale; // Remove effects of a scaled world
					                        // never actually seen a seen a scaled world
					                        // but here it is just in case
				}
			}
		}
		return allScale / ourScale;
	}

	void ResetToInitScale(Actor* actor) {
		if (actor) {
			if (actor->Is3DLoaded()) {
				InitialScales data;
				if (GetActorInitialScales(actor, data)) {
					set_model_scale(actor, data.model);
					set_npcnode_scale(actor, data.npc);
					logger::trace("Actor: {:08} ResetToInitScale", actor->formID);
				}
			}
		}
	}

	float GetInitialScale(Actor* actor) {

		if (actor) {
			InitialScales data;
			if (GetActorInitialScales(actor, data)) {
				return data.model;
			}
		}
		return 1.0f;
	}

	void RefreshInitialScales(Actor* actor) {
		if (actor) {
			std::string name = std::format("UpdateRace_{}", actor->formID);
			ActorHandle gianthandle = actor->CreateRefHandle();
			TaskManager::RunOnce(name, [=](auto& progressData) { // Reset it one frame later, called by SwitchRaceHook only, inside Hooks/RaceMenu.cpp 
				if (gianthandle) {
					Actor* giantref = gianthandle.get().get();
					if (giantref) {
						InitialScales data;
						if (GetActorInitialScales(giantref, data)) {
							data.model = giantref->GetScale();
							SetActorInitialScales(giantref, data);
						}
					}
				}
			});
		}
	}

	//No longer used
	void set_ref_scale(Actor* actor, float target_scale) {
		// This is how the game sets scale with the `SetScale` command
		// It is limited to x10 and messes up all sorts of things like actor damage
		// and anim speeds
		// Calling too fast also kills frames
		float refScale = static_cast<float>(actor->GetReferenceRuntimeData().refScale) / 100.0F;
		if (fabs(refScale - target_scale) > 1e-5) {
			actor->GetReferenceRuntimeData().refScale = static_cast<std::uint16_t>(target_scale * 100.0F);
			actor->DoReset3D(false);
		}
	}

	bool set_model_scale(Actor* actor, float target_scale) {
		// This will set the scale of the model root (not the root npc node)
		if (!actor->Is3DLoaded()) {
			return false;
		}

		bool result = false;

    	UpdateInitScale(actor); // This will update the inital scales BEFORE we alter them

		auto model = actor->Get3D(false);
		if (model) {
			result = true;
			model->local.scale = target_scale;
			update_node(model);
		}

		auto first_model = actor->Get3D(true);
		if (first_model) {
			result = true;
			first_model->local.scale = target_scale;
			update_node(first_model);
		}
		
		return result;
	}

	bool set_npcnode_scale(Actor* actor, float target_scale) {
		// This will set the scale of the root npc node
		std::string node_name = "NPC Root [Root]";
		bool result = false;

    	UpdateInitScale(actor); // This will update the inital scales BEFORE we alter them

		auto node = find_node(actor, node_name, false);
		if (node) {
			result = true;
			node->local.scale = target_scale;
			update_node(node);
		}

		auto first_node = find_node(actor, node_name, true);
		if (first_node) {
			result = true;
			first_node->local.scale = target_scale;
			update_node(first_node);
		}
		return result;
	}

	float get_npcnode_scale(Actor* actor) {
		// This will get the scale of the root npc node
		std::string node_name = "NPC Root [Root]";
		auto node = find_node(actor, node_name, false);
		if (node) {
			return node->local.scale;
		}
		auto first_node = find_node(actor, node_name, true);
		if (first_node) {
			return first_node->local.scale;
		}
		return 1.0f;
	}

	float get_npcparentnode_scale(Actor* actor) {
		GTS_PROFILE_SCOPE("Modscale: GetNPCParentScale");
		// This will get the scale of the root npc node
		// this is also called the race scale, since it is
		// the racemenu scale
		//
		// The name of it is variable. For actors it is NPC
		// but for others it is the creature name
		std::string node_name = "NPC Root [Root]";
		auto childNode = find_node(actor, node_name, false);
		if (!childNode) {
			childNode = find_node(actor, node_name, true);
			if (!childNode) {
				return 1.0f;
			}
		}
		auto parent = childNode->parent;
		if (parent) {
			return parent->local.scale;
		}
		return 1.0f;
	}

	float get_model_scale(Actor* actor) {
		// This will set the scale of the root npc node
		if (!actor->Is3DLoaded()) {
			return 1.0f;
		}

		auto model = actor->Get3D(false);
		if (model) {
			return model->local.scale;
		}
		auto first_model = actor->Get3D(true);
		if (first_model) {
			return first_model->local.scale;
		}
		return 1.0f;
	}

	float get_scale(Actor* actor) {
		return get_model_scale(actor);
	}

	float game_getactorscale(Actor* actor) {
		// This function reports same values as GetScale() in the console, so it is a value from SetScale() command
		// Used inside: GTSManager.cpp - apply_height
		//              Scale.cpp   -  get_natural_scale
		// actor->GetBaseHeight(); gets the refscale*racescale*actorBaseScale which is more accurate
		// refscale is the scalemult in the REFR esp record or runtime value SetScale.
		//return actor->GetBaseHeight();
		return static_cast<float>(actor->GetReferenceRuntimeData().refScale) / 100.0F;
	}

	float game_getactorscale(Actor& actor) {
		return game_getactorscale(&actor); 
	}

	float game_get_scale_overrides(Actor* actor) { // Obtain RaceMenu * GetScale values of actor
		GTS_PROFILE_SCOPE("Modscale: GameGetScaleOverrides");
		//log::info("Getting Natural Scale of {}: {}", actor->GetDisplayFullName(), get_natural_scale(actor));
		//log::info("Game Override Result for {}: {}", actor->GetDisplayFullName(), game_getactorscale(actor) * get_natural_scale(actor));
		return get_natural_scale(actor, true);
	}

	float game_get_scale_overrides(Actor& actor) {
		return game_get_scale_overrides(&actor);
	}

	bool update_model_visuals(Actor* actor, float scale) {
		return set_model_scale(actor, scale);
	}
}
