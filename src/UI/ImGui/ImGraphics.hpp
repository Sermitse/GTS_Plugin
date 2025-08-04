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
        ImGraphics(ID3D11Device* a_device, ID3D11DeviceContext* a_context) : m_Device(a_device), m_Context(a_context) {

            HRESULT hr = CoCreateInstance(
                CLSID_WICImagingFactory,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_wicFactory)
            );

            if (FAILED(hr)) {
                logger::critical("Failed to create WIC Imaging Factory. HRESULT={:X}", hr);
                ReportAndExit("Could not initialize WIC Imaging Factory.\n"
							  "The game will now close."
                );
            }

            if (!CreateDefaultCheckerboardTexture()) {
                logger::critical("Failed to create default checkerboard texture.");
                ReportAndExit("Could not create the default texture.\n"
							  "The game will now close."
                );
               
            }

            CreateSamplers();

        }

        ~ImGraphics() {
            ClearCache();
            if (m_PointSampler) m_PointSampler->Release();
            if (m_LinearSampler) m_LinearSampler->Release();
        }

        // Load all SVGs from a directory
        void Load() {

            try {

                for (const auto& entry : std::filesystem::directory_iterator(_path)) {
                    std::string ext = entry.path().extension().string();
                    ranges::transform(ext, ext.begin(), ::tolower);

                    const std::string path = entry.path().string();
                    std::string name = entry.path().filename().string();

                    // Remove file extension from name
                    size_t lastdot = name.find_last_of('.');
                    if (lastdot != std::string::npos) {
                        name = name.substr(0, lastdot);
                    }

                    if (ext == ".svg") {
                        if (!LoadSVG(name, path)) {
                            logger::error("Could not load SVG file: {}", name);
                        }
                    }

                    else if (ext == ".png"  || ext == ".jpg"  || ext == ".jpeg" || ext == ".bmp") {
                        if (!LoadImage(name, path)) {
                            logger::error("Could not load image file: {}", name);
                        }
                    }
                }
            }
            catch (const std::exception& e) {
                logger::critical("Exception during icon load: {}", e.what());
                ReportAndExit("Something went wrong while trying to load icons.\n"
			                  "Check GTSPlugin.log for more info.\n"
			                  "The game will now close."
                );
            }
        }

        // Load a single SVG file
        bool LoadSVG(const std::string& a_name, const std::string& a_path) {
            std::lock_guard<std::mutex> lock(m_Lock);

            auto document = lunasvg::Document::loadFromFile(a_path);
            if (!document) {
                logger::error("LoadSVG() -> load fail on {}", a_name);
                return false;
            }

            auto texture = std::make_shared<Texture>();
            texture->document = std::move(document);

            // Store original dimensions
            auto bbox = texture->document->boundingBox();
            texture->originalSize = ImVec2(bbox.w, bbox.h);

            // Create initial texture at original size
            if (!RasterizeSVG(texture.get(), texture->originalSize)) {
                logger::error("LoadSVG() -> Rasterize fail on {}", a_name);
                return false;
            }

            m_TextureMap[a_name] = texture;
            return true;
        }

        // Load using WIC
        bool LoadImage(const std::string& name, const std::string& filePath) {
            std::lock_guard<std::mutex> lock(m_Lock);

            // Convert std::string to wstring for WIC
            std::wstring wFilePath(filePath.begin(), filePath.end());

            // Create a WIC decoder
            Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
            HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
                wFilePath.c_str(),
                nullptr,
                GENERIC_READ,
                WICDecodeMetadataCacheOnLoad,
                &decoder
            );

            if (FAILED(hr)) {
                logger::warn("WIC decoder creation failed: {:X}", hr);
                return false;
            }

            // Get the first frame of the image
            Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
            hr = decoder->GetFrame(0, &frame);
            if (FAILED(hr)) {
                logger::warn("WIC frame grab fail: {:X}", hr);
                return false;
            }

            // Get image dimensions
            UINT width, height;
            hr = frame->GetSize(&width, &height);
            if (FAILED(hr)) {
                logger::warn("WIC size grab fail: {:X}", hr);
                return false;
            }

            // Convert to 32bpp BGRA format (required for DirectX)
            Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
            hr = m_wicFactory->CreateFormatConverter(&converter);
            if (FAILED(hr)) {
                logger::warn("WIC format converter create fail: {:X}", hr);
                return false;
            }

            hr = converter->Initialize(
                frame.Get(),
                GUID_WICPixelFormat32bppBGRA,
                WICBitmapDitherTypeNone,
                nullptr,
                0.0f,
                WICBitmapPaletteTypeCustom
            );

            if (FAILED(hr)) {
                logger::warn("WIC format converter init fail: {:X}", hr);
                return false;
            }

            // Create a WIC bitmap to access the pixel data
            Microsoft::WRL::ComPtr<IWICBitmap> wicBitmap;
            hr = m_wicFactory->CreateBitmapFromSource(
                converter.Get(),
                WICBitmapCacheOnLoad,
                &wicBitmap
            );

            if (FAILED(hr)) {
                logger::warn("WIC bitmap create fail: {:X}", hr);
                return false;
            }

            // Lock the bitmap to access the pixel data
            Microsoft::WRL::ComPtr<IWICBitmapLock> bitm_lock;
            WICRect rect = { 0, 0, static_cast<INT>(width), static_cast<INT>(height) };
            hr = wicBitmap->Lock(&rect, WICBitmapLockRead, &bitm_lock);
            if (FAILED(hr)) {
                logger::warn("WIC lock fail: {:X}", hr);
                return false;
            }

            // Get pixel data
            UINT bufferSize = 0;
            BYTE* pData = nullptr;
            UINT stride = 0;

            hr = bitm_lock->GetDataPointer(&bufferSize, &pData);
            if (FAILED(hr)) {
                logger::warn("WIC bad pointer: {:X}", hr);
                return false;
            }

            hr = bitm_lock->GetStride(&stride);
            if (FAILED(hr)) {
                logger::warn("WIC bad stride: {:X}", hr);
                return false;
            }

            // Create texture object
            auto texture = std::make_shared<Texture>();
            texture->type = BaseImageType::Raster;
            texture->originalSize = ImVec2(static_cast<float>(width), static_cast<float>(height));
            texture->filePath = filePath;

            // Create DirectX texture
            bool result = CreateTextureFromWICBitmap(texture.get(), pData, width, height, stride);

            if (result) {
                m_TextureMap[name] = texture;
            }

            return result;
        }


        // Get texture by name
        Texture* GetTexture(const std::string& a_name, ImVec2 a_requestedSize = ImVec2(0, 0)) {
            std::lock_guard<std::mutex> lock(m_Lock);

            auto it = m_TextureMap.find(a_name);
            if (it == m_TextureMap.end()) {
                return m_defaultTexture.get();
            }

            auto& texture = it->second;

            // If requested size is specified and different from current size, re-rasterize
            if (a_requestedSize.x > 0 && a_requestedSize.y > 0 &&
                (std::abs(texture->size.x - a_requestedSize.x) > 1.0f ||
                std::abs(texture->size.y - a_requestedSize.y) > 1.0f)) {
                RasterizeSVG(texture.get(), a_requestedSize);
            }

            return texture.get();
        }

        // Render SVG by name within ImGui
        bool Render(const std::string& a_name, ImVec2 a_size = ImVec2(0, 0)) {
            Texture* texture = GetTexture(a_name, a_size);
            if (!texture || !texture->texture) {
                return false;
            }

            //If default size is less than 32px (caused by bad svg metadata) enforce min size
            if (texture->size.x < 32 || texture->size.y < 32) {
                texture->size.x = 32;
                texture->size.y = 32;
            }

            // If no size specified, use the texture's size
            ImVec2 renderSize = (a_size.x <= 0 || a_size.y <= 0) ? texture->size : a_size;


            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<void*>(texture->texture)), renderSize);
            return true;
        }

        // Render SVG by name within ImGui
        std::tuple<ImTextureID, ImVec2> GetAsImGuiTexture(const std::string& a_name, ImVec2 a_size = { 0,0 }) {
            std::lock_guard<std::mutex> lock(m_Lock);
            auto it = m_TextureMap.find(a_name);
            if (it == m_TextureMap.end())
                return { reinterpret_cast<ImTextureID>(m_defaultTexture->texture), m_defaultTexture->size };

            auto tex = it->second.get();

            // **SVG**: auto re-rasterize when size differs
            if (tex->type == BaseImageType::Vector &&
                a_size.x > 0 && a_size.y > 0 &&
                (fabs(tex->size.x - a_size.x) > 1.0f || fabs(tex->size.y - a_size.y) > 1.0f)) {
                RasterizeSVG(tex, a_size);
            }
            // **Raster**: auto resample via WIC scaler when downsizing
            else if (tex->type == BaseImageType::Raster &&
                a_size.x > 0 && a_size.y > 0 &&
                (fabs(tex->originalSize.x - a_size.x) > 1.0f || fabs(tex->originalSize.y - a_size.y) > 1.0f)) {
                ResampleRaster(tex, a_size);
            }

            ImVec2 renderSize = (a_size.x > 0 && a_size.y > 0) ? a_size : tex->size;
            return { reinterpret_cast<ImTextureID>(tex->texture), renderSize };
        }

        // Clear all textures
        void ClearCache() {
            std::lock_guard<std::mutex> lock(m_Lock);
            m_TextureMap.clear();
        }

        // Check if an SVG is loaded
        bool HasSvg(const std::string& a_name) {
            std::lock_guard<std::mutex> lock(m_Lock);
            return m_TextureMap.contains(a_name);
        }

        // Get all loaded SVG names
        std::vector<std::string> GetLoadedSvgNames() {
            std::lock_guard<std::mutex> lock(m_Lock);
            std::vector<std::string> names;
            names.reserve(m_TextureMap.size());

            for (const auto& pair : m_TextureMap | views::keys) {
                names.push_back(pair);
            }

            return names;
        }

        private:

    	// Create or update a texture from an SVG document
        bool RasterizeSVG(Texture* a_svgTexture, ImVec2 a_size) const {
            if (!a_svgTexture->document) {
                return false;
            }

            // Calculate aspect ratio for proper scaling
            float aspectRatio = a_svgTexture->originalSize.x / a_svgTexture->originalSize.y;

            // If only width or only height is specified, calculate the other dimension
            if (a_size.x <= 0 && a_size.y > 0) {
                a_size.x = a_size.y * aspectRatio;
            }
            else if (a_size.y <= 0 && a_size.x > 0) {
                a_size.y = a_size.x / aspectRatio;
            }
            else if (a_size.x <= 0 && a_size.y <= 0) {
                // Use original size if no dimensions specified
                a_size = a_svgTexture->originalSize;
            }

            // Round to integers to avoid texture scaling artifacts
            a_size.x = std::round(a_size.x);
            a_size.y = std::round(a_size.y);

            // Release old texture if it exists
            if (a_svgTexture->texture) {
                a_svgTexture->texture->Release();
                a_svgTexture->texture = nullptr;
            }

            // Render SVG to bitmap with the requested size
            lunasvg::Bitmap bitmap = a_svgTexture->document->renderToBitmap(
                static_cast<uint32_t>(a_size.x),
                static_cast<uint32_t>(a_size.y)
            );

            if (!bitmap.valid()) {
                return false;
            }

            // Store the new size
            a_svgTexture->size = a_size;

            // Create DirectX texture
            D3D11_TEXTURE2D_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Width = bitmap.width();
            desc.Height = bitmap.height();
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;

            // Create texture with bitmap data
            ID3D11Texture2D* pTexture = nullptr;
            D3D11_SUBRESOURCE_DATA subResource;
            subResource.pSysMem = bitmap.data();
            subResource.SysMemPitch = bitmap.width() * 4; // 4 bytes per pixel (RGBA)
            subResource.SysMemSlicePitch = 0;

            HRESULT hr = m_Device->CreateTexture2D(&desc, &subResource, &pTexture);
            if (FAILED(hr)) {
                return false;
            }

            // Create shader resource view
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            ZeroMemory(&srvDesc, sizeof(srvDesc));
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
            srvDesc.Texture2D.MostDetailedMip = 0;

            hr = m_Device->CreateShaderResourceView(pTexture, &srvDesc, &a_svgTexture->texture);
            pTexture->Release();

            return SUCCEEDED(hr);
        }

        // Create a DirectX texture from WIC bitmap data
        bool CreateTextureFromWICBitmap(Texture* texture, const BYTE* pixelData,
            UINT width, UINT height, UINT stride) const {
            // Release old texture if it exists
            if (texture->texture) {
                texture->texture->Release();
                texture->texture = nullptr;
            }

            // Create DirectX texture
            D3D11_TEXTURE2D_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // WIC uses BGRA format
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;

            // Create texture with pixel data
            ID3D11Texture2D* pTexture = nullptr;
            D3D11_SUBRESOURCE_DATA subResource;
            subResource.pSysMem = pixelData;
            subResource.SysMemPitch = stride;
            subResource.SysMemSlicePitch = 0;

            HRESULT hr = m_Device->CreateTexture2D(&desc, &subResource, &pTexture);
            if (FAILED(hr))
                return false;

            // Create shader resource view
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            ZeroMemory(&srvDesc, sizeof(srvDesc));
            srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
            srvDesc.Texture2D.MostDetailedMip = 0;

            hr = m_Device->CreateShaderResourceView(pTexture, &srvDesc, &texture->texture);
            pTexture->Release();

            // Update texture size
            texture->size = ImVec2(static_cast<float>(width), static_cast<float>(height));

            return SUCCEEDED(hr);
        }

        bool CreateDefaultCheckerboardTexture(UINT tileSize = 4, UINT tiles = 8) {
            UINT width = tileSize * tiles;
            UINT height = tileSize * tiles;

            std::vector<uint32_t> pixelData(width * height);

            for (UINT y = 0; y < height; ++y) {
                for (UINT x = 0; x < width; ++x) {
                    bool isPink = ((x / tileSize) + (y / tileSize)) % 2 == 0;
                    uint32_t color = isPink ? 0xFFFF00FF : 0xFF000000; // ARGB: pink or black
                    pixelData[y * width + x] = color;
                }
            }

            auto texture = std::make_shared<Texture>();
            texture->type = BaseImageType::Raster;
            texture->originalSize = ImVec2(static_cast<float>(width), static_cast<float>(height));

            // Create DirectX texture
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // BGRA
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            D3D11_SUBRESOURCE_DATA subResource = {};
            subResource.pSysMem = pixelData.data();
            subResource.SysMemPitch = width * 4;

            ID3D11Texture2D* pTexture = nullptr;
            HRESULT hr = m_Device->CreateTexture2D(&desc, &subResource, &pTexture);
            if (FAILED(hr))
                return false;

            // Create shader resource view
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;

            hr = m_Device->CreateShaderResourceView(pTexture, &srvDesc, &texture->texture);
            pTexture->Release();

            if (FAILED(hr))
                return false;

            texture->size = texture->originalSize;
            m_defaultTexture = texture;
            return true;
        }

        void CreateSamplers() {

            D3D11_SAMPLER_DESC sd{};
            sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
            sd.MinLOD = 0;
            sd.MaxLOD = D3D11_FLOAT32_MAX;

            HRESULT hr = m_Device->CreateSamplerState(&sd, &m_PointSampler);
            if (FAILED(hr)) {
                logger::error("CreateSamplerState m_PointSampler fail: {:X}", hr);
            }

            sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            hr =  m_Device->CreateSamplerState(&sd, &m_LinearSampler);
            if (FAILED(hr)) {
                logger::error("CreateSamplerState m_LinearSampler fail: {:X}", hr);
            }

            logger::info("Samplers built");
        }

        bool ResampleRaster(Texture* tex, ImVec2 size) const {

            // Convert path to wstring
            std::wstring wpath(tex->filePath.begin(), tex->filePath.end());
            Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
            if (FAILED(m_wicFactory->CreateDecoderFromFilename(wpath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder))) {
                return false;
            }

            Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
            if (FAILED(decoder->GetFrame(0, &frame))) {
                return false;
            }

            Microsoft::WRL::ComPtr<IWICBitmapScaler> scaler;
            if (FAILED(m_wicFactory->CreateBitmapScaler(&scaler))) {
                return false;
            }

            HRESULT hr = scaler->Initialize(frame.Get(), static_cast<UINT>(size.x), static_cast<UINT>(size.y), WICBitmapInterpolationModeNearestNeighbor);
            if (FAILED(hr)) {
                logger::error("scaler->Initialize fail: {:X}", hr);
                return false;
            }

            Microsoft::WRL::ComPtr<IWICFormatConverter> conv;
            hr = m_wicFactory->CreateFormatConverter(&conv);
            if (FAILED(hr)) {
                logger::error("m_wicFactory->CreateFormatConverter fail: {:X}", hr);
                return false;
            }

            hr = conv->Initialize(scaler.Get(), GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom);
            if (FAILED(hr)) {
                logger::error("conv->Initialize fail: {:X}", hr);
                return false;
            }

            Microsoft::WRL::ComPtr<IWICBitmap> bmp;
            hr = m_wicFactory->CreateBitmapFromSource(conv.Get(), WICBitmapCacheOnLoad, &bmp);
            if (FAILED(hr)) {
                logger::error("m_wicFactory->CreateBitmapFromSource fail: {:X}", hr);
                return false;
            }

            Microsoft::WRL::ComPtr<IWICBitmapLock> lockBmp;
            WICRect rc{ 0,0,static_cast<INT>(size.x),static_cast<INT>(size.y) };
            hr = bmp->Lock(&rc, WICBitmapLockRead, &lockBmp);
            if (FAILED(hr)) {
                logger::error("bmp->Lock fail: {:X}", hr);
                return false;
            }

            UINT stride, bufSz; BYTE* data;
            hr = lockBmp->GetStride(&stride);
            if (FAILED(hr)) {
                logger::error("lockBmp->GetStride fail: {:X}", hr);
                return false;
            }

            hr = lockBmp->GetDataPointer(&bufSz, &data);
            if (FAILED(hr)) {
                logger::error("lockBmp->GetDataPointer fail: {:X}", hr);
                return false;
            }

            // Release old texture
            if (tex->texture) {
                tex->texture->Release(); tex->texture = nullptr;
            }

            // Create new D3D texture
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = static_cast<UINT>(size.x); desc.Height = static_cast<UINT>(size.y);
            desc.MipLevels = 1; desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            D3D11_SUBRESOURCE_DATA sub{ data, stride, 0 };
            ID3D11Texture2D* pTex = nullptr;

            if (FAILED(m_Device->CreateTexture2D(&desc, &sub, &pTex))) {
                return false;
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
            srv.Format = desc.Format;
            srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv.Texture2D.MipLevels = 1;

            hr = m_Device->CreateShaderResourceView(pTex, &srv, &tex->texture);
            if (FAILED(hr)) {
                logger::error("m_Device->CreateShaderResourceView fail: {:X}", hr);
                return false;
            }

            pTex->Release();

            tex->size = size;
            return true;
        }

    };

}