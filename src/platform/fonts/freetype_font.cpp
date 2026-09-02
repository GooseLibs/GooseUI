#include "GooseUI/platform/fonts/freetype_font.h"

#include "GooseUI/font/fontManager.h"


namespace GooseUI::platform // Private
{
    font::glyph freetype_font::_getGlyphBitmap(uint16_t index, float scale, const FT_GlyphSlot& glyphMetrics, const FT_Size_Metrics& fontMetrics, const font::cfg::bitmap& config)
    {
        FT_Library pFactory = reinterpret_cast<FT_Library>(font::manager::instance().getFontFactory());
        font::glyph t_glyph{};

        // Rasterize
        FT_Error error = FT_Set_Pixel_Sizes(_face, 0, static_cast<FT_UInt>(_fontData.size));
        if(error){ printf("GooseUI: Failed to set font pixel size.\n"); return t_glyph; }

        error = FT_Load_Glyph(_face, index, FT_LOAD_DEFAULT);
        if(error){ printf("GooseUI: Failed to load glyph for rasterization.\n"); return t_glyph; }

        error = FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL);
        if(error){ printf("GooseUI: Failed to render glyph bitmap.\n"); return t_glyph; }

        // Handle Spaces
        FT_Bitmap& bitmap = _face->glyph->bitmap;
        
        if(bitmap.width <= 0 || bitmap.rows <= 0)
        {
            t_glyph.u0 = t_glyph.u1 = t_glyph.v0 = t_glyph.v1 = 0.0f;
            t_glyph.width  = 0.0f;
            t_glyph.height = 0.0f;

            t_glyph.xAdvance = (float)_face->glyph->advance.x / 64.f;
            t_glyph.xOffset  = (float)_face->glyph->bitmap_left;
            t_glyph.yOffset  = (float)_face->glyph->bitmap_top;
        }else 
        {
            float u0, u1, v0, v1;
            _atlas.addGlyphData(bitmap.width, bitmap.rows, 0, bitmap.buffer, u0, u1, v0, v1);
            
            if (config.createMipmaps && config.mipmap_amount > 0)
            {
                // TODO: Mipmap, I wana get the main rendering code updated first
                printf("GooseUI: Mipmap In-Development \n");
            }
                        

            t_glyph.u0 = u0;
            t_glyph.u1 = u1;
            t_glyph.v0 = v1;
            t_glyph.v1 = v0; 
            t_glyph.width = (float)bitmap.width;
            t_glyph.height = (float)bitmap.rows;
            t_glyph.xAdvance = (float)(_face->glyph->advance.x * scale);
            t_glyph.xOffset = (float)(_face->glyph->bitmap_left * scale);
            t_glyph.yOffset = -_face->glyph->bitmap_top + (float)bitmap.rows;
        }

        return t_glyph;
    }

    font::glyph freetype_font::_getGlyphSDF(uint16_t index, float scale, const FT_GlyphSlot& glyphMetrics, const FT_Size_Metrics& fontMetrics, const font::cfg::SDF& config)
    {
        FT_Library pFactory = reinterpret_cast<FT_Library>(font::manager::instance().getFontFactory());
        font::glyph t_glyph{};

        // Ima get bitmap working before SDF
        printf("GooseUI: SDF Indevelopment \n"); return t_glyph;
    }
}

namespace GooseUI::platform // Public
{
    freetype_font::freetype_font()
    {
        if(font::manager::instance().getFontFactory() != nullptr){ return; }
        FT_Library library;

        if(FT_Init_FreeType(&library))
            { printf("GooseUI: Faild to init FreeType \n"); }

        font::manager::instance().setFontFactory(reinterpret_cast<void*>(library));
    }

    bool freetype_font::load(const std::string &PathToFont, const font::fontData &fontData)
    {
        _initilized = false;
        FT_Library pFactory = reinterpret_cast<FT_Library>(font::manager::instance().getFontFactory());
        if(pFactory == nullptr){ printf("GooseUI: Faild to get FT_Library \n"); return false; }

        _fontData = fontData;

        // DPI - Still too lazy to fix this
        _fontData.size = fontData.size + 1;

        // Load Font
        FT_Error error = FT_New_Face(pFactory, PathToFont.c_str(), 0, &_face);
        if(error){ printf("GooseUI: Faild to load Font: %s \n", PathToFont.c_str()); return false; }

        _initilized = true;
        return true;
    }

    const font::glyph& freetype_font::getGlyph(uint32_t codepoint)
    {
        if(!_initilized) { static font::glyph empty{}; return empty; }

        std::unordered_map<uint32_t, font::glyph>::iterator iterator = _glyphs.find(codepoint);
        if(iterator != _glyphs.end()) { return iterator->second; }

        // Map point to index then get metrics
        FT_UInt index = FT_Get_Char_Index(_face, codepoint);
        if(index == 0){ index = FT_Get_Char_Index(_face, '?'); }

        if(FT_Load_Glyph(_face, index, FT_LOAD_DEFAULT) != 0){ static font::glyph empty{}; return empty; }

        FT_GlyphSlot glyphMetrics = _face->glyph;
        FT_Size_Metrics& fontMetrics = _face->size->metrics;
        
        float scale = _fontData.size / static_cast<float>(_face->units_per_EM);
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

    const font::fontData& freetype_font::getFontData(){ return _fontData; }
    font::atlas* freetype_font::getAtlas(){ return &_atlas; }
}