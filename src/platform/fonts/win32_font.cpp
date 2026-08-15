#include "GooseUI/platform/win32_font.h"

#include "GooseUI/font/fontManager.h"


namespace GooseUI::platform // Private
{
    font::glyph win32_font::_getGlyphBitmap(UINT16 index, float scale, const DWRITE_GLYPH_METRICS& glyphMetrics, const DWRITE_FONT_METRICS& fontMetrics, const font::cfg::bitmap& config)
    {
        IDWriteFactory* pFactory = reinterpret_cast<IDWriteFactory*>(font::manager::getFontFactory());
        font::glyph t_glyph{};
        
        // Rasterize
        Microsoft::WRL::ComPtr<IDWriteGlyphRunAnalysis> analysis;
        DWRITE_MATRIX transform = { 1.0f, 0, 0, -1.0f, 0, 0 };
        DWRITE_GLYPH_RUN dRun = {};
        dRun.fontFace = _face.Get();
        dRun.fontEmSize = _fontData.size;
        dRun.glyphCount = 1;
        dRun.glyphIndices = &index;
        
        HRESULT hResult = pFactory->CreateGlyphRunAnalysis(
            &dRun,
            1.0f,
            &transform,
            DWRITE_RENDERING_MODE_NATURAL,
            DWRITE_MEASURING_MODE_NATURAL, 
            0,
            0,
            &analysis
        );
        if(FAILED(hResult)) { printf("GooseUI: Faild to create Glyph Analysis \n"); return t_glyph; }

        // Create the Outline
        RECT bounds;
        analysis->GetAlphaTextureBounds(DWRITE_TEXTURE_CLEARTYPE_3x1, &bounds);

        int width = bounds.right - bounds.left;
        int height = bounds.bottom - bounds.top;
        
        if (width <= 0 || height <= 0)
        {
            // Keep metrics for space
            t_glyph.u0 = t_glyph.u1 = t_glyph.v0 = t_glyph.v1 = 0.0f;
            t_glyph.width  = 0.0f;
            t_glyph.height = 0.0f;
        
            t_glyph.xAdvance = (float)(glyphMetrics.advanceWidth * scale);
            t_glyph.xOffset  = (float)(glyphMetrics.leftSideBearing * scale);
            t_glyph.yOffset  = (float)(fontMetrics.ascent * scale);
        }else 
        {
            // Create and Convert bitmap to Grayscale
            std::vector<BYTE> bitmap(width * height * 3);
            std::vector<BYTE> bitmap_grayScale(width * height);
            analysis->CreateAlphaTexture(DWRITE_TEXTURE_CLEARTYPE_3x1, &bounds, bitmap.data(), (UINT32)bitmap.size());
            
            for (int i = 0; i < width * height; ++i)
            {
                BYTE r = bitmap[i*3 + 0];
                BYTE g = bitmap[i*3 + 1];
                BYTE b = bitmap[i*3 + 2];
                bitmap_grayScale[i] = (r + g + b) / 3;
            }
                    
            float u0, u1, v0, v1;
            _atlas.addGlyphData(width, height, 0, bitmap_grayScale.data(), u0, u1, v0, v1);

            if (config.createMipmaps && config.mipmap_amount > 0)
            {
                // TODO: Mipmap, I wana get the main rendering code updated first
                printf("GooseUI: Mipmap In-Development \n");
            }
            
            // Cache Glyph
            t_glyph.u0 = u0;
            t_glyph.u1 = u1;
            t_glyph.v0 = v0;
            t_glyph.v1 = v1;
            t_glyph.width = (float)width;
            t_glyph.height = (float)height;
            t_glyph.xAdvance = (float)(glyphMetrics.advanceWidth * scale);
            t_glyph.xOffset = (float)(glyphMetrics.leftSideBearing * scale);
            t_glyph.yOffset = -bounds.top;
        }

        return t_glyph;
    }

