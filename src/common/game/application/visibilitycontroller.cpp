#include "visibilitycontroller.h"

VisiblityController::VisiblityController()
{ }

void VisiblityController::initialise(ApplicationContext& context) {
    this->context = &context;
    initialised = true;
}

void VisiblityController::revealTiles(int participantId, const std::vector<glm::ivec2>& tiles) {
    std::vector<RevealedTile> tilesForEventData;

    auto grid = context->getGrid();

    for(auto& tile : tiles) {
        auto [t, isNew] = revealedTiles[participantId].insert(
            { tile.x, tile.y, true, grid->getTileAt(tile.x, tile.y) }
        );

        if(isNew) {
            tilesForEventData.push_back({ t->tile.id, t->tile.orientation, t->x, t->y });
        }
    }

    if(!tilesForEventData.empty()) {
        publish<TilesRevealedEventData>({ participantId, tilesForEventData });
    }
}

const std::map<int, std::set<VisiblityController::TileWithVisibility>>& VisiblityController::getTilesWithVisibility(void) const {
    return revealedTiles;
}

const std::set<VisiblityController::TileWithVisibility>& VisiblityController::getTilesWithVisibility(int participantId) {
    if(!revealedTiles.contains(participantId)) {
        revealedTiles[participantId] = std::set<TileWithVisibility>();
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

void VisiblityController::onPublish(const Event<ActorSetPositionEventData>& event) {
    auto& actor = context->getEntityRegistry().get<Actor>(event.data.entity);

    if(!actor.hasParticipant()) {
        return;
    }
    
    auto tiles = context->getGrid()->getVisibleTiles(
        glm::vec2(event.data.position.x, event.data.position.y),
        actor.getAggroRange()
    );

    auto participantId = actor.getParticipantId();

    revealTiles(participantId, tiles);
    visibleTiles[participantId] = std::unordered_set<glm::ivec2, glm::ivec2Hash>(tiles.begin(), tiles.end());

    auto const& actorPosition = context->getEntityRegistry().get<Position>(event.data.entity);
    
    for(auto [otherEntity, other, otherPosition, externalId] : 
        context->getEntityRegistry().view<Actor, Position, ExternalId>().each())
    {
        if(other.getParticipantId() == actor.getParticipantId()) {
            continue;
        }

        auto distance = glm::distance(glm::vec2(actorPosition), glm::vec2(otherPosition));

        assignVisibility(event.data.entity, otherEntity, distance, visibleTiles[participantId]);
        assignVisibility(otherEntity, event.data.entity, distance, visibleTiles[other.getParticipantId()]);
    }
}

void VisiblityController::assignVisibility(
    entt::entity actorEntity, 
    entt::entity otherEntity, 
    float distanceBetweenActors,
    const std::unordered_set<glm::ivec2, glm::ivec2Hash>& visibleTiles
) {
    auto& actor = context->getEntityRegistry().get<Actor>(actorEntity);
    auto const& otherPosition = context->getEntityRegistry().get<Position>(otherEntity);

    auto participant = context->getGameController()->getParticipant(actor.getParticipantId());
    
    bool isInRange = distanceBetweenActors < actor.getAggroRange();
    bool isInLOS = contains(visibleTiles, otherPosition);
    bool isVisible = isInRange && isInLOS;

    if(!participant->hasVisibleActor(otherEntity) && isVisible) {
        participant->addVisibleActor(otherEntity);
        publish<ActorVisibilityToParticipantData>({ otherEntity, participant->getId(), true });
    }
    else if(participant->hasVisibleActor(otherEntity) && !isVisible) {
        participant->removeVisibleActor(otherEntity);
        publish<ActorVisibilityToParticipantData>({ otherEntity, participant->getId(), false });
    }
}

bool VisiblityController::isVisible(entt::entity entity, entt::entity target) {
    auto& actor = context->getEntityRegistry().get<Actor>(entity);
    auto const& actorPosition = context->getEntityRegistry().get<Position>(entity);
    auto const& targetPosition = context->getEntityRegistry().get<Position>(target);
    
    auto distance = glm::distance(glm::vec2(actorPosition), glm::vec2(targetPosition));

    bool isInRange = distance < actor.getAggroRange();
    bool isInLOS = contains(visibleTiles[actor.getParticipantId()], targetPosition);
    
    return isInRange && isInLOS;
}
