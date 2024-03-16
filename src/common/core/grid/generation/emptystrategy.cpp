#include "emptystrategy.h"

EmptyStrategy::EmptyStrategy(int width, int height) :
    GenerationStrategy(width, height)
{ }

std::vector<std::vector<Grid::Tile>> EmptyStrategy::generate(void) {
    for(auto i = 0; i < getWidth(); i++) {
        for(auto j = 0; j < getHeight(); j++) {
            setTile(i, j, { 1, true });
        }
    }

    return getData();
}