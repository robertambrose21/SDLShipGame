#pragma once

#include "graphics/graphicscontext.h"

template<class T>
class DrawStrategy {
public:
    virtual void draw(T* drawable, GraphicsContext& graphicsContext) = 0;
};
