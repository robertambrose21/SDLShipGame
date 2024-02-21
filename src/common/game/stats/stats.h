#pragma once

#include <map>
#include <string>

class Stats {
public:
    int moves;
    int hp;
    int armour;

    Stats();
    Stats(int moves, int hp, int armour);

    void add(const Stats& other);
    void remove(const Stats& other);

    std::map<std::string, std::string> getValues(void);
};

class WeaponStats : Stats {
public:
    int range;
    int uses;
    int power;

    WeaponStats(int range, int uses, int power);

    void add(const WeaponStats& other);
    void remove(const WeaponStats& other);

    std::map<std::string, std::string> getValues(void);
};
