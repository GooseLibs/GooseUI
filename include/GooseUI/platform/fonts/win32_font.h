#ifndef _GOOSEUI_WIN32_FONT_H_
#define _GOOSEUI_WIN32_FONT_H_

#include "GooseUI/abstractions/iFont.h"
#include "GooseUI/font/atlas.h"
#include "GooseUI/font/glyph.h"

#include <unordered_map>

#include <dwrite.h>
#include <wrl.h>

namespace GooseUI
{
    namespace platform
    {
        class win32_font : public absractions::iFont
        {
            Microsoft::WRL::ComPtr<IDWriteFontFace> _face;
            Microsoft::WRL::ComPtr<IDWriteFontFile> _fontFile;

            font::atlas _atlas;
            font::fontData _fontData;
            std::unordered_map<uint32_t, font::glyph> _glyphs;
            
            bool _initilized = false;

            font::glyph _getGlyphBitmap(UINT16 index, float scale, const DWRITE_GLYPH_METRICS& glyphMetrics, const DWRITE_FONT_METRICS& fontMetrics, const font::cfg::bitmap& config);
            font::glyph _getGlyphSDF(UINT16 index, float scale, const DWRITE_GLYPH_METRICS& glyphMetrics, const DWRITE_FONT_METRICS& fontMetrics, const font::cfg::SDF& config);
            
            public:
            win32_font();

            // Overides
            bool load(const std::string &PathToFont, const font::fontData &fontData) override;

            const font::glyph& getGlyph(uint32_t codepoint) override;
            const font::fontData& getFontData() override;
            font::atlas* getAtlas() override;
        };
    }
}

#endif /*_GOOSEUI_WIN32_FONT_H_*/