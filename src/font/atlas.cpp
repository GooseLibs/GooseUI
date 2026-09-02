#include "GooseUI/font/atlas.h"

#include <cstdio>
#include <cstring>


namespace GooseUI::font // Public
{
    atlas::atlas(int width, int height, int channels)
        : _width(width), _height(height), _colorChannel(channels), _x(0), _y(0), _rowHeight(0), _pixels(width * height * channels, 0), _id(0) {}
    atlas::~atlas() {}

    bool atlas::addGlyphData(int width, int height, int padding, const uint8_t* data, float &u0, float &u1, float &v0, float &v1)
    {
        // Do we need to start new row?
        if(_x + width >= _width)
        {
            _x = 0;
            _y += _rowHeight + padding;
            _rowHeight = 0;
        }

        // Is Atlas full?
        if(_y + height >= _height){ return false; }

        for(int row = 0; row < height; ++row) 
        { 
            memcpy(&_pixels[(_y + row) * _width + _x], &data[row * width], width);
        }

        u0 = (float)_x / (float)_width;
        u1 = (float)(_x + width) / (float)_width;
        v0 = (float)_y / (float)_height;
        v1 = (float)(_y + height) / (float)_height;

        // Update Cursor
        _x += width + padding;
        if(height > _rowHeight) { _rowHeight = height; }

        _uploaded = false;
        return true;
    }

    void atlas::uploaded(){ _uploaded = true; }
    void atlas::setid(unsigned int id){ _id = id; }

    std::vector<uint8_t>& atlas::getPixels(){ return _pixels; }
    unsigned int atlas::getId(){ return _id; }
    bool atlas::isUploaded(){ return _uploaded; }
    int atlas::getWidth(){ return _width; }
    int atlas::getHeight(){ return _height; }
    int atlas::getColorChannels(){ return _colorChannel; }
}