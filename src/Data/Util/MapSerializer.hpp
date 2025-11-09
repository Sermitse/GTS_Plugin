#pragma once
#include "Data/Util/TLVSerializer.hpp"

namespace Serialization {

    template<typename Key, typename Value>
    class MapSerializer {

		public:
        static_assert(std::is_integral_v<Key>, "Key must be integral");
        static_assert(std::is_standard_layout_v<Value>, "Value must be POD/reflectable");

        static std::vector<uint8_t> Serialize(const std::unordered_map<Key, Value>& map, uint32_t version) {
            std::vector<uint8_t> out;
            append_le(out, version);
            append_le(out, static_cast<uint32_t>(map.size()));

            for (auto& [key, value] : map) {
                append_le(out, static_cast<Key>(key));

                auto bytes = TLVSerializer::Serialize(value);
                append_le(out, static_cast<uint32_t>(bytes.size()));
                out.insert(out.end(), bytes.begin(), bytes.end());
            }
            return out;
        }

        static void Deserialize(std::unordered_map<Key, Value>& map, std::span<const uint8_t> data, uint32_t& out_version) {
            size_t i = 0;
            if (data.size() < 8) throw std::runtime_error("corrupt header");

            out_version = read_le<uint32_t>(data.data() + i); i += 4;
            uint32_t count = read_le<uint32_t>(data.data() + i); i += 4;

            for (uint32_t n = 0; n < count; ++n) {
                if (i + sizeof(Key) + 4 > data.size()) throw std::runtime_error("corrupt map entry header");

                Key key = read_le<Key>(data.data() + i); i += sizeof(Key);
                uint32_t len = read_le<uint32_t>(data.data() + i); i += 4;

                if (i + len > data.size()) throw std::runtime_error("corrupt map entry payload");

                Value val{};
                TLVSerializer::Deserialize(val, std::span(data.data() + i, len));
                i += len;

                map[key] = std::move(val);
                //map.emplace(key, std::move(val));
            }
        }

		private:
        template<typename U> static void append_le(std::vector<uint8_t>& out, U v) {
            for (size_t b = 0; b < sizeof(U); ++b)
                out.push_back(static_cast<uint8_t>((v >> (8 * b)) & 0xFF));
        }
        template<typename U> static U read_le(const uint8_t* p) {
            U v = 0;
            for (size_t b = 0; b < sizeof(U); ++b)
                v |= (U(p[b]) << (8 * b));
            return v;
        }
    };

}
