#include "ECS.h"
#include "../logger/logger.h"

int BaseComponent::nextID = 0;

int Entity::GetID() const {
	return id;
}

void System::AddEntityToSystem(Entity entity) {
	entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
	for (const auto& entityID = entities.begin(); entityID != entities.end();) {
		if (entityID->GetID() == entity.GetID()) {
			entities.erase(entityID);
			break;
		}
	}
}

std::vector<Entity> System::GetSystemEntities() const {
	return entities;
}

const Signature& System::GetComponentSignature() const {
	return componentSignature;
}

Entity Registry::CreateEntity() {
	int entityID;

	entityID = numEntities++;

	Entity entity(entityID);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	// check if the entityComponentSignature vector can accomodate the new entity
	if (entityID >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityID + 1);
	}

	Logger::Log("Entity created with ID = " + std::to_string(entityID));

	return entity;
}

void Registry::AddEntityToSystems(Entity entity) {
	const auto entityID = entity.GetID();

	const auto& entityComponentSignature = entityComponentSignatures[entityID];

	// loop all of the systems
	for (auto& system : systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested) {
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::Update() {
	// TODO: Add the entities that are waiting to be created
	for (auto entity : entitiesToBeAdded) {
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	// TODO: Add the entities that are waiting to be destroyed
}