#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "graphicscontext.h"
#include "textureloader.h"
#include "camera.h"
#include "core/grid/grid.h"
#include "core/event/eventsubscriber.h"
#include "game/event/events.h"

class GraphicsContext;

// TODO: This is potentially overloaded - entities/projectiles and everything else call the 'draw' function
class GridRenderer : 
    public EventSubscriber<TileEventData>, 
    public EventSubscriber<GridEffectEvent>,
    public EventSubscriber<GridDirtyEventData>
{
private:
    typedef struct _chunk {
        glm::ivec2 min, max;
        std::unique_ptr<Texture> texture;
        bool textureNeedsRebuilding;
    } Chunk;

    const int ChunkSize = 32;

    int windowHeight;
    int tileSize;

    std::map<int, uint32_t> tileTexturesIds;
    std::vector<std::unique_ptr<Chunk>> chunks;
    Grid* grid;

    std::vector<std::unique_ptr<Chunk>> createChunks(void);
    void buildChunkTexture(GraphicsContext& graphicsContext, Chunk* chunk);
    bool isTileInChunk(Chunk* chunk, int x, int y);

    // TODO: Why on earth is the camera on the GridRenderer???? Move this!
    std::unique_ptr<Camera> camera;

public:
    GridRenderer(Grid* grid, int windowHeight);

    void setTileTexture(int tileId, uint32_t textureId);
    void draw(GraphicsContext& graphicsContext);
    void draw(GraphicsContext& graphicsContext, uint32_t textureId, const glm::ivec2& position);
    void draw(
        GraphicsContext& graphicsContext,
        uint32_t textureId,
        const Texture::Colour& colour,
        const uint8_t alpha,
        const glm::ivec2& position
    );

    void onPublish(const Event<TileEventData>& event);
    void onPublish(const Event<GridEffectEvent>& event);
    void onPublish(const Event<GridDirtyEventData>& event);

    Grid* getGrid(void);
    Camera& getCamera(void);

    glm::ivec2 getTilePosition(int x, int y) const;
    std::pair<int, int> getTileIndices(const glm::ivec2& position);
    int getTileSize(void) const;
};
