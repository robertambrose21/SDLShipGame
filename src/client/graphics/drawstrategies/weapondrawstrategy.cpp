#include "weapondrawstrategy.h"

void WeaponDrawStrategy::draw(Weapon* drawable, GraphicsContext& graphicsContext) {
    switch(drawable->getType()) {
        case Weapon::Type::MELEE:
            drawMeleeWeapon(static_cast<MeleeWeapon*>(drawable), graphicsContext);
            break;

        case Weapon::Type::PROJECTILE:
            drawProjectileWeapon(static_cast<ProjectileWeapon*>(drawable), graphicsContext);
            break;
        
        default:
            break;
    }
}

void WeaponDrawStrategy::drawMeleeWeapon(MeleeWeapon* drawable, GraphicsContext& graphicsContext) {
    // no-op
}

void WeaponDrawStrategy::drawProjectileWeapon(ProjectileWeapon* drawable, GraphicsContext& graphicsContext) {
    // no-op
}