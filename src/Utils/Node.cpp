
#include "Utils/Node.hpp"

using namespace RE;

namespace {

	void UpdateNodeWorldTransform(RE::NiAVObject* node) {
    	if (!node) {
			return;
		}

		if (node->parent) {
			node->world.rotate = node->local.rotate * node->parent->world.rotate;
			node->world.scale = node->local.scale * node->parent->world.scale;
			node->world.translate = node->parent->world.rotate * (node->local.translate * node->parent->world.scale) + node->parent->world.translate;
		} else {
			node->world = node->local;
		}
	}

	void UpdateTreeTransforms(RE::NiAVObject* node) {

		if (!node) {
			return;
		}

		UpdateNodeWorldTransform(node);

		if (auto niNode = node->AsNode()) {
			for (auto& child : niNode->GetChildren()) {
				if (child) {
					UpdateTreeTransforms(child.get());
				}
			}
		}
	}

	void AttachChildAndUpdate(RE::NiNode* parent, RE::NiAVObject* child) {

		if (!parent || !child) {
			return;
		}

		parent->AttachChild(child, true);

		UpdateNodeWorldTransform(child);
		UpdateTreeTransforms(child);
	}

	void loop_message(NiAVObject* root, std::string_view message) {

		auto owner_data = root->GetUserData();
		if (owner_data) {
			auto name = owner_data->GetDisplayFullName();
			logger::error("{} : Possible Endless Loop on {}", message, name);
		}
	}

	void loop_message(TESObjectREFR* object, std::string_view message) {
		if (object) {
			logger::error("{} : Possible Endless Loop on {}", message, object->GetDisplayFullName());
		}
	}

	void loop_message(Actor* actor, std::string_view message) {
		if (actor) {
			logger::error("{} : Possible Endless Loop on {}", message, actor->GetDisplayFullName());
		}
	}

}

namespace GTS {

	constexpr int loop_threshold = 256;

	void Node_CreateNewNode(Actor* giant, std::string_view name, std::string_view connect_to) {
		if (find_node(giant, name)) {
			return; // Don't re-create it
		}
		NiNode* newNode = NiNode::Create(1);

		// Give it a name
		newNode->name = BSFixedString(name);

		// New Coordinates and such
		newNode->local.translate = NiPoint3(0.0f, 0.0f, 0.0f);
		newNode->local.scale = 1.0f;
		newNode->local.rotate = RE::NiMatrix3();

		// Attach to parent
		NiAVObject* parent = find_node(giant, connect_to);
		if (auto* parentNode = parent->AsNode()) {
			// Note: new node seems to inherit parent rotation
			newNode->local.rotate = parentNode->world.Invert().rotate; // So we compensate rotation, x/y/z adjustment should happen in world coordinates now

			AttachChildAndUpdate(parentNode, newNode);
		}
	}

	NiPoint3 Node_WorldToLocal(NiAVObject* node, const NiPoint3& world_pos) { // Wasn't tested, may not work properly
		if (!node) {
			return NiPoint3();
		}

		const NiMatrix3& rot = node->world.rotate;
		const NiPoint3& trans = node->world.translate;

		return rot.Transpose() * (world_pos - trans);
	}

	NiPoint3 Node_LocalToWorld(NiAVObject* node, const NiPoint3& local_pos) { // Wasn't tested, may not work properly
		if (!node) {
			return NiPoint3();
		}

		const NiMatrix3& rot = node->world.rotate;
		const NiPoint3& trans = node->world.translate;

		return rot * local_pos + trans;
	}

