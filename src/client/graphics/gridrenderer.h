#pragma once

#include <vector>
#include <unordered_set>
#include <glm/glm.hpp>

#include "graphicscontext.h"
#include "textureloader.h"
#include "camera.h"
#include "core/grid/grid.h"
#include "core/event/eventsubscriber.h"
#include "game/event/events.h"
#include "game/application/visibilitycontroller.h"
#include "game/participant/participant.h"
#include "ui/text.h"

class GraphicsContext;

// TODO: This is potentially overloaded - entities/projectiles and everything else call the 'draw' function
// This is currently the 'GameRenderer' not the grid renderer. Split up at some point
class GridRenderer : 
    public EventSubscriber<TileEventData>, 
    public EventSubscriber<GridEffectEvent>,
    public EventSubscriber<GridDirtyEventData>,
    public EventSubscriber<TilesRevealedEventData>,
    public EventSubscriber<EntitySetPositionEventData>
{
private:
    typedef struct _chunk {
        int id;
        glm::ivec2 min, max;
        std::unique_ptr<Texture> texture;
        bool textureNeedsRebuilding;
    } Chunk;

    const int ChunkSize = 32;

    int windowHeight;
    int tileSize;

    std::unique_ptr<Texture> fogTexture;
    bool fogTextureNeedsRebuilding;
    std::unique_ptr<Texture> debugTexture;

    std::vector<std::unique_ptr<Text>> coords;

    std::map<int, uint32_t> tileTexturesIds;
    std::vector<std::unique_ptr<Chunk>> chunks;
    Grid* grid;
    VisiblityController* visiblityController;
    EntityPool* entityPool;

    std::vector<std::unique_ptr<Chunk>> createChunks(void);
    void buildChunkTexture(GraphicsContext& graphicsContext, Chunk* chunk);
    bool isTileInChunk(Chunk* chunk, int x, int y);

    void buildFogBorders(GraphicsContext& graphicsContext, int xMin, int xMax, int yMin, int yMax);
    void buildFogTiles(
        GraphicsContext& graphicsContext, 
        Entity* entity, 
        int xMin, 
        int xMax, 
        int yMin, 
        int yMax
    );
    void buildFogTexture(GraphicsContext& graphicsContext);
    void buildDebugTexture(GraphicsContext& graphicsContext);
    void drawDebugTexture(GraphicsContext& graphicsContext);

    // TODO: Why on earth is the camera on the GridRenderer???? Move this!
    std::unique_ptr<Camera> camera;
    Participant* participant;

public:
    GridRenderer(
        Grid* grid, 
        VisiblityController* visiblityController,
        EntityPool* entityPool,
        int windowHeight
    );

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
    void onPublish(const Event<TilesRevealedEventData>& event);
    void onPublish(const Event<EntitySetPositionEventData>& event);

    Grid* getGrid(void);
    Camera& getCamera(void);

    void setParticipant(Participant* participant);
    Participant* getParticipant(void);

    glm::ivec2 getTilePosition(int x, int y) const;
    std::pair<int, int> getTileIndices(const glm::ivec2& position);
    int getTileSize(void) const;
};
