#pragma once
#include "UI/ImGui/Lib/imgui.h"

#include <lunasvg.h>
#include <wincodec.h>
#include <wrl/client.h>

namespace ImageList {

    PSString PlaceHolder               = "placeholder";
    PSString Dummy                     = "dummy";              //Doesn't exist. meant as a fallback to the missing texture pattern.

    PSString Keybind_ShowAdvanced     = "keybind_showadv";
    PSString Keybind_EditKeybind      = "keybind_dokeyrebind";

    PSString Infocard_Spectate        = "infocard_spectate";
    PSString Infocard_Kills           = "infocard_kills";
    PSString Infocard_ExtraInfo       = "infocard_extra";

    PSString Export_Cleanup           = "export_cleanup";
    PSString Export_Delete            = "export_delete";
    PSString Export_Load              = "export_load";
    PSString Export_Save              = "export_save";

    PSString Generic_OK               = "generic_tick";
    PSString Generic_X                = "generic_x";
    PSString Generic_Reset            = "generic_reset";
    PSString Generic_Square           = "generic_square";

    PSString BuffIcon_VoreStacks         = PlaceHolder;
    PSString BuffIcon_DamageReduction    = PlaceHolder;
    PSString BuffIcon_GTSAspect          = PlaceHolder;
    PSString BuffIcon_LifeAbsorbStacks   = PlaceHolder;
    PSString BuffIcon_OnTheEdge          = PlaceHolder;
    PSString BuffIcon_SizeReserve        = PlaceHolder;







}

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

        enum class CutoffDirection {
            None,
            LeftToRight,
            RightToLeft,
            TopToBottom,
            BottomToTop
        };

        struct AffineTransform {
            float rotation = 0.0f;               // Rotation in radians
            ImVec2 scale = { 1.0f, 1.0f };       // Scale factors
            ImVec2 translation = { 0.0f, 0.0f }; // Translation offset
            bool flipHorizontal = false;
            bool flipVertical = false;
        };

        struct ImageTransform {

            bool recolorEnabled = false;
            ImVec4 targetColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA
            AffineTransform affine;
            CutoffDirection cutoffDir = CutoffDirection::None;
            float cutoffPercent = 1.0f; // 0.0 to 1.0

            // Helper to check if any transform is active
            bool IsActive() const {
                return recolorEnabled ||
                    affine.rotation != 0.0f ||
                    affine.scale.x != 1.0f || affine.scale.y != 1.0f ||
                    affine.translation.x != 0.0f || affine.translation.y != 0.0f ||
                    affine.flipHorizontal || affine.flipVertical ||
                    cutoffDir != CutoffDirection::None;
            }

            // Equality operator for caching
            bool operator==(const ImageTransform& other) const {
                return recolorEnabled == other.recolorEnabled &&
                    targetColor.x == other.targetColor.x &&
                    targetColor.y == other.targetColor.y &&
                    targetColor.z == other.targetColor.z &&
                    targetColor.w == other.targetColor.w &&
                    affine.rotation == other.affine.rotation &&
                    affine.scale.x == other.affine.scale.x &&
                    affine.scale.y == other.affine.scale.y &&
                    affine.translation.x == other.affine.translation.x &&
                    affine.translation.y == other.affine.translation.y &&
                    affine.flipHorizontal == other.affine.flipHorizontal &&
                    affine.flipVertical == other.affine.flipVertical &&
                    cutoffDir == other.cutoffDir &&
                    cutoffPercent == other.cutoffPercent;
            }

            bool operator!=(const ImageTransform& other) const {
                return !(*this == other);
            }

            // Add operator< so std::map can use this struct as a key
            bool operator<(const ImageTransform& other) const {
                return
            		std::tie(
                        recolorEnabled, 
                        targetColor.x, targetColor.y, targetColor.z, targetColor.w, 
                        affine.rotation, 
                        affine.scale.x, affine.scale.y, 
                        affine.translation.x, affine.translation.y,
						affine.flipHorizontal, affine.flipVertical,
						cutoffDir, cutoffPercent
                    )
                    <
                    std::tie(
                        other.recolorEnabled, 
                        other.targetColor.x, other.targetColor.y, other.targetColor.z, other.targetColor.w,
                        other.affine.rotation, 
                        other.affine.scale.x, other.affine.scale.y, 
                        other.affine.translation.x, other.affine.translation.y,
                        other.affine.flipHorizontal, other.affine.flipVertical,
                        other.cutoffDir, other.cutoffPercent
                    );
            }
        };

        struct Texture {

            ImVec2 size;
            BaseImageType type = BaseImageType::Unknown;

            ImVec2 originalSize;                         // Original SVG dimensions
            std::unique_ptr<lunasvg::Document> document; // SVG Data
            std::string filePath;                        // Path to img file

            // CPU-side pixel cache (BGRA format)
            std::vector<uint32_t> pixelData;
            std::vector<uint32_t> pixelDataOrig;
            UINT pixelWidth = 0;
            UINT pixelHeight = 0;

            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> oldTextureToRelease;
            std::map<ImageTransform, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> transformedTextureCache;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

            ~Texture() {}
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
        static bool Render(const std::string& a_name, ImVec2 a_size = ImVec2(0, 0));
        static bool RenderTransformed(const std::string& a_name, const ImageTransform& transform, ImVec2 a_size = ImVec2(0, 0));
        static Texture* GetTexture(const std::string& a_name, ImVec2 a_requestedSize = ImVec2(0, 0));
        static std::tuple<ImTextureID, ImVec2> GetAsImGuiTexture(const std::string& a_name, ImVec2 a_size = { 0,0 });
        static std::tuple<ImTextureID, ImVec2> GetAsImGuiTextureTransformed(const std::string& a_name, const ImageTransform& transform, ImVec2 a_size = { 0, 0 });
        static bool HasSvg(const std::string& a_name);
        static std::vector<std::string> GetLoadedSvgNames();
        static void ClearTextureMap();
        static void ClearTransformedTextureCache();
        static void DebugDrawTest();

        private:
        static inline std::atomic_bool m_ready = false;
        static bool RasterizeSVG(Texture* a_svgTexture, ImVec2 a_size);
        static bool CreateDefaultCheckerboardTexture(UINT tileSize = 4, UINT tiles = 8);
        static void CreateSamplers();
        static bool ResampleRaster(Texture* tex, ImVec2 size);
        static void SwapBaseTexture(Texture* texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&& newTexture);
        static bool CreateTextureFromWICBitmap(Texture* texture, const BYTE* pixelData, UINT width, UINT height, UINT stride);
        static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ApplyTransformations(Texture* texture, const ImageTransform& transform);
        static void ApplyRecolor(std::vector<uint32_t>& pixels, UINT width, UINT height, const ImVec4& color);
        static void ApplyAffineTransform(std::vector<uint32_t>& pixels, UINT& width, UINT& height, const AffineTransform& affine);
        static void ApplyCutoff(std::vector<uint32_t>& pixels, UINT width, UINT height, CutoffDirection direction, float percent);
        static bool CreateTextureFromPixels(ID3D11ShaderResourceView** outTexture, const std::vector<uint32_t>& pixels, UINT width, UINT height);
    };

}