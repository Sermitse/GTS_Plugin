#include "UI/Core/ImGraphics.hpp"

namespace GTS {

	void ImGraphics::SwapBaseTexture(Texture* texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&& newTexture) {
		if (!texture) return;

		// Move the CURRENT resource to the 'old' buffer.
		// The ComPtr will not release the resource here, only decrement the ref count.
		texture->oldTextureToRelease = std::move(texture->texture);

		// Assign the new resource.
		texture->texture = std::move(newTexture);
	}

	void ImGraphics::Init(ID3D11Device* a_device, ID3D11DeviceContext* a_context) {

		if (m_ready.exchange(true) == true) {
			return;
		}

		m_Device = a_device;
		m_Context = a_context;

		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_wicFactory)
		);

		if (FAILED(hr)) {
			logger::critical("Failed to create WIC Imaging Factory. HRESULT={:X}", hr);
			ReportAndExit("ImGraphics: Could not initialize WIC Imaging Factory.");
		}

		if (!CreateDefaultCheckerboardTexture()) {
			logger::critical("Failed to create default checkerboard texture.");
			ReportAndExit("ImGraphics: Could not create the default checkerboard texture.");

		}

		CreateSamplers();

	}

	// Load all Files from a directory
	void ImGraphics::Load() {

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

				else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
					if (!LoadImage(name, path)) {
						logger::error("Could not load image file: {}", name);
					}
				}
			}
		}
		catch (const std::exception& e) {
			logger::critical("Exception during icon load: {}", e.what());
			ReportAndExit("ImGraphics: Something went wrong while trying to load icons.\n"
				          "Check GTSPlugin.log for more info."
			);
		}
	}

	// Load a single SVG file
	bool ImGraphics::LoadSVG(const std::string& a_name, const std::string& a_path) {
		std::lock_guard lock(m_Lock);

		auto document = lunasvg::Document::loadFromFile(a_path);
		if (!document) {
			logger::error("LoadSVG() -> load fail on {}", a_name);
			return false;
		}

		auto texture = std::make_shared<Texture>();
		texture->document = std::move(document);
		texture->type = BaseImageType::Vector;

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
	bool ImGraphics::LoadImage(const std::string& name, const std::string& filePath) {
		std::lock_guard lock(m_Lock);

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
		texture->pixelWidth = width;
		texture->pixelHeight = height;

		// Cache pixel data in CPU memory
		texture->pixelData.resize(width * height);
		for (UINT y = 0; y < height; ++y) {
			std::memcpy(&texture->pixelData[y * width], pData + y * stride, width * 4);
		}

		//Store Copy
		texture->pixelDataOrig = texture->pixelData;

		// Create DirectX texture
		bool result = CreateTextureFromWICBitmap(texture.get(), pData, width, height, stride);

		if (result) {
			m_TextureMap[name] = texture;
		}

		return result;
	}

	// Get texture by name
	ImGraphics::Texture* ImGraphics::GetTexture(const std::string& a_name, ImVec2 a_requestedSize) {
		std::lock_guard lock(m_Lock);

		auto it = m_TextureMap.find(a_name);
		if (it == m_TextureMap.end()) {
			return m_defaultTexture.get();
		}

		auto& texture = it->second;

		// If requested size is specified and different from current size, re-rasterize
		if (a_requestedSize.x > 0 && a_requestedSize.y > 0 &&
			(std::abs(texture->size.x - a_requestedSize.x) > 1.0f ||
			std::abs(texture->size.y - a_requestedSize.y) > 1.0f)) {
			if (texture->type == BaseImageType::Vector) {
				RasterizeSVG(texture.get(), a_requestedSize);
			}
			else {
				ResampleRaster(texture.get(), a_requestedSize);
			}
		}

		return texture.get();
	}

	// Render Image by name within ImGui
	bool ImGraphics::Render(const std::string& a_name, ImVec2 a_size) {
		Texture* texture = GetTexture(a_name, a_size);
		if (!texture || !texture->texture) {
			return false;
		}

		//If default size is less than 32px (caused by bad svg metadata) enforce min size
		if (texture->size.x < 8 || texture->size.y < 8) {
			texture->size.x = 8;
			texture->size.y = 8;
		}

		// If no size specified, use the texture's size
		ImVec2 renderSize = (a_size.x <= 0 || a_size.y <= 0) ? texture->size : a_size;

		ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<void*>(texture->texture.Get())), renderSize);
		return true;
	}

	bool ImGraphics::RenderTransformed(const std::string& a_name, const ImageTransform& transform, ImVec2 a_size) {
		std::lock_guard lock(m_Lock);

		auto it = m_TextureMap.find(a_name);
		if (it == m_TextureMap.end()) {
			return false; // Not found
		}

		auto& texture = it->second;

		// Resize base texture if needed
		if (a_size.x > 0 && a_size.y > 0 &&
			(std::abs(texture->size.x - a_size.x) > 1.0f ||
			std::abs(texture->size.y - a_size.y) > 1.0f)) {
			if (texture->type == BaseImageType::Vector) {
				RasterizeSVG(texture.get(), a_size);
			}
			else {
				ResampleRaster(texture.get(), a_size);
			}
		}

		ImVec2 renderSize = (a_size.x <= 0 || a_size.y <= 0) ? texture->size : a_size;

		// If no transform is active, render the base texture
		if (!transform.IsActive()) {
			ImGui::Image(reinterpret_cast<ImTextureID>(texture->texture.Get()), renderSize);
			return true;
		}

		//Transform Cache

		// Check if this transform is already cached
		auto cache_it = texture->transformedTextureCache.find(transform);
		if (cache_it != texture->transformedTextureCache.end()) {
			// Found it. Render the cached texture.
			ImGui::Image(reinterpret_cast<ImTextureID>(cache_it->second.Get()), renderSize);
			return true;
		}

		// Not found. We need to create it.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newTransformedTexture = ApplyTransformations(texture.get(), transform);

		if (!newTransformedTexture) {
			return false; // Failed to create
		}

		// Store the new texture in the cache
		texture->transformedTextureCache[transform] = newTransformedTexture;

		// Render the new texture
		ImGui::Image(reinterpret_cast<ImTextureID>(newTransformedTexture.Get()), renderSize);

		return true;
	}

	void ImGraphics::DebugDrawTest() {

		constexpr int split = 12;
		int amtDrawn = 0;

		Render(ImageList::Dummy, { 32, 32 });

		ImGui::SameLine();

		for (const auto& key : m_TextureMap | views::keys) {
			Render(key, { 32,32 });
			ImGui::SameLine();
			amtDrawn++;
			if (amtDrawn % split == 0) {
				ImGui::NewLine();
			}
		}

		ImGui::NewLine();

	}

	std::tuple<ImTextureID, ImVec2> ImGraphics::GetAsImGuiTexture(const std::string& a_name, ImVec2 a_size) {
		std::lock_guard lock(m_Lock);

		auto it = m_TextureMap.find(a_name);
		if (it == m_TextureMap.end())
			return { reinterpret_cast<ImTextureID>(m_defaultTexture->texture.Get()), m_defaultTexture->size };

		auto tex = it->second.get();

		// SVG: auto re-rasterize when size differs
		if (tex->type == BaseImageType::Vector && a_size.x > 0 && a_size.y > 0 &&
			(fabs(tex->size.x - a_size.x) > 1.0f || fabs(tex->size.y - a_size.y) > 1.0f)) {
			RasterizeSVG(tex, a_size);
		}
		// Raster: auto resample via WIC scaler when downsizing
		else if (tex->type == BaseImageType::Raster && a_size.x > 0 && a_size.y > 0 &&
			(fabs(tex->originalSize.x - a_size.x) > 1.0f || fabs(tex->originalSize.y - a_size.y) > 1.0f)) {
			ResampleRaster(tex, a_size);
		}

		ImVec2 renderSize = (a_size.x > 0 && a_size.y > 0) ? a_size : tex->size;
		return { reinterpret_cast<ImTextureID>(tex->texture.Get()), renderSize };
	}

	std::tuple<ImTextureID, ImVec2> ImGraphics::GetAsImGuiTextureTransformed(const std::string& a_name, const ImageTransform& transform, ImVec2 a_size) {
		std::lock_guard lock(m_Lock);

		auto it = m_TextureMap.find(a_name);
		if (it == m_TextureMap.end()) {
			return { reinterpret_cast<ImTextureID>(m_defaultTexture->texture.Get()), m_defaultTexture->size };
		}

		auto tex = it->second.get();

		// Resize if needed
		if (a_size.x > 0 && a_size.y > 0 &&
			(std::abs(tex->size.x - a_size.x) > 1.0f || std::abs(tex->size.y - a_size.y) > 1.0f)) {
			if (tex->type == BaseImageType::Vector) {
				RasterizeSVG(tex, a_size);
			}
			else {
				ResampleRaster(tex, a_size);
			}
		}

		ImVec2 renderSize = (a_size.x > 0 && a_size.y > 0) ? a_size : tex->size;

		// If no transform, return the base texture
		if (!transform.IsActive()) {
			return { reinterpret_cast<ImTextureID>(tex->texture.Get()), renderSize };
		}

		//Texture Cache

		// Check if this transform is already cached
		auto cache_it = tex->transformedTextureCache.find(transform);
		if (cache_it != tex->transformedTextureCache.end()) {
			// Found it. Return the cached texture.
			return { reinterpret_cast<ImTextureID>(cache_it->second.Get()), renderSize };
		}

		// Not found. We need to create it.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newTransformedTexture = ApplyTransformations(tex, transform);

		if (!newTransformedTexture) {
			// Failed to create, return default
			return { reinterpret_cast<ImTextureID>(m_defaultTexture->texture.Get()), m_defaultTexture->size };
		}

		// Store the new texture in the cache
		tex->transformedTextureCache[transform] = newTransformedTexture;

		// Return the new texture
		return { reinterpret_cast<ImTextureID>(newTransformedTexture.Get()), renderSize };
	}

	// Clear all Texture structs
	void ImGraphics::ClearTextureMap() {
		std::lock_guard lock(m_Lock);
		m_TextureMap.clear();
	}

	//Clear all the cached transforms
	void ImGraphics::ClearTransformedTextureCache() {
		std::lock_guard lock(m_Lock);
		for (auto& texture : m_TextureMap | views::values) {
			texture->transformedTextureCache.clear();
		}
	}

	// Check if an SVG image exists within the texture map
	bool ImGraphics::HasSvg(const std::string& a_name) {
		std::lock_guard lock(m_Lock);
		return m_TextureMap.contains(a_name);
	}

	// Get all loaded SVG names
	std::vector<std::string> ImGraphics::GetLoadedSvgNames() {
		std::lock_guard lock(m_Lock);
		std::vector<std::string> names;
		names.reserve(m_TextureMap.size());

		for (const auto& pair : m_TextureMap | views::keys) {
			names.push_back(pair);
		}

		return names;
	}

	bool ImGraphics::RasterizeSVG(Texture* a_svgTexture, ImVec2 a_size) {
		if (!a_svgTexture->document) {
			return false;
		}

		// Calculate aspect ratio for proper scaling
		float aspectRatio = a_svgTexture->originalSize.x / a_svgTexture->originalSize.y;

		// Calculate requested size if needed
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

		// --- START OF REVISION ---

		// Render SVG to bitmap with the requested size
		lunasvg::Bitmap bitmap = a_svgTexture->document->renderToBitmap(
			static_cast<uint32_t>(a_size.x),
			static_cast<uint32_t>(a_size.y)
		);

		if (!bitmap.valid()) {
			return false;
		}

		// Store the new size and pixel info
		a_svgTexture->size = a_size;
		UINT width = bitmap.width();
		UINT height = bitmap.height();
		a_svgTexture->pixelWidth = width;
		a_svgTexture->pixelHeight = height;

		// Cache pixel data (convert from RGBA to BGRA)
		a_svgTexture->pixelData.resize(width * height);
		const uint8_t* src = bitmap.data();
		for (size_t i = 0; i < a_svgTexture->pixelData.size(); ++i) {
			uint8_t r = src[i * 4 + 0];
			uint8_t g = src[i * 4 + 1];
			uint8_t b = src[i * 4 + 2];
			uint8_t a = src[i * 4 + 3];
			// Store as BGRA
			a_svgTexture->pixelData[i] = (a << 24) | (r << 16) | (g << 8) | b;
		}

		// Create DirectX texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		// Create texture with bitmap data
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = a_svgTexture->pixelData.data();
		subResource.SysMemPitch = width * 4;
		subResource.SysMemSlicePitch = 0;

		HRESULT hr = m_Device->CreateTexture2D(&desc, &subResource, pTexture.GetAddressOf());
		if (FAILED(hr)) {
			return false;
		}

		// Create shader resource view
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newTexture;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		hr = m_Device->CreateShaderResourceView(pTexture.Get(), &srvDesc, newTexture.GetAddressOf());

		if (SUCCEEDED(hr)) {
			// Safe swap the base texture pointer
			SwapBaseTexture(a_svgTexture, std::move(newTexture));
		}

		return SUCCEEDED(hr);
		// --- END OF REVISION ---
	}

	bool ImGraphics::CreateDefaultCheckerboardTexture(UINT tileSize, UINT tiles) {
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
		texture->pixelWidth = width;
		texture->pixelHeight = height;
		texture->pixelData = pixelData;

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

	void ImGraphics::CreateSamplers() {

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
		hr = m_Device->CreateSamplerState(&sd, &m_LinearSampler);
		if (FAILED(hr)) {
			logger::error("CreateSamplerState m_LinearSampler fail: {:X}", hr);
		}

		logger::info("Samplers built");
	}

	// Lanczos 'a' parameter
	static inline float LanczosKernel(float x, int a = 3) {
		if (x == 0.0f) return 1.0f;
		x = fabsf(x);
		if (x >= a) return 0.0f;
		const float pi_x = std::numbers::pi * x;
		return (sinf(pi_x) / pi_x) * (sinf(pi_x / a) / (pi_x / a));
	}

	//Resample loaded pixel data using Lanczos
	bool ImGraphics::ResampleRaster(Texture* tex, ImVec2 size) {
		if (!tex) return false;
		const UINT dstW = static_cast<UINT>(size.x);
		const UINT dstH = static_cast<UINT>(size.y);

		// If no original cached pixels, fall back to file-based WIC resample.
		if (tex->pixelDataOrig.empty()) {
			return false;
		}

		const UINT srcW = static_cast<UINT>(tex->originalSize.x);
		const UINT srcH = static_cast<UINT>(tex->originalSize.y);
		if (srcW == 0 || srcH == 0 || dstW == 0 || dstH == 0) return false;

		// Quick copy when sizes match
		if (srcW == dstW && srcH == dstH) {
			tex->pixelData = tex->pixelDataOrig;
		}
		else {
			// Lanczos window
			constexpr int a = 3; 
			// Precompute horizontal contributors
			struct Contrib {
				int first; std::vector<float> weights;
			};

			std::vector<Contrib> contribX(dstW);
			const float xScale = static_cast<float>(srcW) / static_cast<float>(dstW);
			for (UINT x = 0; x < dstW; ++x) {
				const float center = (x + 0.5f) * xScale - 0.5f;
				const int left = static_cast<int>(ceilf(center - a));
				const int right = static_cast<int>(floorf(center + a));
				contribX[x].first = left;
				contribX[x].weights.reserve(right - left + 1);
				float sum = 0.0f;
				for (int i = left; i <= right; ++i) {
					float w = LanczosKernel(center - static_cast<float>(i), a);
					contribX[x].weights.push_back(w);
					sum += w;
				}
				// normalize
				if (sum != 0.0f) {
					for (auto& w : contribX[x].weights) w /= sum;
				}
			}

			// Horizontal pass -> intermediate float buffer (channels in order B,G,R,A) normalized [0..255]
			std::vector<float> tmp(static_cast<size_t>(dstW) * srcH * 4);
			tmp.assign(tmp.size(), 0.0f);

			for (UINT y = 0; y < srcH; ++y) {
				const uint32_t* srcRow = tex->pixelDataOrig.data() + static_cast<size_t>(y) * srcW;
				for (UINT x = 0; x < dstW; ++x) {
					const Contrib& c = contribX[x];
					__m128 sumB = _mm_setzero_ps();
					__m128 sumG = _mm_setzero_ps();
					__m128 sumR = _mm_setzero_ps();
					__m128 sumA = _mm_setzero_ps();

					int sx = c.first;
					for (size_t k = 0; k < c.weights.size(); ++k, ++sx) {
						int sx_clamped = sx;
						if (sx_clamped < 0) sx_clamped = 0;
						else if (sx_clamped >= static_cast<int>(srcW)) sx_clamped = srcW - 1;
						uint32_t spix = srcRow[sx_clamped];
						// BGRA -> extract channels
						const float b = static_cast<float>(spix & 0xFFu);
						const float g = static_cast<float>((spix >> 8) & 0xFFu);
						const float r = static_cast<float>((spix >> 16) & 0xFFu);
						const float a_chan = static_cast<float>((spix >> 24) & 0xFFu);
						const float w = c.weights[k];
						__m128 wv = _mm_set1_ps(w);
						sumB = _mm_add_ps(sumB, _mm_mul_ps(_mm_set1_ps(b), wv));
						sumG = _mm_add_ps(sumG, _mm_mul_ps(_mm_set1_ps(g), wv));
						sumR = _mm_add_ps(sumR, _mm_mul_ps(_mm_set1_ps(r), wv));
						sumA = _mm_add_ps(sumA, _mm_mul_ps(_mm_set1_ps(a_chan), wv));
					}
					// store to tmp
					const size_t idx = (static_cast<size_t>(y) * dstW + x) * 4;
					tmp[idx + 0] = _mm_cvtss_f32(sumB);
					tmp[idx + 1] = _mm_cvtss_f32(sumG);
					tmp[idx + 2] = _mm_cvtss_f32(sumR);
					tmp[idx + 3] = _mm_cvtss_f32(sumA);
				}
			}

			// Precompute vertical contributors
			std::vector<Contrib> contribY(dstH);
			const float yScale = static_cast<float>(srcH) / static_cast<float>(dstH);
			for (UINT y = 0; y < dstH; ++y) {
				const float center = (y + 0.5f) * yScale - 0.5f;
				const int top = static_cast<int>(ceilf(center - a));
				const int bottom = static_cast<int>(floorf(center + a));
				contribY[y].first = top;
				contribY[y].weights.reserve(bottom - top + 1);
				float sum = 0.0f;
				for (int i = top; i <= bottom; ++i) {
					float w = LanczosKernel(center - static_cast<float>(i), a);
					contribY[y].weights.push_back(w);
					sum += w;
				}
				if (sum != 0.0f) {
					for (auto& w : contribY[y].weights) w /= sum;
				}
			}

			// Vertical pass -> final uint32_t pixels
			tex->pixelData.assign(static_cast<size_t>(dstW) * dstH, 0u);

			for (UINT x = 0; x < dstW; ++x) {
				for (UINT y = 0; y < dstH; ++y) {
					const Contrib& c = contribY[y];
					__m128 sumB = _mm_setzero_ps();
					__m128 sumG = _mm_setzero_ps();
					__m128 sumR = _mm_setzero_ps();
					__m128 sumA = _mm_setzero_ps();

					int sy = c.first;
					for (size_t k = 0; k < c.weights.size(); ++k, ++sy) {
						int sy_clamped = sy;
						if (sy_clamped < 0) sy_clamped = 0;
						else if (sy_clamped >= static_cast<int>(srcH)) sy_clamped = srcH - 1;
						const size_t idx = (static_cast<size_t>(sy_clamped) * dstW + x) * 4;
						const float b = tmp[idx + 0];
						const float g = tmp[idx + 1];
						const float r = tmp[idx + 2];
						const float a_chan = tmp[idx + 3];
						const float w = c.weights[k];
						__m128 wv = _mm_set1_ps(w);
						sumB = _mm_add_ps(sumB, _mm_mul_ps(_mm_set1_ps(b), wv));
						sumG = _mm_add_ps(sumG, _mm_mul_ps(_mm_set1_ps(g), wv));
						sumR = _mm_add_ps(sumR, _mm_mul_ps(_mm_set1_ps(r), wv));
						sumA = _mm_add_ps(sumA, _mm_mul_ps(_mm_set1_ps(a_chan), wv));
					}

					float fb = _mm_cvtss_f32(sumB);
					float fg = _mm_cvtss_f32(sumG);
					float fr = _mm_cvtss_f32(sumR);
					float fa = _mm_cvtss_f32(sumA);

					// clamp 0..255 and convert to uint8
					auto clamp8 = [](float v)->uint32_t {
						if (v <= 0.0f) return 0u;
						if (v >= 255.0f) return 255u;
						return static_cast<uint32_t>(v + 0.5f);
					};

					uint32_t bb = clamp8(fb);
					uint32_t gg = clamp8(fg);
					uint32_t rr = clamp8(fr);
					uint32_t aa = clamp8(fa);

					// pack BGRA -> lowest byte = B
					uint32_t out = (aa << 24) | (rr << 16) | (gg << 8) | bb;
					tex->pixelData[static_cast<size_t>(y) * dstW + x] = out;
				}
			}
		}

		// Create/Update D3D texture using tex->pixelData
		UINT stride = dstW * 4u;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = dstW; desc.Height = dstH;
		desc.MipLevels = 1; desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA sub{ tex->pixelData.data(), stride, 0 };

		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTex;
		HRESULT hr = m_Device->CreateTexture2D(&desc, &sub, pTex.GetAddressOf());
		if (FAILED(hr)) {
			logger::error("CreateTexture2D fail: {:X}", hr);
			return false;
		}

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newTexture;
		D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
		srv.Format = desc.Format;
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = 1;

		hr = m_Device->CreateShaderResourceView(pTex.Get(), &srv, newTexture.GetAddressOf());
		if (FAILED(hr)) {
			logger::error("CreateShaderResourceView fail: {:X}", hr);
			return false;
		}

		SwapBaseTexture(tex, std::move(newTexture));
		tex->pixelWidth = dstW;
		tex->pixelHeight = dstH;
		tex->size = ImVec2(static_cast<float>(dstW), static_cast<float>(dstH));
		return true;
	}

	bool ImGraphics::CreateTextureFromWICBitmap(Texture* texture, const BYTE* pixelData, UINT width, UINT height, UINT stride) {
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
		if (FAILED(hr)) return false;

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

	//-----------------
	//  TRANSFORMATION FUNCTIONS
	//-----------------

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ImGraphics::ApplyTransformations(Texture* texture, const ImageTransform& transform) {
		if (!texture || texture->pixelData.empty()) {
			logger::error("ApplyTransformations: null texture or no pixel data");
			return nullptr;
		}

		if (!m_Device) {
			logger::error("ApplyTransformations: D3D device not initialized");
			return nullptr;
		}

		// Work on a copy of the cached pixel data
		std::vector<uint32_t> pixels = texture->pixelData;
		UINT width = texture->pixelWidth;
		UINT height = texture->pixelHeight;

		if (pixels.empty() || width == 0 || height == 0) {
			logger::error("ApplyTransformations: invalid pixel data or dimensions");
			return nullptr;
		}

		// Apply recoloring first (if enabled)
		if (transform.recolorEnabled) {
			ApplyRecolor(pixels, width, height, transform.targetColor);
		}

		// Apply affine transformations
		if (transform.affine.rotation != 0.0f ||
			transform.affine.scale.x != 1.0f || transform.affine.scale.y != 1.0f ||
			transform.affine.translation.x != 0.0f || transform.affine.translation.y != 0.0f ||
			transform.affine.flipHorizontal || transform.affine.flipVertical) {
			ApplyAffineTransform(pixels, width, height, transform.affine);
		}

		// Apply cutoff last
		if (transform.cutoffDir != CutoffDirection::None) {
			ApplyCutoff(pixels, width, height, transform.cutoffDir, transform.cutoffPercent);
		}

		// Create or update the transformed texture
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> newTexture;
		// Pass the address of the ComPtr's internal pointer
		bool result = CreateTextureFromPixels(newTexture.GetAddressOf(), pixels, width, height);

		if (result) {
			return newTexture;
		}

		return nullptr;
	}

	void ImGraphics::ApplyRecolor(std::vector<uint32_t>& pixels, UINT width, UINT height, const ImVec4& color) {
		for (auto& pixel : pixels) {

			// Extract BGRA components
			uint8_t b = (pixel >> 0)  & 0xFF;
			uint8_t g = (pixel >> 8)  & 0xFF;
			uint8_t r = (pixel >> 16) & 0xFF;
			uint8_t a = (pixel >> 24) & 0xFF;

			// Calculate grayscale value (luminance)
			float gray = 0.299f * r + 0.587f * g + 0.114f * b;
			float intensity = gray / 255.0f;

			// Apply target color
			uint8_t newR = static_cast<uint8_t>(color.x * intensity * 255.0f);
			uint8_t newG = static_cast<uint8_t>(color.y * intensity * 255.0f);
			uint8_t newB = static_cast<uint8_t>(color.z * intensity * 255.0f);
			uint8_t newA = static_cast<uint8_t>(a * color.w);

			// Reconstruct pixel in BGRA format
			pixel = (newA << 24) | (newR << 16) | (newG << 8) | newB;
		}
	}

	void ImGraphics::ApplyAffineTransform(std::vector<uint32_t>& pixels, UINT& width, UINT& height, const AffineTransform& affine) {
		std::vector<uint32_t> output(pixels.size(), 0);

		float centerX = width * 0.5f;
		float centerY = height * 0.5f;

		float cosTheta = std::cos(affine.rotation);
		float sinTheta = std::sin(affine.rotation);

		for (UINT y = 0; y < height; ++y) {
			for (UINT x = 0; x < width; ++x) {

				// Translate to origin
				float tx = x - centerX;
				float ty = y - centerY;

				// Apply flips
				if (affine.flipHorizontal) tx = -tx;
				if (affine.flipVertical) ty = -ty;

				// Apply scale
				tx /= affine.scale.x;
				ty /= affine.scale.y;

				// Apply rotation
				float rx = tx * cosTheta + ty * sinTheta;
				float ry = -tx * sinTheta + ty * cosTheta;

				// Apply translation and move back from origin
				rx += centerX + affine.translation.x;
				ry += centerY + affine.translation.y;

				// Sample from source (with bounds checking)
				int srcX = static_cast<int>(std::round(rx));
				int srcY = static_cast<int>(std::round(ry));

				if (srcX >= 0 && srcX < static_cast<int>(width) &&
					srcY >= 0 && srcY < static_cast<int>(height)) {
					output[y * width + x] = pixels[srcY * width + srcX];
				}
			}
		}

		pixels = std::move(output);
	}

	void ImGraphics::ApplyCutoff(std::vector<uint32_t>& pixels, UINT width, UINT height, CutoffDirection direction, float percent) {
		percent = std::clamp(percent, 0.0f, 1.0f);

		for (UINT y = 0; y < height; ++y) {
			for (UINT x = 0; x < width; ++x) {
				bool shouldCutoff = false;

				switch (direction) {
					case CutoffDirection::LeftToRight: shouldCutoff = (static_cast<float>(x) / width) > percent;
					break;

					case CutoffDirection::RightToLeft: shouldCutoff = (static_cast<float>(x) / width) < (1.0f - percent);
					break;

					case CutoffDirection::TopToBottom: shouldCutoff = (static_cast<float>(y) / height) > percent;
					break;

					case CutoffDirection::BottomToTop: shouldCutoff = (static_cast<float>(y) / height) < (1.0f - percent);
					break;

					default: break;
				}

				if (shouldCutoff) {
					// Set alpha to 0 (make transparent)
					pixels[y * width + x] &= 0x00FFFFFF;
				}
			}
		}
	}

	bool ImGraphics::CreateTextureFromPixels(ID3D11ShaderResourceView** outTexture, const std::vector<uint32_t>& pixels, UINT width, UINT height) {
		if (*outTexture) {
			(*outTexture)->Release();
			*outTexture = nullptr;
		}

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA sub{};
		sub.pSysMem = pixels.data();
		sub.SysMemPitch = width * 4;

		ID3D11Texture2D* pTex = nullptr;
		HRESULT hr = m_Device->CreateTexture2D(&desc, &sub, &pTex);
		if (FAILED(hr)) {
			logger::error("CreateTextureFromPixels: CreateTexture2D fail: {:X}", hr);
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
		srv.Format = desc.Format;
		srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = 1;

		hr = m_Device->CreateShaderResourceView(pTex, &srv, outTexture);
		pTex->Release();

		if (FAILED(hr)) {
			logger::error("CreateTextureFromPixels: CreateShaderResourceView fail: {:X}", hr);
			return false;
		}

		return true;
	}


}