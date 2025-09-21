#pragma once
#include "Utils/MessageboxUtil.hpp"

/*
    TOML11 Reflection Macro & Templates. Based on https://github.com/ToruNiina/toml11/blob/main/examples/reflect/reflect.hpp
*/

namespace toml::refl {

	template<typename T, typename TC>
	T from(const basic_value<TC>& a_v) {
		T x;
		reflect::for_each([&a_v, &x](auto I) {
			using member_type = std::remove_cvref_t<decltype(reflect::get<I>(x))>;
			const auto key = std::string(reflect::member_name<I>(x));
			//Changed to find_or. If the value is not found return the struct's value.
			reflect::get<I>(x) = toml::find_or<member_type>(a_v, key,reflect::get<I>(x));
		}, x);
		return x;
	}

	template<typename TC = toml::ordered_type_config, typename T>
	basic_value<TC> into(const T& a_x) {
		basic_value<TC> v(toml::ordered_table{});
		reflect::for_each([&v, &a_x](auto I) {
			using member_type = std::remove_cvref_t<decltype(reflect::get<I>(a_x))>;
			const auto key = std::string(reflect::member_name<I>(a_x));
			v[key] = reflect::get<I>(a_x);
		}, a_x);
		return v;
	}

	template <typename T>
	constexpr std::string_view GetFriendlyName(const T&) {
		return reflect::type_name<T>();
	}
}

#define TOML_SERIALIZABLE(X)                               \
namespace toml {                                           \
    template<>                                             \
    struct into<X>                                         \
    {                                                      \
        template<typename TC = toml::ordered_type_config>  \
        static toml::basic_value<TC> into_toml(const X& x) \
        {                                                  \
            return refl::into(x);                          \
        }                                                  \
    };                                                     \
    template<>                                             \
    struct from<X>                                         \
    {                                                      \
        template<typename TC = toml::ordered_type_config>  \
        static X from_toml(const toml::basic_value<TC>& v) \
        {                                                  \
            return refl::from<X>(v);                       \
        }                                                  \
    };                                                     \
} /* toml */

#define TOML_REGISTER_NAME(TYPE, NAME)                                               \
namespace toml {                                                                     \
    template <>                                                                      \
		constexpr std::string_view toml::refl::GetFriendlyName<TYPE>(const TYPE&) {  \
			return NAME;                                                             \
	}                                                                                \
}                                                                                    \
