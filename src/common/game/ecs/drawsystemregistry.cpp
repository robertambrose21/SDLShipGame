#include "drawsystemregistry.h"

DrawSystem::DrawSystem(const std::string& name) :
    System(name)
{ }



DrawSystemRegistry::DrawSystemRegistry(entt::registry& registry) :
    SystemRegistry(registry)
{ }

void DrawSystemRegistry::draw(GraphicsContext& graphicsContext) {
    for(const auto& [name, system] : getAllRegisteredSystems()) {
        system->draw(getRegistry(), graphicsContext);
    }
}