	std::vector<NiAVObject*> GetAllNodes(Actor* actor) {
		if (!actor->Is3DLoaded()) {
			return {};
		}
		auto model = actor->Get3D();

		std::deque<NiAVObject*> queue;
		std::vector<NiAVObject*> nodes = {};
		queue.push_back(model);

		int counter = 0;

		while (!queue.empty()) { 

			counter += 1;

			auto currentnode = queue.front();
			queue.pop_front();
			try {
				if (currentnode) {
					auto ninode = currentnode->AsNode();
					if (ninode) {
						for (auto &child : ninode->GetChildren()) {
							// Bredth first search
							nodes.push_back(child.get());
							// Depth first search
							//queue.push_front(child.get());
						}
					}
					// Do smth
					log::trace("Node {}", currentnode->name);
				}
				else if (counter > loop_threshold) {
					queue.clear();
				}
			}
			catch (const std::overflow_error& e) {
				log::warn("Overflow: {}", e.what());
				return {};
			} // this executes if f() throws std::overflow_error (same type rule)
			catch (const std::runtime_error& e) {
				log::warn("Underflow: {}", e.what());
				return {};
			} // this executes if f() throws std::underflow_error (base class rule)
			catch (const std::exception& e) {
				log::warn("Exception: {}", e.what());
				return {};
			} // this executes if f() throws std::logic_error (base class rule)
			catch (...) {
				log::warn("Exception Other");
				return {};
			}
		}
		return nodes;
	}

	void walk_nodes(Actor* actor) {
		if (!actor->Is3DLoaded()) {
			return;
		}

		auto model = actor->Get3D();
		std::deque<NiAVObject*> queue;
		queue.push_back(model);

		int counter = 0;

		while (!queue.empty()) {
			auto currentnode = queue.front();
			queue.pop_front();

			counter += 1;

			try {
				if (currentnode) {
					auto ninode = currentnode->AsNode();
					if (ninode) {
						for (auto &child : ninode->GetChildren()) {
							// Bredth first search
							queue.push_back(child.get());
							// Depth first search
							//queue.push_front(child.get());
						}
					}
					// Do smth
					log::trace("Node {}", currentnode->name);
				} else if (counter > loop_threshold) {
					queue.clear();
				}
			}
			catch (const std::overflow_error& e) {
				log::warn("Overflow: {}", e.what());
				return;
			} // this executes if f() throws std::overflow_error (same type rule)
			catch (const std::runtime_error& e) {
				log::warn("Underflow: {}", e.what());
				return;
			} // this executes if f() throws std::underflow_error (base class rule)
			catch (const std::exception& e) {
				log::warn("Exception: {}", e.what());
				return;
			} // this executes if f() throws std::logic_error (base class rule)
			catch (...) {
				log::warn("Exception Other");
				return;
			}
		}
	}

	NiAVObject* find_node(Actor* actor, std::string_view node_name, bool first_person) {
		if (!actor->Is3DLoaded()) {
			return nullptr;
		}
		auto model = actor->Get3D(first_person);
		if (!model) {
			return nullptr;
		}
		auto node_lookup = model->GetObjectByName(node_name);
		if (node_lookup) {
			return node_lookup;
		}

		// Game lookup failed we try and find it manually
		std::deque<NiAVObject*> queue;
		queue.push_back(model);

        int counter = 0;

		while (!queue.empty()) {
			auto currentnode = queue.front();
			queue.pop_front();

			counter += 1;
			try {
				if (currentnode) {
					auto ninode = currentnode->AsNode();
					if (ninode) {
						for (auto &child : ninode->GetChildren()) {
							// Bredth first search
							if (child) {
								queue.push_back(child.get());
							}
							// Depth first search
							//queue.push_front(child.get());
						}
					} 
					// Do smth
					if (currentnode->name == node_name) {
						log::trace("Found bone: {}", node_name);
						return currentnode;
					}

					if (counter > loop_threshold) {
						//log::info("Counter {} on {} is > than size {}", counter, actor->GetDisplayFullName(), queue.size());
						queue.clear();
						return nullptr;
					}
				}
			}
			catch (const std::overflow_error& e) {
				log::warn("Overflow: {}", e.what());
				return nullptr;
			} // this executes if f() throws std::overflow_error (same type rule)
			catch (const std::runtime_error& e) {
				log::warn("Underflow: {}", e.what());
				return nullptr;
			} // this executes if f() throws std::underflow_error (base class rule)
			catch (const std::exception& e) {
				log::warn("Exception: {}", e.what());
				return nullptr;
			} // this executes if f() throws std::logic_error (base class rule)
			catch (...) {
				log::warn("Exception Other");
				return nullptr;
				}
			}

		return nullptr;
	}


