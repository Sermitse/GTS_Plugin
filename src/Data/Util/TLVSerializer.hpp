#pragma once

namespace Serialization {

    // -------------------- compile-time FNV-1a (32-bit) --------------------
    consteval uint32_t fnv1a32_cstr(std::string_view s) noexcept {
        uint32_t h = 0x811c9dc5u;
        for (char c : s) {
            h ^= static_cast<uint8_t>(c);
            h = h * 0x01000193u;
        }
        return h;
    }

    // -------------------- compile-time collision check --------------------
    template<typename T>
    consteval bool check_member_hash_collisions() {
        // Build vector of hashes using reflect::for_each
        std::vector<uint32_t> hashes;

        auto obj = [] {
            if constexpr (std::is_default_constructible_v<T>) {
                return T{};
            }
            else {
                // For non-default constructible types, use aggregate initialization
                return T{};
            }
        }();

        reflect::for_each([&](const auto I) {
            constexpr std::string_view name = reflect::member_name<I, T>();
            constexpr uint32_t tag = fnv1a32_cstr(name);
            hashes.push_back(tag);
        }, obj);

        // Check for duplicates
        for (size_t i = 0; i < hashes.size(); ++i) {
            for (size_t j = i + 1; j < hashes.size(); ++j) {
                if (hashes[i] == hashes[j]) {
                    return false; // Collision detected
                }
            }
        }
        return true;
    }

    // -------------------- type codes for basic types --------------------
    enum : uint8_t {
        TC_INVALID = 0,
        TC_INT8    = 1,
        TC_INT16   = 2,
        TC_INT32   = 3,
        TC_INT64   = 4,
        TC_UINT8   = 5,
        TC_UINT16  = 6,
        TC_UINT32  = 7,
        TC_UINT64  = 8,
        TC_FLOAT   = 9,
        TC_DOUBLE  = 10,
        TC_BOOL    = 11
    };

    template<typename T> struct type_code { static constexpr uint8_t value = TC_INVALID; };

#define TC_MAP(type, code) template<> struct type_code<type> { static constexpr uint8_t value = code; }
    TC_MAP(int8_t, TC_INT8);
    TC_MAP(int16_t, TC_INT16);
    TC_MAP(int32_t, TC_INT32);
    TC_MAP(int64_t, TC_INT64);
    TC_MAP(uint8_t, TC_UINT8);
    TC_MAP(uint16_t, TC_UINT16);
    TC_MAP(uint32_t, TC_UINT32);
    TC_MAP(uint64_t, TC_UINT64);
    TC_MAP(float, TC_FLOAT);
    TC_MAP(double, TC_DOUBLE);
    TC_MAP(bool, TC_BOOL);
#undef TC_MAP

    // helpers
    template<typename T>
    inline constexpr bool supported_basic_v = (type_code<std::remove_cv_t<std::remove_reference_t<T>>>::value != TC_INVALID)
        && std::is_trivially_copyable_v<std::remove_cv_t<std::remove_reference_t<T>>>;

    // -------------------- TLVSerializer --------------------
    class TLVSerializer {

    public:
        // Serialize object to TLV vector
        template<typename T>
        static std::vector<uint8_t> Serialize(const T& obj) {
            static_assert(std::is_standard_layout_v<T> || std::is_aggregate_v<T>, "Only standard layout or aggregate structs supported");
            static_assert(check_member_hash_collisions<T>(), "Hash collision detected between member names in struct T");

            std::vector<uint8_t> out;

            reflect::for_each([&](const auto I) {
                constexpr std::string_view name = reflect::member_name<I, T>();
                constexpr uint32_t tag = fnv1a32_cstr(name);
                using MemberT = decltype(reflect::get<I>(obj));
                static_assert(supported_basic_v<MemberT>, "Member type not supported by TLVSerializer");

                const auto& val_ref = reflect::get<I>(obj);
                uint8_t tcode = type_code<std::remove_cv_t<std::remove_reference_t<MemberT>>>::value;
                uint16_t len = static_cast<uint16_t>(sizeof(MemberT));

                //logger::trace("Serialize member '{}' tag=0x{:08X} type={} size={}", name, tag, tcode, len);

                append_le(out, tag);
                out.push_back(tcode);
                append_le(out, len);
                const uint8_t* p = reinterpret_cast<const uint8_t*>(std::addressof(val_ref));
                out.insert(out.end(), p, p + len);
            }, obj);

            //logger::trace("Serialized total size: {}", out.size());
            return out;
        }

        template<typename T>
        static void Deserialize(T& obj, std::span<const uint8_t> data) {
            static_assert(std::is_standard_layout_v<T> || std::is_aggregate_v<T>, "Only standard layout or aggregate structs supported");
            static_assert(check_member_hash_collisions<T>(), "Hash collision detected between member names in struct T");

            std::unordered_map<uint32_t, std::pair<uint8_t, std::span<const uint8_t>>> map;
            size_t i = 0;
            while (i + 4 + 1 + 2 <= data.size()) {
                uint32_t tag = read_le<uint32_t>(data.data() + i); i += 4;
                uint8_t tcode = data[i++];
                uint16_t len = read_le<uint16_t>(data.data() + i); i += 2;
                if (i + len > data.size()) throw std::runtime_error("Corrupt TLV buffer");

               // logger::trace("Deserialize found TLV: tag=0x{:08X} type={} len={}", tag, tcode, len);

                map.emplace(tag, std::make_pair(tcode, std::span<const uint8_t>(data.data() + i, len)));
                i += len;
            }

            reflect::for_each([&](const auto I) {
                constexpr std::string_view name = reflect::member_name<I, T>();
                constexpr uint32_t tag = fnv1a32_cstr(name);
                using MemberT = decltype(reflect::get<I>(obj));
                if (auto it = map.find(tag); it != map.end()) {
                    uint8_t tcode_expected = type_code<std::remove_cv_t<std::remove_reference_t<MemberT>>>::value;
                    auto [tcode, spanv] = it->second;
                    //logger::trace("Checking member '{}' tag=0x{:08X} expected_type={} found_type={} len={}", name, tag, tcode_expected, tcode, spanv.size());

                    if (tcode == tcode_expected && spanv.size() == sizeof(MemberT)) {
                        auto& member_ref = reflect::get<I>(obj);
                        std::memcpy(std::addressof(member_ref), spanv.data(), spanv.size());
                        //logger::trace("Deserialized member '{}' successfully", name);
                    }
                }
            }, obj);
        }

		private:
        template<typename U>
        static void append_le(std::vector<uint8_t>& out, U value) {
            static_assert(std::is_integral_v<U>, "append_le requires integral");
            for (size_t b = 0; b < sizeof(U); ++b)
                out.push_back(static_cast<uint8_t>((value >> (8 * b)) & 0xFF));
        }

        template<typename U>
        static U read_le(const uint8_t* p) {
            U v = 0;
            for (size_t b = 0; b < sizeof(U); ++b)
                v |= (U(p[b]) << (8 * b));
            return v;
        }
    };
}