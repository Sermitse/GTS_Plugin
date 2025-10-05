#pragma once
#include "UI/ImGui/Lib/imgui.h"

#include <lunasvg.h>
#include <wincodec.h>
#include <wrl/client.h>

namespace GTS {

    class ImGraphics {

        public:

        const std::string _path = R"(Data\SKSE\Plugins\GTSPlugin\Icons\)";
        const std::string _svg = ".svg";
        const std::string _png = ".png";

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
        ID3D11Device* m_Device = nullptr;
        ID3D11DeviceContext* m_Context = nullptr;
        ID3D11SamplerState* m_PointSampler = nullptr;
        ID3D11SamplerState* m_LinearSampler = nullptr;

        std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureMap;
        std::mutex m_Lock;
        Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
        std::shared_ptr<Texture> m_defaultTexture;

        public:
        ImGraphics(ID3D11Device* a_device, ID3D11DeviceContext* a_context);
        ~ImGraphics();
        void Load();
        bool LoadSVG(const std::string& a_name, const std::string& a_path);
        bool LoadImage(const std::string& name, const std::string& filePath);
        Texture* GetTexture(const std::string& a_name, ImVec2 a_requestedSize = ImVec2(0, 0));
        bool Render(const std::string& a_name, ImVec2 a_size = ImVec2(0, 0));
        void DebugDrawTest();
        std::tuple<ImTextureID, ImVec2> GetAsImGuiTexture(const std::string& a_name, ImVec2 a_size = { 0,0 });
        void ClearCache();
        bool HasSvg(const std::string& a_name);
        std::vector<std::string> GetLoadedSvgNames();

		private:
        bool RasterizeSVG(Texture* a_svgTexture, ImVec2 a_size) const;
        bool CreateTextureFromWICBitmap(Texture* texture, const BYTE* pixelData, UINT width, UINT height, UINT stride) const;
        bool CreateDefaultCheckerboardTexture(UINT tileSize = 4, UINT tiles = 8);
        void CreateSamplers();
        bool ResampleRaster(Texture* tex, ImVec2 size) const;
    };

}