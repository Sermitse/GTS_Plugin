#pragma once

namespace RE {

	class hkpConvexVerticesConnectivity : public hkReferencedObject {
		public:
		inline static constexpr auto RTTI = RTTI_hkpConvexVerticesConnectivity;

		~hkpConvexVerticesConnectivity() override;  // 00
		virtual void clear();

		hkArray<std::uint16_t> vertexIndices;
		hkArray<std::uint8_t> numVerticesPerFace;
	};

}