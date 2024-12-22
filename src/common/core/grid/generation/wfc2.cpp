#include "wfc2.h"

WFC2::WFC2(
    Grid* grid,
    const RoomConfiguration& roomConfiguration,
    const WFCTileSet& tileSet
) : 
    GenerationStrategy(grid->getWidth(), grid->getHeight(), roomConfiguration),
    tileSet(tileSet)
{ }

std::vector<std::vector<Grid::Tile>> WFC2::generate(void) {
    std::cout << "Generating map... " << std::endl;

    auto wfcTiles = tileSet.getTiles();
    auto neighbours = tileSet.getNeighbours();
    auto walkableTiles = tileSet.getWalkableTiles();

    auto seed = randomRange(0, INT_MAX);
    TilingWFC<WFCTileSet::WFCTile> wfc(wfcTiles, neighbours, getHeight(), getWidth(), { false }, seed);

    auto success = wfc.run();

    if(!success.has_value()) {
        std::cout << "Failed generating map" << std::endl;
        return getData();
    }

    for(int x = 0; x < getWidth(); x++) {
        for(int y = 0; y < getHeight(); y++) {
            auto const& wfcTile = (*success).data[y * getWidth() + x];
            
            setTile(x, y, { 
                (int) wfcTile.id, 
                tileSet.isTileWalkable(wfcTile.id),
                false, 
                wfcTile.orientation 
            });
        }
    }

    std::cout << "Done, seed: " << seed << std::endl;
    return getData();
}
