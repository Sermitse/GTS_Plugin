

#include "UI/ImGui/Lib/imgui.h"

namespace ImUtil::Colors {


    std::array<float, 3> HSVtoRGB(const std::array<float, 3>& hsv) {
        float H = hsv[0], S = hsv[1], V = hsv[2];
        float r = 0, g = 0, b = 0;

        int i = static_cast<int>(H * 6.0f);
        float f = H * 6.0f - i;
        float p = V * (1.0f - S);
        float q = V * (1.0f - f * S);
        float t = V * (1.0f - (1.0f - f) * S);

        switch (i % 6) {
	        case 0: r = V; g = t; b = p; break;
	        case 1: r = q; g = V; b = p; break;
	        case 2: r = p; g = V; b = t; break;
	        case 3: r = p; g = q; b = V; break;
	        case 4: r = t; g = p; b = V; break;
	        case 5: r = V; g = p; b = q; break;
        }
        return { r,g,b };
    }

    std::array<float, 3> RGBtoHSV(const std::array<float, 3>& rgb) {
        float r = rgb[0], g = rgb[1], b = rgb[2];
        float maxC = std::max({ r,g,b });
        float minC = std::min({ r,g,b });
        float delta = maxC - minC;

        float H = 0.0f, S = 0.0f, V = maxC;

        if (delta > 1e-6f) {
            S = delta / maxC;
            if (maxC == r)      H = (g - b) / delta;
            else if (maxC == g) H = 2.0f + (b - r) / delta;
            else                H = 4.0f + (r - g) / delta;

            H /= 6.0f;
            if (H < 0.0f) H += 1.0f;
        }
        return { H, S, V };
    }

    std::array<float, 3> ShiftHue(const std::array<float, 3>& hsv, float hueShift) {
        __m128 hsv_vec = _mm_set_ps(0.0f, hsv[2], hsv[1], hsv[0]); // [x, V, S, H]
        __m128 shift_vec = _mm_set_ps(0.0f, 0.0f, 0.0f, hueShift);
        __m128 shifted = _mm_add_ps(hsv_vec, shift_vec);

        // Wrap hue
        float H = fmod(_mm_cvtss_f32(shifted), 1.0f);
        if (H < 0.0f) H += 1.0f;

        // Saturation and value stay the same
        float S = _mm_cvtss_f32(_mm_shuffle_ps(shifted, shifted, _MM_SHUFFLE(1, 1, 1, 1)));
        float V = _mm_cvtss_f32(_mm_shuffle_ps(shifted, shifted, _MM_SHUFFLE(2, 2, 2, 2)));

        return HSVtoRGB({ H, S, V });
    }

    ImVec4 AdjustAlpha(const ImVec4& color, float alpha) {
        return { color.x, color.y, color.z, alpha };
    }

    ImVec4 AdjustLightness(const ImVec4& color, float factor) {
        float r = color.x;
        float g = color.y;
        float b = color.z;

        float maxC = std::max({ r, g, b });
        float minC = std::min({ r, g, b });
        float l = (maxC + minC) * 0.5f;

        if (maxC == minC) {
            // achromatic (gray) color
            float newL = std::clamp(l * factor, 0.0f, 1.0f);
            return { newL, newL, newL, color.w };
        }

        float delta = maxC - minC;
        float s = (l > 0.5f) ? delta / (2.0f - maxC - minC) : delta / (maxC + minC);

        float h;
        if       (maxC == r) h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
        else if  (maxC == g) h = (b - r) / delta + 2.0f;
        else     h = (r - g) / delta + 4.0f;
        h /= 6.0f;

        l = std::clamp(l * factor, 0.0f, 1.0f);
        float q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;

        auto hue2rgb = [](float a_p, float a_q, float t) -> float {
            if (t < 0.0f) t += 1.0f;
            if (t > 1.0f) t -= 1.0f;
            if (t < 1.0f / 6.0f) return a_p + (a_q - a_p) * 6.0f * t;
            if (t < 1.0f / 2.0f) return a_q;
            if (t < 2.0f / 3.0f) return a_p + (a_q - a_p) * (2.0f / 3.0f - t) * 6.0f;
            return a_p;
        };

        float r_out = hue2rgb(p, q, h + 1.0f / 3.0f);
        float g_out = hue2rgb(p, q, h);
        float b_out = hue2rgb(p, q, h - 1.0f / 3.0f);

        return { r_out, g_out, b_out, color.w };
    }

    ImVec4 CalculateContrastColor(const ImVec4& background) {
        __m128 v = _mm_set_ps(0.0f, background.z, background.y, background.x);
        __m128 w = _mm_set_ps(0.0f, 0.0722f, 0.7152f, 0.2126f);
        __m128 mul = _mm_mul_ps(v, w);
        float luminance = mul.m128_f32[0] + mul.m128_f32[1] + mul.m128_f32[2];
        float mask = luminance > 0.5f ? 0.0f : 1.0f;
        return { mask, mask, mask, 1.0f };
    }

