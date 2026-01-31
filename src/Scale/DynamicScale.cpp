#include "Scale/DynamicScale.hpp"
#include "Systems/Rays/Raycast.hpp"


namespace GTS {

	float GetCeilingHeight(Actor* giant) {

		if (!giant) {
			return std::numeric_limits<float>::infinity();
		}

		auto charCont = giant->GetCharController();
		if (!charCont) {
			return std::numeric_limits<float>::infinity();
		}

		auto root_node = giant->GetCurrent3D();
		if (!root_node) {
			return std::numeric_limits<float>::infinity();
		}

		bool debug = DebugDraw::CanDraw();

		float scale = get_visual_scale(giant);
		// === Calculation of ray directions ===
		auto transform = root_node->world;
		transform.scale = 1.0f;
		// ray 1 center on giant + 70 (default), +100 now
		auto ray1_p = NiPoint3(0.0f, 0.0f, 100.0f); // in local space
		ray1_p = transform * ray1_p; // in global space
		// straight up
		auto ray1_d = NiPoint3(0.0f, 0.0f, 1.0f); // direction


		// List of ray positions and directions for the ceiling
		// Don't add a down here, down is made automatically as -dir
		std::vector<std::pair<NiPoint3, NiPoint3> > rays = {
			{ray1_p, ray1_d},
		};
	
		int sides = 6;
		float degrees = 380.0f / sides;
		float rads = degrees * std::numbers::pi_v<float> / 180.0f;
		constexpr float BASE_DIST = 18.0f;
		constexpr float LEVEL_SEP = 12.0f;
		constexpr int LEVELS = 3;
		
		for (int i=0; i<sides; i++) {
			for (int j=0; j < LEVELS; j++) {

				auto mat = NiMatrix3(0.0f, 0.0f, rads * i);
				auto vert = mat * NiPoint3(0.0f, BASE_DIST + LEVEL_SEP*j, 0.0f);
				vert = transform.rotate * (vert * scale);
				vert = ray1_p + vert;

				// Test ray
				constexpr bool DO_TESTRAY = true;
				if (DO_TESTRAY) {
					float TESTRAY_LENGTH = LEVEL_SEP * scale;
					auto ray_start = vert;
					auto ray_dir = transform.rotate * (mat * NiPoint3(0.0f, 1.0f, 0.0f));
					if (debug) {
						NiPoint3 ray_end = vert + ray_dir*TESTRAY_LENGTH;
						DebugDraw::DrawSphere(glm::vec3(ray_start.x, ray_start.y, ray_start.z), 8.0f, 10, {1.0f, 1.0f, 0.0f, 1.0f});
						DebugDraw::DrawLineForMS(glm::vec3(ray_start.x, ray_start.y, ray_start.z), glm::vec3(ray_end.x, ray_end.y, ray_end.z), 10, {1.0f, 0.0f, 1.0f, 1.0f});
					}
					bool success = false;
					NiPoint3 testPos = CastRayStatics(giant, ray_start, ray_dir, TESTRAY_LENGTH, success);
					if (success) {
						if (debug) {
							DebugDraw::DrawSphere(glm::vec3(testPos.x, testPos.y, testPos.z), 5.0f, 30, {1.0f, 0.0f, 0.0f, 1.0f});
						}
						break; // Don't do later levels either
					}
				}
				rays.emplace_back(vert,NiPoint3(0.0f, 0.0f, 1.0f));
			}
		}

		float RAY_LENGTH = 200.f * scale;
		

		// Ceiling
		std::vector<float> ceiling_heights = {};
		//log::info("Casting ceiling rays");
		for (const auto& ray: rays) {
			NiPoint3 ray_start = ray.first;
			NiPoint3 ray_dir = ray.second;
			if (debug) {
				NiPoint3 ray_end = ray_start + ray_dir*RAY_LENGTH;
				DebugDraw::DrawSphere(glm::vec3(ray_start.x, ray_start.y, ray_start.z), 8.0f, 10, {0.0f, 1.0f, 0.0f, 1.0f});
				DebugDraw::DrawLineForMS(glm::vec3(ray_start.x, ray_start.y, ray_start.z), glm::vec3(ray_end.x, ray_end.y, ray_end.z), 10, {1.0f, 0.0f, 0.0f, 1.0f});
			}
			bool success = false;
			NiPoint3 endpos_up = CastRayStatics(giant, ray_start, ray_dir, RAY_LENGTH, success);
			if (success) {
				if (debug) {
					DebugDraw::DrawSphere(glm::vec3(endpos_up.x, endpos_up.y, endpos_up.z), 5.0f, 30, {1.0f, 0.0f, 0.0f, 1.0f});
				}
				ceiling_heights.push_back(endpos_up.z);
			}
		}

		if (ceiling_heights.empty()) {
			return std::numeric_limits<float>::infinity();
		}
		float ceiling = *std::ranges::min_element(ceiling_heights);

		// Floor
		std::vector<float>  floor_heights = {};
		for (const auto& ray: rays) {
			NiPoint3 ray_start = ray.first;
			NiPoint3 ray_dir = ray.second * -1.0f;
			if (debug) {
				NiPoint3 ray_end = ray_start + ray_dir*RAY_LENGTH;
				DebugDraw::DrawSphere(glm::vec3(ray_start.x, ray_start.y, ray_start.z), 8.0f, 10, {0.0f, 1.0f, 1.0f, 1.0f});
				DebugDraw::DrawLineForMS(glm::vec3(ray_start.x, ray_start.y, ray_start.z), glm::vec3(ray_end.x, ray_end.y, ray_end.z), 10, {1.0f, 0.0f, 1.0f, 1.0f});
			}
			bool success = false;
			NiPoint3 endpos_up = CastRayStatics(giant, ray_start, ray_dir, RAY_LENGTH, success);
			if (success) {
				if (debug) {
					DebugDraw::DrawSphere(glm::vec3(endpos_up.x, endpos_up.y, endpos_up.z), 5.0f, 30, {1.0f, 0.0f, 1.0f, 1.0f});
				}
				floor_heights.push_back(endpos_up.z);
			}
		}

		if (floor_heights.empty()) {
			return std::numeric_limits<float>::infinity();
		}
		float floor = *std::ranges::max_element(floor_heights);

		// Room height
		float room_height = fabs(ceiling - floor);
		float room_height_m = GameUnitToMeter(room_height);

		return room_height_m;
	}

