#pragma once

typedef struct _stats {
    _stats() :
        moves(0),
        hp(0),
        armour(0)
    { }

    _stats(int moves, int hp, int armour) :
        moves(moves),
        hp(hp),
        armour(armour)
    { }

    void add(const _stats& other) {
        moves += other.moves;
        hp += other.hp;
        armour += other.armour;
    }

    void remove(const _stats& other) {
        moves -= other.moves;
        hp -= other.hp;
        armour -= other.armour;
    }

    int moves;
    int hp;
    int armour;
} Stats;
