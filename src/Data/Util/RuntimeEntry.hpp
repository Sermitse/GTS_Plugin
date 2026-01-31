#pragma once

namespace RuntimeData {

	template<typename RecordType>
	struct RuntimeEntry {
		const RE::BSFixedString Plugin;
		const RE::FormID ID;

		RecordType* Value = nullptr;

		constexpr std::string AsString() const {
			return std::format("{}|{:X}", Plugin, ID);
		}

		explicit RuntimeEntry(const RE::BSFixedString& a_plugin, RE::FormID a_formID) : Plugin(a_plugin), ID(a_formID) {};
		
		bool Resolve() {

			Value = RE::TESDataHandler::GetSingleton()->LookupForm<RecordType>(ID, Plugin);

			if (Value) {
				return true; 
			}

			Value = nullptr;
			return false;
		}

	};

	template<typename A>
	struct iListable {
		using T = A;
		inline static absl::flat_hash_map<std::string, RuntimeEntry<A>*> List {};

		void Resolve() {
			for (const auto& entry : List) {
				if (!entry.second->Resolve()) {
					logger::warn("Form Lookup Failed for {} on {}|{:X} ({})", typeid(T).name() , entry.second->Plugin, entry.second->ID, entry.first);
				}
				else {
					logger::trace("Form Lookup OK for {} on {}|{:X} -> Ptr: {} ({})", typeid(T).name() , entry.second->Plugin, entry.second->ID, fmt::ptr(entry.second->Value), entry.first);
				}
			}
		}
	};

}