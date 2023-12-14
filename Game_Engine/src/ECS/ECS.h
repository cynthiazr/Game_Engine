#ifndef ECS_H
#define ECS_H
#include "../Logger/Logger.h"
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>

const unsigned int MAX_COMPONENTS = 32;

/// <summary>
/// Use a bitset (1 and 0) to know which components an entity has,
/// and helps to keep track of which entities a system is interested in.
/// </summary>
typedef std::bitset<MAX_COMPONENTS> Signature;

struct BaseComponent {
protected:
	static int nextID;
};

// Assigns a unique ID to a component type
template <typename T>
class Component: public BaseComponent {
public:
	// Return the unique ID of Component<T>
	static int GetID() {
		static auto ID = nextID++;
		return ID;

	}
};

class Entity {
private:
	int id;

public:
	Entity(int id) : id(id) {};
	Entity(const Entity& entity) = default;
	int GetID() const;

	Entity& operator = (const Entity& other) = default;
	bool operator == (const Entity& other) const { return id == other.id; }
	bool operator != (const Entity& other) const { return id != other.id; }
	bool operator > (const Entity& other) const { return id > other.id; }
	bool operator < (const Entity & other) const { return id < other.id; }

	template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename TComponent> void RemoveComponent();
	template <typename TComponent> bool HasComponent();
	template <typename TComponent> TComponent& GetComponent() const;
	
	// Hold a pointer to the entity's owner registry
	class Registry* registry;
};

/// <summary>
/// The system will processes entities that contain a specific signature
/// </summary>
class System {
private:
	Signature componentSignature;
	std::vector<Entity> entities;

public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	// Defines the component type that entities have to be considered by the system
	template <typename TComponent> void RequireComponent();
};

/// <summary>
/// A pool is just a vector of objects type T
/// </summary>
class BasePool {
public:
	virtual ~BasePool() {};
};

template <typename T>
class Pool: public BasePool {
private:
	std::vector<T> data;

public:
	Pool(int size = 100) {
		data.resize(size);
	}

	virtual ~Pool() = default;

	bool isEmpty() const {
		return data.empty();
	}

	int GetSize() const {
		return data.size();
	}

	void Resize(int n) {
		data.resize(n);
	}

	void Clear() {
		data.clear();
	}

	void Add(T object) {
		data.pushback(object);
	}

	void Set(int index, T object) {
		data[index] = object;
	}

	T& Get(int index) {
		return static_cast<T&>(data[index]);
	}

	T& operator [](unsigned int index) {
		return data[index];
	}
};

/// <summary>
/// The registry manages the creation and destruction of entities, as well as
/// adding systems and adding components to entities.
/// </summary>
class Registry {
private: 
	int numEntities = 0;

	// Vector of component pools, each pools contains all the data for a certain component type
	// Vector index = component type ID
	// Pool index = entity ID
	std::vector<std::shared_ptr<BasePool>> componentPools;

	// Vector of component signatues
	// The signature lets us know which components are turned "on" for an entity
	// [vector index = entity id]
	std::vector<Signature> entityComponentSignatures;

	// Map of active system [index = system typeID
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Set of entities that are flagged to be added/removed in the next Registry Update()
	std::set<Entity> entitiesToBeDestroyed;
	std::set<Entity> entitiesToBeAdded;

public:
	Registry() {
		Logger::Log("Registry constructor called");
	}

	~Registry() {
		Logger::Log("Registry destructor called");
	}

	// The Registry Update() processes the entities that are waiting to be added/removed
	void Update();

	// Entity Management
	Entity CreateEntity();

	// Component Management
	template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename TComponent> void RemoveComponent(Entity entity);
	template <typename TComponent> bool HasComponent(Entity entity);
	template <typename TComponent> TComponent& GetComponent(Entity entity) const;

	// System Management
	template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename TSystem> void RemoveSystem();
	template <typename TSystem> bool HasSystem() const;
	template <typename TSystem> TSystem& GetSystem() const;

	// Checks the component sugnature of an entity and add it to the systems
	// that are interested in the entity
	void AddEntityToSystems(Entity entity);
};

template <typename TComponent>
void System::RequireComponent() {
	const auto componentID = Component<TComponent>::GetID();
	componentSignature.set(componentID);
}

template <typename TComponent, typename ...TArgs> 
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();

	// If the componentID is greater than the current size of componentPools, then rsize the vector
	if (componentID >= componentPools.size()) {
		componentPools.resize(componentID + 1, nullptr);
	}

	// If we dont have a Pool for that component type
	if (!componentPools[componentID]) {
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentID] = newComponentPool;
	}

	// Get the pool of component values for that component type
	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentID]);

	// if the entityID is greater than the current size of the component pool, then resize the pool
	if (entityID >= componentPool->GetSize()) {
		componentPool->Resize(numEntities);
	}

	// new component object of the type T, and forward the various parameters to the constructor
	TComponent newComponent(std::forward<TArgs>(args)...);

	// add the new component to the component pool list
	componentPool->Set(entityID, newComponent);

	// change the component signature of the entity and set the componentID on the bitset to 1
	entityComponentSignatures[entityID].set(componentID);

	Logger::Log("Component ID = " + std::to_string(componentID) + " was added to entity ID " + std::to_string(entityID));
}

template <typename TComponent> 
void Registry::RemoveComponent(Entity entity) {
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();
	entityComponentSignatures[entityID].set(componentID, false);

	Logger::Log("Component ID = " + std::to_string(componentID) + " was removed from entity ID " + std::to_string(entityID));

}

template <typename TComponent> 
bool Registry::HasComponent(Entity entity) {
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();
	return entityComponentSignatures[entityID].test(componentID);
}

template <typename TComponent> 
TComponent& Registry::GetComponent(Entity entity) const {
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentID]);
	return componentPool->Get(entityID);
}

template <typename TSystem, typename ...TArgs> 
void Registry::AddSystem(TArgs&& ...args) {
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem> 
void Registry::RemoveSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem> 
bool Registry::HasSystem() const {
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> 
TSystem& Registry::GetSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() {
	registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
	return 	registry->GetComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() {
	return 	registry->HasComponent<TComponent>(*this);
}

#endif // !ECS_H