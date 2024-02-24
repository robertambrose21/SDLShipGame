#include "statuseffect.h"

StatusEffect::StatusEffect(int turnsActive) :
    turnsActive(turnsActive),
    turnsLeft(turnsActive)
{ }

// void StatusEffect::apply(const EntityBaseStats& baseStats) {
//     if(turnsLeft <= 0) {
//         return;
//     }

//     for(auto const& modifier : modifiers) {
//         switch(modifier.statName) {
//             case BaseStat::MovesPerTurn:
//                 modify(baseStats.movesPerTurn, modifier.value, modifier.operation);
//                 break;

//             case BaseStat::TotalHP:
//                 modify(baseStats.totalHP, modifier.value, modifier.operation);
//                 break;

//             default:
//                 break;
//         }
//     }

//     turnsLeft--;
// }

double StatusEffect::modify(double currentValue, double modifierValue, char operation) {
    switch(operation) {
        case '+':
            return currentValue + modifierValue;
            break;

        case '-':
            return currentValue - modifierValue;
            break;

        case '*':
            return currentValue * modifierValue;
            break;

        default:
            return currentValue;
        }
}

void StatusEffect::addModifier(const StatusEffect::Modifier& modifier) {
    modifiers.insert(modifier);
}

int StatusEffect::getTurnsLeft(void) const {
    return turnsLeft;
}
