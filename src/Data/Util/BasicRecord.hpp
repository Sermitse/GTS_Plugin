#pragma once

namespace Serialization {

	template<typename T>
	bool CheckFloat(T& value) {
		if constexpr (std::is_floating_point_v<T>) {
			if (std::isnan(value)) {
				value = 0;
				return true;
			}
		}
		return false;
	}

	// Function to convert uint32_t to a string of 4 characters
	static std::string Uint32ToStr(uint32_t value) {
		char bytes[4];
		bytes[3] = static_cast<char>((value >> 24) & 0xFF);
		bytes[2] = static_cast<char>((value >> 16) & 0xFF);
		bytes[1] = static_cast<char>((value >> 8) & 0xFF);
		bytes[0] = static_cast<char>(value & 0xFF);
		return std::string(bytes, 4);
	}

	template <typename T, const uint32_t uid, const uint32_t ver = 1>
	struct BasicRecord {
		T value;
		static constexpr auto ID = std::byteswap(uid);

		BasicRecord() = default;
		BasicRecord(const T& val) : value(val) {}

		void Load(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version, uint32_t size) {

			if (type == ID) {
				logger::trace("{}: Is being Read", Uint32ToStr(ID));
				if (version == ver && size == sizeof(T)) {
					if (serializationInterface->ReadRecordData(&value, sizeof(T))) {
						logger::trace("{}: Read OK!", Uint32ToStr(ID));
						if (CheckFloat(value)) {
							logger::warn("{}: Was NaN!", Uint32ToStr(ID));
						}
						return;
					}
				}
				logger::error("{}: Could not be loaded!", Uint32ToStr(ID));
			}
		}

		void Save(SKSE::SerializationInterface* serializationInterface) const {
			logger::trace("{}: Is being saved!", Uint32ToStr(ID));
			if (serializationInterface->OpenRecord(ID, ver)) {
				if (serializationInterface->WriteRecordData(&value, sizeof(T))) {
					logger::trace("{}: Save OK!", Uint32ToStr(ID));
					return;
				}
			}

			logger::error("{}: Could not be saved", Uint32ToStr(ID));
		}

	};

    template <const uint32_t uid, const uint32_t ver = 1>
    struct StringRecord {
        std::string value;
        static constexpr auto ID = std::byteswap(uid);

        StringRecord() = default;
        StringRecord(const std::string& val) : value(val) {}

        void Load(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version, uint32_t size) {
            if (type == ID) {
                logger::trace("{}: String is being Read", Uint32ToStr(ID));
                if (version == ver) {
                    // First read the string length
                    uint64_t stringLength = 0;
                    if (serializationInterface->ReadRecordData(&stringLength, sizeof(uint64_t))) {
                        // Create a buffer to hold the string data
                        std::vector<char> buffer(stringLength);

                        // Read the string data
                        if (stringLength > 0 && serializationInterface->ReadRecordData(buffer.data(), stringLength)) {
                            value.assign(buffer.data(), stringLength);
                            logger::trace("{}: String Read OK! Length: {}", Uint32ToStr(ID), stringLength);
                            return;
                        }
                    }
                }
                logger::error("{}: String could not be loaded!", Uint32ToStr(ID));
            }
        }

        void Save(SKSE::SerializationInterface* serializationInterface) const {

			if (value.length() == 0) {
				logger::trace("{}: Nothing To Save Length is 0", Uint32ToStr(ID), value.length());
				return;
			}

            logger::trace("{}: String is being saved! Length: {}", Uint32ToStr(ID), value.length());
            if (serializationInterface->OpenRecord(ID, ver)) {
                // Write the string length first
                uint64_t stringLength = static_cast<uint64_t>(value.length());
                if (serializationInterface->WriteRecordData(&stringLength, sizeof(uint64_t))) {
                    // Then write the string data if there is any
                    if (serializationInterface->WriteRecordData(value.data(), stringLength)) {
                        logger::trace("{}: String Save OK!", Uint32ToStr(ID));
                        return;
                    }
                }
            }
            logger::error("{}: String could not be saved", Uint32ToStr(ID));
        }
    };
}