    ImVec4 RGBAToImVec4(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept {
        constexpr float scale = 1.0f / 255.0f;
        return ImVec4(r * scale, g * scale, b * scale, a * scale);
    }

    ImU32 fRGBToU32(std::array<float, 3> a_rgb) {

        auto clamp = [](float v) -> int {
            int iv = static_cast<int>(v * 255.0f);
            if (iv < 0) iv = 0;
            if (iv > 255) iv = 255;
            return iv;
        };

        int r = clamp(a_rgb[0]);
        int g = clamp(a_rgb[1]);
        int b = clamp(a_rgb[2]);

        return IM_COL32(r, g, b, 255); // full alpha
    }

    ImU32 fRGBAToU32(std::array<float, 4> a_rgba) {

        auto clamp = [](float v) -> int {
            int iv = static_cast<int>(v * 255.0f);
            if (iv < 0) iv = 0;
            if (iv > 255) iv = 255;
            return iv;
        };

        int r = clamp(a_rgba[0]);
        int g = clamp(a_rgba[1]);
        int b = clamp(a_rgba[2]);
        int a = clamp(a_rgba[3]);

        return IM_COL32(r, g, b, a); // full alpha
    }


    ImVec4 fRGBToImVec4(std::array<float, 3> a_rgb) {
        return { a_rgb[0], a_rgb[1], a_rgb[2], 1.0f }; // full alpha
    }

    std::array<float, 3> RGBToLinear(float r, float g, float b) {
        alignas(16) float rgb[4] = { r, g, b, 0.0f };
        __m128 v = _mm_load_ps(rgb);
        __m128 result = _mm_pow_ps(v, _mm_set1_ps(2.2f));
        alignas(16) float out[4];
        _mm_store_ps(out, result);
        return { out[0], out[1], out[2] };
    }

    std::array<float, 3> LinearToRGB(float r, float g, float b) {
        alignas(16) float rgb[4] = { r, g, b, 0.0f };
        __m128 v = _mm_load_ps(rgb);
        __m128 result = _mm_pow_ps(v, _mm_set1_ps(1.0f / 2.2f));
        alignas(16) float out[4];
        _mm_store_ps(out, result);
        return { out[0], out[1], out[2] };
    }

    std::array<float, 3> LinearToOKLab(float r, float g, float b) {
        float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
        float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
        float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

        // Vectorized cbrt approximation via pow
        alignas(16) float LMS[4] = { l, m, s, 0.0f };
        __m128 v = _mm_load_ps(LMS);
        __m128 res = _mm_pow_ps(v, _mm_set1_ps(1.0f / 3.0f));
        alignas(16) float cube[4];
        _mm_store_ps(cube, res);

        l = cube[0]; m = cube[1]; s = cube[2];

        return {
            0.2104542553f * l + 0.7936177850f * m - 0.0040720468f * s,
            1.9779984951f * l - 2.4285922050f * m + 0.4505937099f * s,
            0.0259040371f * l + 0.7827717662f * m - 0.8086757660f * s
        };
    }

    std::array<float, 3> OKLabToLinear(float L, float a, float b) {
        float l_ = L + 0.3963377774f * a + 0.2158037573f * b;
        float m_ = L - 0.1055613458f * a - 0.0638541728f * b;
        float s_ = L - 0.0894841775f * a - 1.2914855480f * b;

        alignas(16) float LMS[4] = { l_, m_, s_, 0.0f };
        __m128 v = _mm_load_ps(LMS);
        __m128 v3 = _mm_mul_ps(_mm_mul_ps(v, v), v); // cube
        alignas(16) float cubed[4];
        _mm_store_ps(cubed, v3);

        l_ = cubed[0]; m_ = cubed[1]; s_ = cubed[2];

        return {
            4.0767416621f  * l_ - 3.3077115913f * m_ + 0.2309699292f * s_,
            -1.2684380046f * l_ + 2.6097574011f * m_ - 0.3413193965f * s_,
            -0.0041960863f * l_ - 0.7034186147f * m_ + 1.7076147010f * s_
        };
    }

    ImVec4 CompensateForDarkColors(const ImVec4& c, float minL) {

        auto [rL, gL, bL] = RGBToLinear(c.x, c.y, c.z);
        auto [L, A, B] = LinearToOKLab(rL, gL, bL);

        if (L < minL) {
            float chroma = std::sqrt(A * A + B * B);
            if (chroma > 1e-6f) {
                float scale = (1.0f - minL) / (1.0f - L);
                A *= scale;
                B *= scale;
            }
            L = minL;
        }

        auto [r2, g2, b2] = OKLabToLinear(L, A, B);
        auto [r, g, b] = LinearToRGB(
            std::clamp(r2, 0.0f, 1.0f),
            std::clamp(g2, 0.0f, 1.0f),
            std::clamp(b2, 0.0f, 1.0f)
        );

        return { r, g, b, c.w };
    }

    ImU32 AdjustGrayScaleLightness(float a_grayScale, float a_alpha) {
        return fRGBAToU32({ a_grayScale ,a_grayScale ,a_grayScale, a_alpha });
    }

}