	NiAVObject* find_object_node(TESObjectREFR* object, std::string_view node_name) { // Used inside Looting.cpp only so far
		auto model = object->GetCurrent3D();
		if (!model) {
			return nullptr;
		}
		auto node_lookup = model->GetObjectByName(node_name);
		if (node_lookup) {
			return node_lookup;
		}

		// Game lookup failed we try and find it manually
		std::deque<NiAVObject*> queue;
		queue.push_back(model);
		
		int counter = 0;

		while (!queue.empty()) {
			auto currentnode = queue.front();
			queue.pop_front();

			counter += 1;

			try {
				if (currentnode) {
					auto ninode = currentnode->AsNode();
					if (ninode) {
						for (auto &child : ninode->GetChildren()) {
							// Bredth first search
							if (child) {
								queue.push_back(child.get());
							}
							// Depth first search
							//queue.push_front(child.get());
						}
					}
					// Do smth
					if (currentnode->name == node_name) {
						return currentnode;
					} else if (counter > loop_threshold) {
						queue.clear();
						return nullptr;
					}
				}
			}
			catch (const std::overflow_error& e) {
				log::warn("Overflow: {}", e.what());
				return nullptr;
			} // this executes if f() throws std::overflow_error (same type rule)
			catch (const std::runtime_error& e) {
				log::warn("Underflow: {}", e.what());
				return nullptr;
			} // this executes if f() throws std::underflow_error (base class rule)
			catch (const std::exception& e) {
				log::warn("Exception: {}", e.what());
				return nullptr;
			} // this executes if f() throws std::logic_error (base class rule)
			catch (...) {
				log::warn("Exception Other");
				return nullptr;
			}
		}

		return nullptr;
	}

	NiAVObject* find_node_regex(Actor* actor, const std::string& node_regex, bool first_person)
	{
		if (!actor->Is3DLoaded()) {
			return nullptr;
		}

		auto model = actor->Get3D(first_person);
		if (!model) {
			return nullptr;
		}

		const re2::RE2 the_regex(node_regex);

		std::deque<NiAVObject*> queue;
		queue.push_back(model);

		int counter = 0;

		while (!queue.empty()) {
			auto currentnode = queue.front();
			queue.pop_front();

			++counter;

			if (!currentnode) {
				continue;
			}

			if (const auto ninode = currentnode->AsNode()) {
				for (auto& child : ninode->GetChildren()) {
					if (child) {
						queue.push_back(child.get());
					}
				}
			}
			
			if (RE2::FullMatch(std::string(currentnode->name), the_regex)) {
				return currentnode;
			}

			if (counter > loop_threshold) {
				return nullptr;
			}
		}

		return nullptr;
	}

	NiAVObject* find_node_any(Actor* actor, std::string_view node_name) {
		NiAVObject* result = nullptr;
		for (auto person: {false, true}) {
			result = find_node(actor, node_name, person);
			if (result) {
				break;
			}
		}
		return result;
	}

	NiAVObject* find_node_regex_any(Actor* actor, const std::string& node_regex) {
		NiAVObject* result = nullptr;
		for (auto person: {false, true}) {
			result = find_node_regex(actor, node_regex, person);
			if (result) {
				break;
			}
		}
		return result;
	}

