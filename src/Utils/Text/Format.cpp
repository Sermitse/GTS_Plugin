#include "Utils/Text/Format.hpp"

namespace GTS {

	std::string GetRawName(const void* obj_c) {
		// Get the meta entry in vftable
		void* obj = const_cast<void*>(obj_c);
		_RTTICompleteObjectLocator* col = reinterpret_cast<_RTTICompleteObjectLocator***>(obj)[0][-1];

		// Calculate image base by subtracting the RTTICompleteObjectLocator's pSelf offset from RTTICompleteObjectLocator's pointer
		uintptr_t imageBase = reinterpret_cast<uintptr_t>(col) - col->pSelf;

		// Get the type descriptor by adding TypeDescriptor's offset to the image base
		TypeDescriptor* tDesc = reinterpret_cast<TypeDescriptor*>(imageBase + col->pTypeDescriptor);

		// At the end, we can get the type's mangled name
		const char* colName = tDesc->name;
		return colName;
	}

	std::string Vector2Str(const hkVector4& vector) {
		return std::format("{:.2f},{:.2f},{:.2f},{:.2f}", vector.quad.m128_f32[0], vector.quad.m128_f32[1], vector.quad.m128_f32[2], vector.quad.m128_f32[3]);
	}

	std::string Vector2Str(const hkVector4* vector) {
		if (vector) {
			return std::format("{:.2f},{:.2f},{:.2f},{:.2f}", vector->quad.m128_f32[0], vector->quad.m128_f32[1], vector->quad.m128_f32[2], vector->quad.m128_f32[3]);
		}
		return "";
	}

	std::string Vector2Str(const NiPoint3& vector) {
		return std::format("{:.2f},{:.2f},{:.2f}", vector.x, vector.y, vector.z);
	}

	std::string Vector2Str(const NiPoint3* vector) {
		if (vector) {
			return std::format("{:.2f},{:.2f},{:.2f}", vector->x, vector->y, vector->z);
		}
		return "";
	}

	std::string Vector2Str(const NiQuaternion& vector) {
		return std::format("{:.2f},{:.2f},{:.2f},{:.2f}", vector.x, vector.y, vector.z, vector.w);
	}

	std::string Vector2Str(const NiQuaternion* vector) {
		if (vector) {
			return std::format("{:.2f},{:.2f},{:.2f},{:.2f}", vector->x, vector->y, vector->z, vector->w);
		}
		return "";
	}
}