    font::glyph win32_font::_getGlyphSDF(UINT16 index, float scale, const DWRITE_GLYPH_METRICS& glyphMetrics, const DWRITE_FONT_METRICS& fontMetrics, const font::cfg::SDF& config)
    {
        IDWriteFactory* pFactory = reinterpret_cast<IDWriteFactory*>(font::manager::getFontFactory());
        font::glyph t_glyph{};

        // Ima get bitmap working before SDF
        printf("GooseUI: SDF Indevelopment \n"); return t_glyph;
    }
}

namespace GooseUI::platform // Public
{
    win32_font::win32_font()
    {
        if(font::manager::getFontFactory() != nullptr){ return; }
        Microsoft::WRL::ComPtr<IDWriteFactory> factory;
        
        HRESULT hResult = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &factory);
        if(FAILED(hResult)) { printf("GooseUI: Faild to create DirectWriteFactory \n"); }

        application::setFontFactory(reinterpret_cast<void*>(factory.Get()));
    }

    bool win32_font::load(const std::string &PathToFont, const font::fontData &fontData)
    {
        _initilized = false;
        IDWriteFactory* pFactory = reinterpret_cast<IDWriteFactory*>(application::getFontFactory());
        if(pFactory == nullptr){ printf("GooseUI: Faild to get DirectWriteFactory \n"); return false; }

        _fontData = fontData;

        // DPI
        HDC screen = GetDC(nullptr);
        int DPIx = GetDeviceCaps(screen, LOGPIXELSX);
        ReleaseDC(nullptr, screen);
        
        float scale = (DPIx / 96.0f) + 1;
        _fontData.size = fontData.size * scale;

        // Load font
        HRESULT hResult = pFactory->CreateFontFileReference(std::wstring(PathToFont.begin(), PathToFont.end()).c_str(), nullptr, &_fontFile);
        if(FAILED(hResult)) { printf("GooseUI: Faild to load Font: %s \n", PathToFont.c_str()); return false; }

        // Analyze - I did me research >:3
        BOOL supported = false; 
        UINT32 numberOfFaces;
        DWRITE_FONT_FILE_TYPE fileType;
        DWRITE_FONT_FACE_TYPE faceType;

        _fontFile->Analyze(&supported, &fileType, &faceType, &numberOfFaces);
        if(!supported) { return false; }

        hResult = pFactory->CreateFontFace(faceType, 1, _fontFile.GetAddressOf(), 0, DWRITE_FONT_SIMULATIONS_NONE, &_face);
        if(FAILED(hResult)) { return false; }
        
        _initilized = true;
        return true;
    }

    const font::glyph& win32_font::getGlyph(uint32_t codepoint)
    {
        if(!_initilized) { static font::glyph empty{}; return empty; }

        std::unordered_map<uint32_t, font::glyph>::iterator iterator = _glyphs.find(codepoint);
        if(iterator != _glyphs.end()) { return iterator->second; }

        // Map point to index then get metrics
        UINT16 index = 0;
        HRESULT hResult = _face->GetGlyphIndicesA(&codepoint, 1, &index);
        if(FAILED(hResult) || index == 0) { index = 0; }
        
        DWRITE_GLYPH_METRICS glyphMetrics{};
        DWRITE_FONT_METRICS fontMetrics{};
        _face->GetDesignGlyphMetrics(&index, 1, &glyphMetrics);
        _face->GetMetrics(&fontMetrics);

        float scale = _fontData.size / static_cast<float>(fontMetrics.designUnitsPerEm);
        font::glyph t_glyph {};

        if(_fontData.fontType == font::type::bitmap)
        {
            t_glyph = _getGlyphBitmap(index, scale, glyphMetrics, fontMetrics, std::get<font::cfg::bitmap>(_fontData.advanceConfiguration));
        }
        else if(_fontData.fontType == font::type::SDF)
        {
            t_glyph = _getGlyphSDF(index, scale, glyphMetrics, fontMetrics, std::get<font::cfg::SDF>(_fontData.advanceConfiguration));
        }

        _glyphs[codepoint] = t_glyph;
        return _glyphs[codepoint];
    }

    const font::fontData& win32_font::getFontData(){ return _fontData; }
    font::atlas* win32_font::getAtlas(){ return &_atlas; }
}