	void scale_hkpnodes(Actor* actor, float prev_scale, float new_scale) { // Unused
		if (!actor->Is3DLoaded()) {
			return;
		}
		auto model = actor->Get3D();
		if (!model) {
			return;
		}
		// Game lookup failed we try and find it manually
		std::deque<NiAVObject*> queue;
		queue.push_back(model);

		int counter = 0;

		while (!queue.empty()) {
			auto currentnode = queue.front();
			queue.pop_front();

			counter += 1;

			try {
				if (currentnode) {
					auto ninode = currentnode->AsNode();
					if (ninode) {
						for (auto &child : ninode->GetChildren()) {
							// Bredth first search
							if (child) {
								queue.push_back(child.get());
							}
							// Depth first search
							//queue.push_front(child.get());
						}
					}
					// Do smth
					auto collision_object = currentnode->GetCollisionObject();
					if (collision_object) {
						auto bhk_rigid_body = collision_object->GetRigidBody();
						if (bhk_rigid_body) {
							hkReferencedObject* hkp_rigidbody_ref = bhk_rigid_body->referencedObject.get();
							if (hkp_rigidbody_ref) {
								hkpRigidBody* hkp_rigidbody = skyrim_cast<hkpRigidBody*>(hkp_rigidbody_ref);
								if (hkp_rigidbody) {
									auto shape = hkp_rigidbody->GetShape();
									if (shape) {
										log::trace("Shape found: {} for {}", typeid(*shape).name(), currentnode->name.c_str());
										if (shape->type == hkpShapeType::kCapsule) {
											const hkpCapsuleShape* orig_capsule = skyrim_cast<const hkpCapsuleShape*>(shape);
											hkTransform identity;
											identity.rotation.col0 = hkVector4(1.0f,0.0f,0.0f,0.0f);
											identity.rotation.col1 = hkVector4(0.0f,1.0f,0.0f,0.0f);
											identity.rotation.col2 = hkVector4(0.0f,0.0f,1.0f,0.0f);
											identity.translation   = hkVector4(0.0f,0.0f,0.0f,1.0f);
											hkAabb out;
											orig_capsule->GetAabbImpl(identity, 1e-3f, out);
											float min[4] = {0.0f};
											float max[4] = {0.0f};
											_mm_store_ps(&min[0], out.min.quad);
											_mm_store_ps(&max[0], out.max.quad);
											log::trace(" - Current bounds: {},{},{}<{},{},{}", min[0], min[1],min[2], max[0],max[1],max[2]);
											// Here be dragons
											hkpCapsuleShape* capsule = const_cast<hkpCapsuleShape*>(orig_capsule);
											log::trace("  - Capsule found: {}", typeid(*orig_capsule).name());
											float scale_factor = new_scale / prev_scale;
											hkVector4 vec_scale = hkVector4(scale_factor);
											capsule->vertexA = capsule->vertexA * vec_scale;
											capsule->vertexB = capsule->vertexB * vec_scale;
											capsule->radius *= scale_factor;

											capsule->GetAabbImpl(identity, 1e-3f, out);
											_mm_store_ps(&min[0], out.min.quad);
											_mm_store_ps(&max[0], out.max.quad);

											log::trace(" - New bounds: {},{},{}<{},{},{}", min[0], min[1],min[2], max[0],max[1],max[2]);
											log::trace(" - pad28: {}", orig_capsule->pad28);
											log::trace(" - pad2C: {}", orig_capsule->pad2C);
											log::trace(" - float(pad28): {}", static_cast<float>(orig_capsule->pad28));
											log::trace(" - float(pad2C): {}", static_cast<float>(orig_capsule->pad2C));

											hkp_rigidbody->SetShape(capsule);
										}
									}
								}
							}
						}
					}
				}

				if (counter > loop_threshold) {
					queue.clear();
					return;
				}
			}
			catch (const std::overflow_error& e) {
				log::warn("Overflow: {}", e.what());
				return;
			} // this executes if f() throws std::overflow_error (same type rule)
			catch (const std::runtime_error& e) {
				log::warn("Underflow: {}", e.what());
				return;
			} // this executes if f() throws std::underflow_error (base class rule)
			catch (const std::exception& e) {
				log::warn("Exception: {}", e.what());
				return;
			} // this executes if f() throws std::logic_error (base class rule)
			catch (...) {
				log::warn("Exception Other");
				return;
			}
		}

		return;
	}

