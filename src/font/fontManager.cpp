#include "GooseUI/font/fontManager.h"
#include "GooseUI/abstractions/iFont.h"


namespace GooseUI::font
{
    manager& manager::instance()
    {
        static manager instance;
        return instance;
    }

    void manager::setFontFactory(void *factory)
    {
        if(_fontFactory){ return; }
        _fontFactory = factory;
    }
    
    void* manager::getFontFactory(){ return _fontFactory; }

    absractions::iFont* manager::getFont(const std::string &PathToFont, const font::fontData &fontData)
    {
         std::string key = PathToFont + "_" + ((fontData.fontType == type::bitmap) ? "bitmap" : "SDF") + "_" + std::to_string(fontData.size);

         std::unordered_map<std::string, absractions::iFont*>::iterator iterator = _fonts.find(key);
         if(iterator != _fonts.end()){ return iterator->second; }

         absractions::iFont* font = absractions::createFont();
         if(!font->load(PathToFont, fontData)){ return nullptr; }

         _fonts[key] = font;
         return _fonts[key];
    }
}