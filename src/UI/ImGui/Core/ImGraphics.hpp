#pragma once
#include "UI/ImGui/Lib/imgui.h"

#include <lunasvg.h>
#include <wincodec.h>
#include <wrl/client.h>

namespace GTS {

    class ImGraphics {

        public:

        static inline const std::string _path = R"(Data\SKSE\Plugins\GTSPlugin\Icons\)";
        static constexpr std::string_view _svg = ".svg";
        static constexpr std::string_view _png = ".png";

        enum class BaseImageType {
            Vector,
            Raster,
            Unknown
        };

        struct Texture {

            ID3D11ShaderResourceView* texture = nullptr;
            ImVec2 size;
            BaseImageType type = BaseImageType::Unknown;

            ImVec2 originalSize;                         // Original SVG dimensions
            std::unique_ptr<lunasvg::Document> document; // SVG Data
            std::string filePath;                        // Path to img file

            ~Texture() {
                if (texture) {
                    texture->Release();
                    texture = nullptr;
                }
            }
        };

        private:
        static inline ID3D11Device* m_Device = nullptr;
        static inline ID3D11DeviceContext* m_Context = nullptr;
        static inline ID3D11SamplerState* m_PointSampler = nullptr;
        static inline ID3D11SamplerState* m_LinearSampler = nullptr;

        static inline std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureMap;
        static inline std::mutex m_Lock;
        static inline Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
        static inline std::shared_ptr<Texture> m_defaultTexture;

        public:
        static void Init(ID3D11Device* a_device, ID3D11DeviceContext* a_context);
        static void Load();
        static bool LoadSVG(const std::string& a_name, const std::string& a_path);
        static bool LoadImage(const std::string& name, const std::string& filePath);
        static Texture* GetTexture(const std::string& a_name, ImVec2 a_requestedSize = ImVec2(0, 0));
        static bool Render(const std::string& a_name, ImVec2 a_size = ImVec2(0, 0));
        static void DebugDrawTest();
        static std::tuple<ImTextureID, ImVec2> GetAsImGuiTexture(const std::string& a_name, ImVec2 a_size = { 0,0 });
        static void ClearCache();
        static bool HasSvg(const std::string& a_name);
        static std::vector<std::string> GetLoadedSvgNames();

		private:
        static inline std::atomic_bool m_ready = false;
        static bool RasterizeSVG(Texture* a_svgTexture, ImVec2 a_size);
        static bool CreateTextureFromWICBitmap(Texture* texture, const BYTE* pixelData, UINT width, UINT height, UINT stride);
        static bool CreateDefaultCheckerboardTexture(UINT tileSize = 4, UINT tiles = 8);
        static void CreateSamplers();
        static bool ResampleRaster(Texture* tex, ImVec2 size);
    };

}