	void clone_bound(Actor* actor) {
		// This is the bound on the NiExtraNodeData
		// This data is shared between all skeletons and this hopes to correct this
		auto model = actor->Get3D();
		if (model) {
			auto extra_bbx = model->GetExtraData("BBX");
			if (extra_bbx) {
				BSBound* bbx = skyrim_cast<BSBound*>(extra_bbx);
				model->RemoveExtraData("BBX");
				auto new_extra_bbx = NiExtraData::Create<BSBound>();
				new_extra_bbx->name = bbx->name;
				new_extra_bbx->center = bbx->center;
				new_extra_bbx->extents = bbx->extents;
				//model->AddExtraData("BBX",  new_extra_bbx);
				model->InsertExtraData(new_extra_bbx);
			}
		}
	}

	BSBound* get_bound(Actor* actor) {
		// This is the bound on the NiExtraNodeData
		if (!actor) {
			return nullptr;
		}
		auto model = actor->Get3D(false);
		if (model) {
			auto extra_bbx = model->GetExtraData("BBX");
			if (extra_bbx) {
				BSBound* bbx = static_cast<BSBound*>(extra_bbx);
				return bbx;
			}
		}
		auto model_first = actor->Get3D(true);
		if (model_first) {
			auto extra_bbx = model_first->GetExtraData("BBX");
			if (extra_bbx) {
				BSBound* bbx = static_cast<BSBound*>(extra_bbx);
				return bbx;
			}
		}
		return nullptr;
	}

	NiPoint3 get_bound_values(Actor* actor) {
		NiPoint3 result = NiPoint3(22.0f, 14.0f, 64.0f); // Default human scale that we return if actor for some reason doesn't have BBX data
		if (actor) {
			auto bound = get_bound(actor);
			if (bound) {
				result = bound->extents;
			}
		}
		return result;
	}

	NiAVObject* get_bumper(Actor* actor) {
		string node_name = "CharacterBumper";
		return find_node(actor, node_name);
	}

	void update_node(NiAVObject* node) {
		if (node) {
			if (State::OnMainThread()) {
				NiUpdateData ctx;
				node->UpdateWorldData(&ctx);
			} else {
				node->IncRefCount();
				auto task = SKSE::GetTaskInterface();
				task->AddTask([node]() {
					if (node) {
						NiUpdateData ctx;
						node->UpdateWorldData(&ctx);
						node->DecRefCount();
					}
				});
			}
		}
	}

	std::vector<NiAVObject*> GetModelsForSlot(Actor* actor, BGSBipedObjectForm::BipedObjectSlot slot) {
		
		enum {
			k3rd,
			k1st,
			kTotal
		};

		std::vector<NiAVObject*> result = {};
		if (actor) {
			auto armo = actor->GetWornArmor(slot);
			if (armo) {
				auto arma = armo->GetArmorAddonByMask(actor->GetRace(), slot);
				if (arma) {
					char addonString[MAX_PATH]{ '\0' };
					arma->GetNodeName(addonString, actor, armo, -1);
					for (auto first: {true, false}) {
						auto node = find_node(actor, addonString, first);
						if (node) {
							result.push_back(node);
						}
					}
				}
			}
		}
		return result;
	}

	void VisitNodes(NiAVObject* root, const std::function<bool(NiAVObject& a_obj)>& a_visitor) {
		std::deque<NiAVObject*> queue;
		queue.push_back(root);

		int counter = 0;

		while (!queue.empty()) {
			auto currentnode = queue.front();
			queue.pop_front();

			counter += 1;

			if (currentnode) {
				auto ninode = currentnode->AsNode();
				if (ninode) {
					for (auto &child : ninode->GetChildren()) {
						// Bredth first search
						if (child) {
							queue.push_back(child.get());
						}
						// Depth first search
						//queue.push_front(child.get());
					}
				}
				if (counter > loop_threshold) {
					queue.clear();
					return;
				}
				if (!a_visitor(*currentnode)) {
					return;
				}
			}
		}
	}
}
