#pragma once

#include "RE/B/bhkSerializable.h"
#include "RE/N/NiAVObject.h"
#include "RE/B/bhkCharacterPointCollector.h"

namespace RE {

	class bhkCharacterRigidBody : public bhkSerializable {
		public:
		inline static constexpr auto RTTI = RTTI_bhkCharacterRigidBody;

		~bhkCharacterRigidBody() override;                                // 00
		void          Unk_2E(void) override;                              // 2E
		void          Unk_2F(void) override;                              // 2F

		bhkRigidBody* rigidBody;
		NiAVObject* unk28;
		bhkCharacterPointCollector ignoredCollisionStartCollector;
	};

	static_assert(offsetof(bhkCharacterRigidBody, ignoredCollisionStartCollector) == 0x30);

}