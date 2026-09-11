#pragma once

#include <cstdint>
#include "../main.h"
#include "../game/RW/RenderWare.h"

class CRGBA
{
public:
    uint8_t R{}, G{}, B{}, A{};

    CRGBA(unsigned char red, unsigned char green, unsigned blue);
    CRGBA(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha);
    CRGBA(CRGBA const &rhs);
    CRGBA(unsigned int intValue);
    CRGBA();

    void Set(unsigned char red, unsigned char green, unsigned blue);
    void Set(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha);
    void Set(CRGBA const &rhs);
    void Set(CRGBA const &rhs, unsigned char alpha);
    void Set(unsigned int intValue);

    CRGBA ToRGB() const;
    unsigned int ToInt() const;
    unsigned int ToIntARGB() const;

    void Invert();
    CRGBA Inverted() const;

    bool operator==(CRGBA const &rhs) const;
    CRGBA &operator=(CRGBA const &rhs);

    operator RwRGBA()     { return { R, G, B, A }; }
};