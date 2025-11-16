#pragma once 

template <typename Enum>
bool Enum_Contains(Enum value, const std::string& substring) {
    static_assert(magic_enum::detail::is_enum_v<Enum>, "Template argument must be an enum type");
    auto enum_name = std::string{ magic_enum::enum_name(value) };
    return enum_name.find(substring) != std::string::npos;
}

template<typename Enum>
[[nodiscard]] static inline Enum StringToEnum(const std::string& name) {
    auto value = magic_enum::enum_cast<Enum>(name);
    if (value.has_value()) {
        return *value;
    }
    // Instead of throwing, return the first element of the enum
    return magic_enum::enum_values<Enum>()[0];
}