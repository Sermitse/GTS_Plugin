#include "Hooks/World/WorldMapActor.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	namespace {

		using SceneList = RE::BSTArray<RE::NiPointer<RE::NiAVObject>>;

		const RE::TESWorldSpace* RootWorldSpace(const RE::TESWorldSpace* a_world) {
			while (a_world && a_world->parentWorld) {
				a_world = a_world->parentWorld;
			}
			return a_world;
		}

		//Null when the map is not open.
		const RE::TESWorldSpace* MapWorldSpace() {
			RE::UI* ui = RE::UI::GetSingleton();
			if (!ui || !ui->IsMenuOpen(RE::MapMenu::MENU_NAME)) {
				return nullptr;
			}
			const RE::GPtr<RE::MapMenu> menu = ui->GetMenu<RE::MapMenu>();
			return menu ? menu->GetRuntimeData2().worldSpace : nullptr;
		}

		//Interior 3D sits in its own coordinates, and a map browsing another worldspace has no use for it.
		RE::NiAVObject* MapRoot(RE::Actor* a_actor, const RE::TESWorldSpace* a_mapWorld) {
			const RE::TESObjectCELL* cell = a_actor ? a_actor->GetParentCell() : nullptr;
			if (!cell || !cell->IsExteriorCell() || RootWorldSpace(a_actor->GetWorldspace()) != RootWorldSpace(a_mapWorld)) {
				return nullptr;
			}
			return a_actor->Get3D(false);
		}

		void AddActors(SceneList& a_list) {
			const RE::TESWorldSpace* mapWorld = MapWorldSpace();
			if (!mapWorld) {
				return;
			}

			//Add more actors here. The engine releases the refs when it clears the lists at the end of Main::Draw.
			if (RE::NiAVObject* root = MapRoot(RE::PlayerCharacter::GetSingleton(), mapWorld)) {
				a_list.push_back(RE::NiPointer<RE::NiAVObject>(root));
			}
		}

		//JobList::Finish in Main::Draw that waits for DrawWorld_BuildSceneLists. The cull jobs are queued after it.
		struct SceneListsBuilt {

			static void thunk(void* a_jobList) {
				func(a_jobList);
				static REL::Relocation<SceneList**> jobLists{ RelocationIDEx(528072, 415017, 415017, NULL).address() };
				if (SceneList* first = *jobLists) {
					AddActors(*first);
				}
			}

			FUNCTYPE_CALL func;
		};
	}

	void Hook_WorldSpace::Install() {
		Hooks::stl::write_call<SceneListsBuilt>(RelocationIDEx(35560, 36559, 36559, NULL), OffsetEx(0x17E, 0x17E, 0x17E, NULL));
	}
}