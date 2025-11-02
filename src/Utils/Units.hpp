#pragma once

namespace GTS {

    double KiloToPound(double a_kg);
    double KiloToMammoth(double a_kg);

    double MetersToFeet(double a_meter);
    double MetersToMammoth(double a_meter);

    std::string FormatMetricHeight(double a_meter);
    std::string FormatImperialHeight(double a_feet);
    std::string FormatMammothHeight(double a_mammoth);

    std::string FormatMetricWeight(double a_kg);
    std::string FormatImperialWeight(double a_lb);
    std::string FormatMammothWeight(double a_mammoth);

    std::string GetFormatedWeight(RE::Actor* a_Actor);

    std::string GetFormatedHeight(RE::Actor* a_Actor);
    std::string GetFormatedHeight(float Value);

	float GameUnitToMeter(const float& unit);
	float MeterToGameUnit(const float& meter);

	NiPoint3 GameUnitToMeter(const NiPoint3& unit);
	NiPoint3 MeterToGameUnit(const NiPoint3& meter);
}
