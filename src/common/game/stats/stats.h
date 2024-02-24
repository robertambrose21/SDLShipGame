#pragma once

#include <map>
#include <string>

template<class T>
class Stats {
public:
    virtual void add(const T& other) = 0;
    virtual void remove(const T& other) = 0;

    virtual std::map<std::string, std::string> getValues(void) = 0;
};

class CommonStats : Stats<CommonStats> {
public:
    int moves;
    int hp;
    int armour;

    CommonStats();
    CommonStats(int moves, int hp, int armour);

    void add(const CommonStats& other);
    void remove(const CommonStats& other);

    std::map<std::string, std::string> getValues(void);
};

class WeaponStats : Stats<WeaponStats> {
public:
    int range;
    int uses;
    int power;

    WeaponStats();
    WeaponStats(int range, int uses, int power);

    void add(const WeaponStats& other);
    void remove(const WeaponStats& other);

    std::map<std::string, std::string> getValues(void);
};

class AllStats : Stats<AllStats> {
public:
    CommonStats common;
    WeaponStats weapon;

    AllStats();

    void add(const AllStats& other);
    void remove(const AllStats& other);

    std::map<std::string, std::string> getValues(void);
};
