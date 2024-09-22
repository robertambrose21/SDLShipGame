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

    // TODO: This is the server participant so nothing to do here, might be a nicer way to do this
    if(entity->getParticipantId() == 0) {
        return;
    }

    auto participant = context->getTurnController()->getParticipant(entity->getParticipantId());

    auto tiles = context->getGrid()->getTilesInCircle(
        event.data.position.x, 
        event.data.position.y, 
        entity->getAggroRange()
    );

    revealTiles(entity->getParticipantId(), tiles);

    auto entities = context->getEntityPool()->getEntities();

    for(auto other : entities) {
        if(other->getParticipantId() == entity->getParticipantId()) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(entity->getPosition()), glm::vec2(other->getPosition()));
        bool isVisible = distance < entity->getAggroRange();

        if(!participant->hasVisibleEntity(other->getId()) && isVisible) {
            participant->addVisibleEntity(other->getId());
            publish<EntityVisibilityToParticipantData>({ other, participant->getId(), true });
        }
        else if(participant->hasVisibleEntity(other->getId()) && !isVisible) {
            participant->removeVisibleEntity(other->getId());
            publish<EntityVisibilityToParticipantData>({ other, participant->getId(), false });
        }
    }
}
