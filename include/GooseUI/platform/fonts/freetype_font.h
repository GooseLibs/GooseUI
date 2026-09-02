#ifndef _GOOSEUI_FREETYPE_FONT_H_
#define _GOOSEUI_FREETYPE_FONT_H_

#include "GooseUI/abstractions/iFont.h"
#include "GooseUI/font/atlas.h"
#include "GooseUI/font/glyph.h"

#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace GooseUI
{
    namespace platform
    {
        class freetype_font : public absractions::iFont
        {
            FT_Face _face = nullptr;

            font::atlas _atlas;
            font::fontData _fontData;
            std::unordered_map<uint32_t, font::glyph> _glyphs;

            bool _initilized = false;

            font::glyph _getGlyphBitmap(uint16_t index, float scale, const FT_GlyphSlot& glyphMetrics, const FT_Size_Metrics& fontMetrics, const font::cfg::bitmap& config);
            font::glyph _getGlyphSDF(uint16_t index, float scale, const FT_GlyphSlot& glyphMetrics, const FT_Size_Metrics& fontMetrics, const font::cfg::SDF& config);

            public:
            freetype_font();

            // Overides
            bool load(const std::string &PathToFont, const font::fontData &fontData) override;

            const font::glyph& getGlyph(uint32_t codepoint) override;
            const font::fontData& getFontData() override;
            font::atlas* getAtlas() override;
        };
    }
}

#endif /*_GOOSEUI_FREETYPE_FONT_H_*/