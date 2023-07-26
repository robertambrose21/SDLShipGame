#pragma once

#include "button.h"

class ActionButton : public Button{
private:
    int totalUses;
    int usesLeft;

public:
    ActionButton(
        uint32_t textureId, 
        const glm::ivec2& position, 
        const glm::ivec2& size, 
        int totalUses
    );

    void draw(GraphicsContext& graphicsContext);
    void click(void);

    void setTotalUses(int totalUses);
    int getTotalUses(void) const;

    void resetUses(void);
};