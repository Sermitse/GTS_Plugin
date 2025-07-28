#include "TES.hpp"
#include "TESObjectCELL.hpp"

namespace RE {

	//Ported Over From PO3's Clib.

	void TESEx::ForEachReferenceEx(const std::function<BSContainer::ForEachResult(TESObjectREFR* a_ref)>& a_callback) {

		const auto& instance = TES::GetSingleton();

		if (instance->interiorCell) {
			TESObjectCELLEx::ForEachReferenceEx(instance->interiorCell, [&](TESObjectREFR* a_ref) {
				return a_callback(a_ref);
			});
		}
		else {
			if (const auto gridLength = instance->gridCells ? instance->gridCells->length : 0; gridLength > 0) {
				std::uint32_t x = 0;
				do {
					std::uint32_t y = 0;
					do {
						if (const auto cell = instance->gridCells->GetCell(x, y); cell && cell->IsAttached()) {
							TESObjectCELLEx::ForEachReferenceEx(cell ,[&](TESObjectREFR* a_ref) {
								return a_callback(a_ref);
							});
						}
						++y;
					}
					while (y < gridLength);
					++x;
				}
				while (x < gridLength);
			}
		}
		if (TESWorldSpace* const ws = GetCurrentWorldSpace()) {
			if (TESObjectCELL* const skyCell = ws ? ws->GetSkyCell() : nullptr; skyCell) {
				TESObjectCELLEx::ForEachReferenceEx(skyCell, [&](TESObjectREFR* a_ref) {
					return a_callback(a_ref);
				});
			}
		}
	}


	void TESEx::ForEachReferenceInRangeEx(TESObjectREFR* a_origin, float a_radius, const std::function<BSContainer::ForEachResult(TESObjectREFR* a_ref)>& a_callback) {

		if (a_origin && a_radius > 0.0f) {

			const auto originPos = a_origin->GetPosition();
			const auto& instance = TES::GetSingleton();

			if (instance->interiorCell) {
				TESObjectCELLEx::ForEachReferenceInRangeEx(instance->interiorCell, originPos, a_radius, [&](TESObjectREFR* a_ref) {
					return a_callback(a_ref);
				});
			}
			else {
				if (const auto gridLength = instance->gridCells ? instance->gridCells->length : 0; gridLength > 0) {
					const float yPlus = originPos.y + a_radius;
					const float yMinus = originPos.y - a_radius;
					const float xPlus = originPos.x + a_radius;
					const float xMinus = originPos.x - a_radius;

					std::uint32_t x = 0;
					do {
						std::uint32_t y = 0;
						do {
							if (const auto cell = instance->gridCells->GetCell(x, y); cell && cell->IsAttached()) {
								if (const auto cellCoords = cell->GetCoordinates(); cellCoords) {
									const NiPoint2 worldPos{ cellCoords->worldX, cellCoords->worldY };
									if (worldPos.x < xPlus && (worldPos.x + 4096.0f) > xMinus && worldPos.y < yPlus && (worldPos.y + 4096.0f) > yMinus) {
										TESObjectCELLEx::ForEachReferenceInRangeEx(cell, originPos, a_radius, [&](TESObjectREFR* a_ref) {
											return a_callback(a_ref);
										});
									}
								}
							}
							++y;
						}
						while (y < gridLength);
						++x;
					}
					while (x < gridLength);
				}
			}

			if (TESWorldSpace* const ws = GetCurrentWorldSpace()) {
				if (TESObjectCELL* const skyCell = ws ? ws->GetSkyCell() : nullptr; skyCell) {
					TESObjectCELLEx::ForEachReferenceInRangeEx(skyCell, originPos, a_radius, [&](TESObjectREFR* a_ref) {
						return a_callback(a_ref);
					});
				}
			}
		}
		else {
			ForEachReferenceEx([&](TESObjectREFR* a_ref) {
				return a_callback(a_ref);
			});
		}
	}
}