	float GetMaxRoomScale(Actor* giant) {
		float stateScale = GetRoomStateScale(giant);

		float room_height_m = GetCeilingHeight(giant);

		// Spring
		auto& dynamicData = DynamicScale::GetData(giant);
		dynamicData.roomHeight.halflife = 0.85f;
		if (!std::isinf(room_height_m)) {
			// Under roof
			if (std::isinf(dynamicData.roomHeight.target)) {
				// Last check was infinity so we just went under a roof
				// Snap current value to new roof
				dynamicData.roomHeight.value = room_height_m;
				dynamicData.roomHeight.velocity = 0.0f;
			}

			dynamicData.roomHeight.target = room_height_m;
			room_height_m = dynamicData.roomHeight.value;
		} else {
			// No roof, set roomHeight to infinity so we know that we left the roof
			// then continue as normal
			if (!std::isinf(dynamicData.roomHeight.target)) {
				dynamicData.roomHeight.target = room_height_m;
				dynamicData.roomHeight.value = room_height_m;
				dynamicData.roomHeight.velocity = 0.0f;
			}
		}

		float room_height_s = room_height_m/Characters_AssumedCharSize; // / height by default character height
		float max_scale = (room_height_s * 0.78f) / stateScale; // Define max scale, make avalibale space seem bigger when prone etc

		return max_scale;
	}

	DynamicScaleData::DynamicScaleData() : roomHeight(Spring(std::numeric_limits<float>::infinity(), 1.0f)){}

	std::string DynamicScale::DebugName() {
		return "::DynamicScale";
	}

	DynamicScaleData& DynamicScale::GetData(Actor* actor) {
		if (!actor) {
			throw std::exception("DynamicScale::GetData: Actor must exist");
		}
		auto id = actor->formID;

		auto& manager = DynamicScale::GetSingleton();
		manager.data.try_emplace(id);

		try {
			return manager.data.at(id);
		}
		catch (const std::out_of_range&) {
			throw std::exception("DynamicScale::GetData: Unable to find actor data");
		}
	}
}
