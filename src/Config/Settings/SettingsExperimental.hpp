#pragma once
#include "Config/Util/TomlRefl.hpp"

/*
Reflection based serializer limitations:

 - C style arrays are unsupported so no "char foo[32]" for example
 - std::array arrays or std::vectors however do work with basic types (aggregate types are untested)
 - Data types like std::tuple or ordered map are unsupported (others are untested)
 - Nested structs are supported as long as these also are put through the TOML_SERIALIZABLE() macro.
 - Structs can only contain a total of 64 unique entries, this is a limitation of the reflect library.
   In order to be able to have > 64 the visit template in the reflect library needs to be expanded.
 - Enums are also unsupported. They can however be saved as either int or string. Its better to save them as a string though
   and use something like magic_enum to do the conversion between string <-> enum.
*/

/* Naming Convention
* i(x) -> integer(Ammount of elements if array)
* f(x) -> float(Ammount of elements if array)
* b(x) -> bool(Ammount of elements if array)
* s(x) -> string(Ammount of elements if array)
* stucts don't get a special notation
*/

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsExperimental_t {
	
	float fTest1 = 1.0f;
	float fTest2 = 1.0f;
	float fTest3 = 1.0f;
	float fTest4 = 1.0f;
	float fTest5 = 1.0f;
	float fTest6 = 1.0f;
	float fTest7 = 1.0f;
	float fTest8 = 1.0f;

	int32_t iTest1 = 0;
	int32_t iTest2 = 0;
	int32_t iTest3 = 0;
	int32_t iTest4 = 0;
	int32_t iTest5 = 0;
	int32_t iTest6 = 0;
	int32_t iTest7 = 0;
	int32_t iTest8 = 0;

};
TOML_SERIALIZABLE(SettingsExperimental_t);
TOML_REGISTER_NAME(SettingsExperimental_t, "Experiments");
