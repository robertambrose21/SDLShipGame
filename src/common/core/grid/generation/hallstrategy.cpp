#include "hallstrategy.h"

HallStrategy::HallStrategy(int width, int height, int maxTunnels, int maxTunnelLength) :
    GenerationStrategy(width, height, RoomConfiguration()),
    maxTunnels(maxTunnels),
    maxTunnelLength(maxTunnelLength)
{ }

std::vector<std::vector<Grid::Tile>> HallStrategy::generate(void) {
    for(auto i = 0; i < getWidth(); i++) {
        for(auto j = 0; j < getHeight(); j++) {
            setTile(i, j, { 2, false });
        }
    }

    int stepsLeft = 0;
    int x = 0, y = 0;
    int direction = 0;

    while(maxTunnels > 0) {
        if(stepsLeft == 0) {
            direction = randomRange(0, 3);
            stepsLeft = randomRange(1, maxTunnelLength);
            maxTunnels--;
        }

        if(direction == 0 && x < getWidth() - 1) {
            x++;
        }
        else if(direction == 1 && x > 0) {
            x--;
        }
        else if(direction == 2 && y < getHeight() - 1) {
            y++;
        }
        else if(direction == 3 && y > 0) {
            y--;
        }

        setTile(x, y, { 1, true });
        stepsLeft--;
    }

    return getData();
}