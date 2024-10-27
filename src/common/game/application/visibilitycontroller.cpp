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

const std::map<int, std::unordered_set<glm::ivec2, glm::ivec2Hash>>& VisiblityController::getVisibleTiles(void) const {
    return visibleTiles;
}

const std::unordered_set<glm::ivec2, glm::ivec2Hash>& VisiblityController::getVisibleTiles(int participantId) {
    if(!visibleTiles.contains(participantId)) {
        visibleTiles[participantId] = std::unordered_set<glm::ivec2, glm::ivec2Hash>();
    }

    return visibleTiles.at(participantId);
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

    auto participantId = entity->getParticipantId();

    revealTiles(participantId, tiles);
    visibleTiles[participantId] = std::unordered_set<glm::ivec2, glm::ivec2Hash>(tiles.begin(), tiles.end());

    for(auto const& other : context->getEntityPool()->getEntities()) {
        if(other->getParticipantId() == entity->getParticipantId()) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(other->getPosition()));

        assignVisibility(entity, other, distance, visibleTiles[participantId]);
        assignVisibility(other, entity, distance, visibleTiles[other->getParticipantId()]);
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
    bool isInLOS = contains(visibleTiles, other->getPosition());
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

bool VisiblityController::isVisible(Entity* entity, Entity* target) {
    auto distance = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(target->getPosition()));

    bool isInRange = distance < entity->getAggroRange();
    bool isInLOS = contains(visibleTiles[entity->getParticipantId()], target->getPosition());
    
    return isInRange && isInLOS;
}
