#pragma once

#include "drawstrategy.h"
#include "game/weapons/weapon.h"
#include "game/weapons/meleeweapon.h"
#include "game/weapons/projectileweapon.h"

class WeaponDrawStrategy : public DrawStrategy<Weapon> {
public:
    void draw(Weapon* drawable, GraphicsContext& graphicsContext);

private:
    void drawMeleeWeapon(MeleeWeapon* drawable, GraphicsContext& graphicsContext);
    void drawProjectileWeapon(ProjectileWeapon* drawable, GraphicsContext& graphicsContext);
};