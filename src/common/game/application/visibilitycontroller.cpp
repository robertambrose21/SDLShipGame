#include "visibilitycontroller.h"

VisiblityController::VisiblityController()
{ }

void VisiblityController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void VisiblityController::revealTiles(int participantId, const std::vector<glm::ivec2>& tiles) {
    std::vector<TilesRevealedEventData::RevealedTile> tilesForEventData;

    auto grid = context->getGrid();

    for(auto& tile : tiles) {
        auto [t, isNew] = revealedTiles[participantId].insert(
            { tile.x, tile.y, true, grid->getTileAt(tile.x, tile.y) }
        );

        if(isNew) {
            tilesForEventData.push_back({ t->tile.id, t->x, t->y });
        }
    }

    if(!tilesForEventData.empty()) {
        publish<TilesRevealedEventData>({ participantId, tilesForEventData });
    }
}

const std::map<int, std::set<VisiblityController::RevealedTile>>& VisiblityController::getRevealedTiles(void) const {
    return revealedTiles;
}

const std::set<VisiblityController::RevealedTile>& VisiblityController::getRevealedTiles(int participantId) {
    if(!revealedTiles.contains(participantId)) {
        revealedTiles[participantId] = std::set<RevealedTile>();
    }

    return revealedTiles.at(participantId);
}

void VisiblityController::onPublish(const Event<EntitySetPositionEventData>& event) {
    auto entity = event.data.entity;

    if(!entity->hasParticipant()) {
        return;
    }
    
    auto tiles = context->getGrid()->getVisibleTiles(
        glm::vec2(event.data.position.x, event.data.position.y),
        entity->getAggroRange()
    );

    revealTiles(entity->getParticipantId(), tiles);
    
    std::unordered_set<glm::ivec2, glm::ivec2Hash> visibleTiles(tiles.begin(), tiles.end());

    for(auto const& other : context->getEntityPool()->getEntities()) {
        if(other->getParticipantId() == entity->getParticipantId()) {
            continue;
        }

        // TODO: should also check if the entity is on a visible tile
        auto distance = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(other->getPosition()));

        assignVisibility(entity, other, distance, visibleTiles);
        assignVisibility(other, entity, distance, visibleTiles);
    }
}

void VisiblityController::assignVisibility(
    Entity* entity, 
    Entity* other, 
    float distanceBetweenEntities,
    const std::unordered_set<glm::ivec2, glm::ivec2Hash>& visibleTiles
) {
    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());
    
    bool isInRange = distanceBetweenEntities < entity->getAggroRange();
    bool isInLOS = visibleTiles.find(glm::ivec2(other->getPosition().x, other->getPosition().y)) != visibleTiles.end();
    bool isVisible = isInRange && isInLOS;

    if(!participant->hasVisibleEntity(other) && isVisible) {
        participant->addVisibleEntity(other);
        publish<EntityVisibilityToParticipantData>({ other, participant->getId(), true });
    }
    else if(participant->hasVisibleEntity(other) && !isVisible) {
        participant->removeVisibleEntity(other);
        publish<EntityVisibilityToParticipantData>({ other, participant->getId(), false });
    }
}
