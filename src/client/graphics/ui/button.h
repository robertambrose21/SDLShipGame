#pragma once

#include <functional>

#include "core/glmimport.h"
#include "core/util/gameassert.h"
#include "../sdlimport.h"
#include "../graphicscontext.h"

class Button {
private:
    uint32_t textureId;
    glm::ivec2 position;
    glm::ivec2 size;
    bool isDisabled;

    std::function<void()> onClickFunc;

    bool isInBounds(int x, int y);

public:
    Button(uint32_t textureId, const glm::ivec2& position, const glm::ivec2& size);

    void draw(GraphicsContext& graphicsContext);
    bool handleClickEvent(int mouseX, int mouseY);
    void click(void);

    void onClick(std::function<void()>&& onClickFunc);
    void toggleDisabled(void);
    void setDisabled(bool isDisabled);
    bool getIsDisabled(void) const;

    void setPosition(const glm::ivec2& position);
    glm::ivec2 getPosition(void) const;
    void setSize(const glm::ivec2& size);
    glm::ivec2 getSize(void) const;
};