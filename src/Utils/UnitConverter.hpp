#pragma once
#include "fmt/format.h"
#include "config/Config.hpp"

namespace GTS {

    //Metric -> Imperial

    inline double KiloToPound(const double a_kg) {
        return a_kg / 0.45359237;
    }

    inline double MetersToFeet(const double a_meter) {
        return a_meter * 3.28084;
    }

    //Metric -> Mammoth (Because memes)

    inline double MetersToMammoth(const double a_meter) {
        return a_meter / 3.04; //<--- Value probably needs tweaking...
    }

    inline double KiloToMammoth(const double a_kg) {
        return a_kg / 5113.01; //<--- Value probably needs tweaking...
    }

    // Formaters

    inline std::string FormatMetricHeight(const double a_meter) {

        if(a_meter < 1.0f)
            return fmt::format("{:.0f} cm",a_meter * 100.f);

        if(a_meter > 500.0f)
            return fmt::format("{:.2f} Km", a_meter / 1000.f);
            
        return fmt::format("{:.2f} m", a_meter);
    }

	inline std::string FormatImperialHeight(const double a_feet) {
        // Get the integer part for feet.
        int feet = static_cast<int>(a_feet);
        // Get the fractional part and convert it to inches.
        double fraction = a_feet - feet;
        int inches = static_cast<int>(std::round(fraction * 12));

        // Handle the case where rounding makes inches equal to 12.
        if (inches == 12) {
            feet += 1;
            inches = 0;
        }

        if (a_feet < 1.0)
            return fmt::format("{}\"", inches);


        if (a_feet > 2000.0)
            return fmt::format("{:.2f} mi", a_feet / 5280);

        return fmt::format("{}'{}\"", feet, inches);
    }

    inline std::string FormatMammothHeight(const double a_mammoth){
        return fmt::format("{:.2f} Mammoths", a_mammoth);
    }

    inline std::string FormatMetricWeight(const double a_kg){
        if(a_kg < 1.0f)
            return fmt::format("{:.0f} g",a_kg * 100.f);

        if(a_kg > 300000.0f)
            return fmt::format("{:.2f} kt", a_kg / 1000000.f);

        if(a_kg > 30000.0f)
            return fmt::format("{:.2f} t", a_kg / 1000.f);


        return fmt::format("{:.2f} kg", a_kg);
    }

    inline std::string FormatImperialWeight(const double a_lb){
        if(a_lb < 1.0)
            return fmt::format("{:.0f} oz", a_lb * 16.0);

        if(a_lb > 2000000.0)
            return fmt::format("{:.2f} kt", a_lb / 2000000.0);

        if(a_lb > 2000.0)
            return fmt::format("{:.2f} t", a_lb / 2000.0);

        return fmt::format("{:.2f} lb", a_lb);
    }

    inline std::string FormatMammothWeight(const double a_mammoth){
        return fmt::format("{:.2f} Mammoths", a_mammoth);
    }

    inline std::string GetFormatedWeight(RE::Actor* a_Actor){
        std::string displayUnits = Config::UI.sDisplayUnits;
        if (displayUnits == "kImperial") 
            return FormatImperialWeight(KiloToPound(GetActorGTSWeight(a_Actor)));

        else if (displayUnits == "kMammoth") 
            return FormatMammothWeight(KiloToMammoth(GetActorGTSWeight(a_Actor)));
        
        else
            return FormatMetricWeight(GetActorGTSWeight(a_Actor));
    }

    inline std::string GetFormatedHeight(RE::Actor* a_Actor){
        std::string displayUnits = Config::UI.sDisplayUnits;
        if (displayUnits == "kImperial") 
            return FormatImperialHeight(MetersToFeet(GetActorGTSHeight(a_Actor)));

        else if (displayUnits == "kMammoth") 
            return FormatMammothHeight(MetersToMammoth(GetActorGTSHeight(a_Actor)));
        
        else
            return FormatMetricHeight(GetActorGTSHeight(a_Actor));
    }

    inline std::string GetFormatedHeight(const float Value) {
        std::string displayUnits = Config::UI.sDisplayUnits;
        if (displayUnits == "kImperial")
            return FormatImperialHeight(MetersToFeet(Value));

        else if (displayUnits == "kMammoth")
            return FormatMammothHeight(MetersToMammoth(Value));

        else
            return FormatMetricHeight(Value);
    }
}