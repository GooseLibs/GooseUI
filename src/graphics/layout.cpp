#include "GooseUI/graphics/layout.h"


namespace GooseUI::graphics // Public
{
    void layout::getInitalOffsets(int (&distanceArray)[4], int windowWidth, int windowHeight, int X, int Y, int Width, int Height)
    {
        distanceArray[0] = windowWidth - (X + Width); // RIGHT
        if(distanceArray[0] < 0) { distanceArray[0] = -distanceArray[0]; }

        distanceArray[1] = X; // LEFT
        if(distanceArray[1] < 0) { distanceArray[1] = -distanceArray[1]; }

        distanceArray[2] = windowHeight - (Y + Height); // BOTTOM
        if(distanceArray[2] < 0) { distanceArray[2] = -distanceArray[2]; }

        distanceArray[3] = Y; // TOP
        if(distanceArray[3] < 0) { distanceArray[3] = -distanceArray[3]; }
    }
    
    void layout::calculateBitmapTextLayout(int widgetWidth, int widgetHeight, float &scale, int &textWidth, int &textHeight)
    {
        scale = widgetHeight / textHeight;
        if((textWidth * scale) > widgetWidth){ scale *= (float)widgetWidth / (textWidth * scale); } // Clamp
        
        textWidth = (int)(textWidth * scale);
        textHeight = (int)(textHeight * scale);
    }
    
    void layout::calculateLayout(bool calulateWithPos, widgetScaleing scaleMode, int alignments, layoutRestraints sizeRestraints, int *distanceArray, int windowWidth, int windowHeight, int windowPosX, int windowPosY, int &X, int &Y, int &Width, int &Height)
    {
        // Determine scaleing
        bool doVerticalScaleing = false, doHorizontalScaleing = false;
        if(scaleMode & widgetScaleing::SCALE_HORIZONTAL) { doHorizontalScaleing = true; }
        if(scaleMode & widgetScaleing::SCALE_VERTICAL) { doVerticalScaleing = true; }
        if(scaleMode & widgetScaleing::SCALE_ALL) { doHorizontalScaleing = true; doVerticalScaleing = true; }

        // With Scale - Left/Right/Top/Bottem
        if((alignments & widgetAlignment::ALIGN_LEFT) && (alignments & widgetAlignment::ALIGN_RIGHT) && doHorizontalScaleing) 
            { X = distanceArray[1]; Width  = windowWidth - distanceArray[1] - distanceArray[0]; }
        if((alignments & widgetAlignment::ALIGN_BOTTOM) && (alignments & widgetAlignment::ALIGN_TOP) && doVerticalScaleing)
            { Y = distanceArray[3]; Height = windowHeight - distanceArray[3] - distanceArray[2]; }

        // No Scale - Left/Right
        if(!calulateWithPos)
        {
            if((alignments & widgetAlignment::ALIGN_RIGHT) && !(alignments & widgetAlignment::ALIGN_LEFT) && !doHorizontalScaleing)
                { X = windowWidth - distanceArray[0] - Width; }
            if((alignments & widgetAlignment::ALIGN_RIGHT) && (alignments & widgetAlignment::ALIGN_LEFT) && !doHorizontalScaleing)
                { X = distanceArray[1] + ((windowWidth - distanceArray[1] - distanceArray[0] - Width) / 2); }
        }else 
        {
            // Add WindowPos to fix alignment
            if((alignments & widgetAlignment::ALIGN_RIGHT) && !(alignments & widgetAlignment::ALIGN_LEFT) && !doHorizontalScaleing)
                { X = windowPosX + windowWidth - distanceArray[0] - Width; }
            if((alignments & widgetAlignment::ALIGN_RIGHT) && (alignments & widgetAlignment::ALIGN_LEFT) && !doHorizontalScaleing)
                { X = windowPosX + distanceArray[1] + ((windowWidth - distanceArray[1] - distanceArray[0] - Width) / 2); }
        }

        // No Scale - Top/Bottom
        if(!calulateWithPos)
        {
            if((alignments & widgetAlignment::ALIGN_BOTTOM) && !(alignments & widgetAlignment::ALIGN_TOP) && !doVerticalScaleing)
                { Y = windowHeight - distanceArray[2] - Height; }
            if((alignments & widgetAlignment::ALIGN_BOTTOM) && (alignments & widgetAlignment::ALIGN_TOP) && !doVerticalScaleing)
                { Y = distanceArray[3] + ((windowHeight - distanceArray[3] - distanceArray[2] - Height) / 2); }
        }else 
        {
            // Add WindowPos to fix alignment
            if((alignments & widgetAlignment::ALIGN_BOTTOM) && !(alignments & widgetAlignment::ALIGN_TOP) && !doVerticalScaleing)
                { Y = windowPosY + windowHeight - distanceArray[2] - Height; }
            if((alignments & widgetAlignment::ALIGN_BOTTOM) && (alignments & widgetAlignment::ALIGN_TOP) && !doVerticalScaleing)
                { Y = windowPosY + distanceArray[3] + ((windowHeight - distanceArray[3] - distanceArray[2] - Height) / 2); }
        }

        // Clamps - For size, when the clamp is 0 the clamp is disabled and we default to 0
        if(sizeRestraints.minWidth > 0 && Width < sizeRestraints.minWidth) { Width = sizeRestraints.minWidth; }
        if(sizeRestraints.maxWidth > 0 && Width > sizeRestraints.maxWidth) { Width = sizeRestraints.maxWidth; }
        if(sizeRestraints.minHeight > 0 && Height < sizeRestraints.minHeight) { Height = sizeRestraints.minHeight; }
        if(sizeRestraints.maxHeight > 0 && Height > sizeRestraints.maxHeight) { Height = sizeRestraints.maxHeight; }
        
        // Clamps - For pos
        if(sizeRestraints.minX > 0 && X < sizeRestraints.minX) { X = sizeRestraints.minX; }
        if(sizeRestraints.maxX > 0 && X > sizeRestraints.minX) { X = sizeRestraints.maxX; }
        if(sizeRestraints.minY > 0 && Y < sizeRestraints.minX) { Y = sizeRestraints.minY; }
        if(sizeRestraints.maxY > 0 && Y > sizeRestraints.minX) { Y = sizeRestraints.maxY; }
        
        // Default Clamps - Don't go lower then zero ever
        if(X <= 0) { X = 0; }
        if(Y <= 0) { Y = 0; }
        if(Height < 0) { Height = 0; }
        if(Width < 0) { Width = 0; } 
    }
}