#pragma once

namespace GTS {

	float GetCeilingHeight(Actor* giant);
	float GetMaxRoomScale(Actor* giant);

	struct DynamicScaleData {
		DynamicScaleData();
		Spring roomHeight;
	};

	class DynamicScale : public EventListener, public CInitSingleton<DynamicScale> {
		public:
		virtual std::string DebugName() override;
		static DynamicScaleData& GetData(Actor* actor);
		std::unordered_map<FormID, DynamicScaleData> data;
	};
}
