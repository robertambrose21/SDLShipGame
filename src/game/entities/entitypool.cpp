#include "entitypool.h"

EntityPool::EntityPool() {
    loadEntityDefinitions();
}

void EntityPool::loadEntityDefinitions(void) {
    std::string directory = "../assets/data/entities";

    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::ifstream f(entry.path());
        json data = json::parse(f);

        EntityDefinition definition;
        definition.filename = entry.path();
        definition.name = data["name"].get<std::string>();
        definition.textureId = data["textureId"].get<uint8_t>();
        definition.movesPerTurn = data["movesPerTurn"].get<int>();
        definition.hp = data["hp"].get<int>();

        std::cout << "Loaded entity definition \"" << definition.name << "\"" << std::endl;

        entityDefinitions[definition.name] = definition;
    }
}

void EntityPool::updateEntities(const uint32_t& timeSinceLastFrame, bool& quit) {
    for(auto [entityId, entity] : entities) {
        updateEntity(entity, timeSinceLastFrame, quit);
    }

    for(auto entity : entitiesForDeletion) {
        entities.erase(entity->getId());
        std::cout << "[" << entity->getName() << "] died" << std::endl;
    }
    
    entitiesForDeletion.clear();
}

void EntityPool::updateEntity(std::shared_ptr<Entity> entity, const uint32_t& timeSinceLastFrame, bool& quit) {
    if(entity->getCurrentHP() <= 0) {
        entitiesForDeletion.insert(entity);
        return;
    }

    entity->update(timeSinceLastFrame, quit);
}

void EntityPool::drawEntities(std::shared_ptr<GraphicsContext> graphicsContext) {
    for(auto [entityId, entity] : entities) {
        entity->draw(graphicsContext);
    }
}

void EntityPool::synchronize(std::vector<GameStateUpdate> updates) {
    std::map<int, std::shared_ptr<Entity>> updatedEntities;

    for(auto update : updates) {
        for(int i = 0; i < update.numEntities; i++) {
            auto entityUpdate = update.entities[i];
            auto existing = entities.contains(entityUpdate.id) ? entities[entityUpdate.id] : nullptr;

            entities[entityUpdate.id] = EntityStateUpdate::deserialize(entityUpdate, existing);

            if(entityUpdate.currentHP <= 0) {
                entitiesForDeletion.insert(entities[entityUpdate.id]);
            } else {
                updatedEntities[entityUpdate.id] = entities[entityUpdate.id];
            }
        }
    }

    // Remove any entities which weren't present in the updates
    for(auto [entityId, entity] : entities) {
        if(!updatedEntities.contains(entityId)) {
            entitiesForDeletion.insert(entity);
        }
    }
}

std::shared_ptr<Entity> EntityPool::addEntity(std::shared_ptr<Entity> entity) {
    if(entities.contains(entity->getId())) {
        throw std::runtime_error(
            "Entity " + 
            entity->getName() + 
            "#" + 
            std::to_string(entity->getId()) + 
            " already exists, cannot add"
        );
    }

    entities[entity->getId()] = entity;
    return entity;
}

std::shared_ptr<Entity> EntityPool::addEntity(const std::string& name) {
    if(!entityDefinitions.contains(name)) {
        throw std::runtime_error("Could not find entity definition with name " + name);
    }

    auto definition = entityDefinitions[name];
    auto entity = std::make_shared<Entity>(definition.name,
        Entity::Stats {
            definition.movesPerTurn,
            definition.hp
        });
    entity->setTextureId(definition.textureId);
    entity->setSelectedTextureId(6);

    return addEntity(entity);
}

std::map<uint32_t, std::shared_ptr<Entity>> EntityPool::getEntities(void) {
    return entities;
}

std::shared_ptr<Entity> EntityPool::getEntity(const uint32_t& id) {
    if(!entities.contains(id)) {
        // TODO: Throw exception
        return nullptr;
    }

    return entities[id];
}
