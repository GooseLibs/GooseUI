#include "GooseUI/font/fontLayout.h"

#include <string>
#include <cstdint>


namespace // Local
{
    std::u32string U8toU32(const std::string& utf8_string)
    {
        // Only if std::codecvt was not removed ߹𖥦߹
        std::u32string convertedString;
        
        size_t i = 0;
        while(i < utf8_string.size())
        {
            uint32_t codePoint = 0;
            unsigned char character = utf8_string[i];
    
            // Yandare Dev Moment - I hate UTF conversion
            if(character < 0x80)
            {
                codePoint = character;
                i += 1;
            }else
            if((character >> 5) == 0x6)
            {
                if(i + 1 >= utf8_string.size()) { convertedString.push_back(U'\uFFFD'); break; }
                else { codePoint = ((character & 0x1F) << 6) | (utf8_string[i + 1] & 0x3f); }
                i += 2;
            }else 
            if((character >> 4) == 0xE)
            {
                if(i + 2 >= utf8_string.size()) { convertedString.push_back(U'\uFFFD'); break; }
                else { codePoint = ((character & 0x0F) << 12) | ((utf8_string[i + 1] & 0x3F) << 6) | (utf8_string[i + 2] & 0x3F); }
                i += 3;
            }else 
            if((character >> 3) == 0x1E)
            {
                if(i + 3 >= utf8_string.size()) { convertedString.push_back(U'\uFFFD'); break; }
                else { codePoint = ((character & 0x07) << 18) | ((utf8_string[i + 1] & 0x3F) << 12) | ((utf8_string[i + 2] & 0x3F) << 6) | (utf8_string[i + 3] & 0x3F); }
                i += 4;
            }else 
            {
                convertedString.push_back(U'\uFFFD'); 
                break;
            }
    
            convertedString.push_back(static_cast<char32_t>(codePoint));
        }
    
        return convertedString;
    }
}

namespace GooseUI::font // Public
{
    void layout::setText(const std::string& text, absractions::iFont* font)
    {
        _glyphsPosistions.clear();
        
        float _xPos = 0.0f;
        for(char32_t character : U8toU32(text))
        {
            const glyph& targetGlyph = font->getGlyph(character);
            _glyphsPosistions.push_back({&targetGlyph, _xPos + targetGlyph.xOffset, targetGlyph.yOffset});
            _xPos += targetGlyph.xAdvance;
        }
    }

    const std::vector<glyphPos>& layout::getGlyphs() const { return _glyphsPosistions; }
}