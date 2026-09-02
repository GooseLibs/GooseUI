#include "GooseUI/abstractions/iFont.h"

#if defined(_WIN32) // Use DirectWrite

    #include "GooseUI/platform/fonts/win32_font.h"
    GooseUI::absractions::iFont* GooseUI::absractions::createFont() { return new GooseUI::platform::win32_font; }
#else // Use Freetype
    #include "GooseUI/platform/fonts/freetype_font.h"
    GooseUI::absractions::iFont* GooseUI::absractions::createFont() { return new GooseUI::platform::freetype_font; }
#endif