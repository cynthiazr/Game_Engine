#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System {
public:
	MovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	// Logic that will be called frame by frame
	void Update(double deltaTime) {
		// Loop all entities that the system is interested in
		for (auto entity : GetSystemEntities()) {
			// Update entity position based on its velocity every frame of the  game
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			Logger::Log("Entity ID = " + std::to_string(entity.GetID()) 
				+ " position is not (" + std::to_string(transform.position.x) 
				+ ", " + std::to_string(transform.position.y) + ")"
			);
		}
	}
};

#endif // !MOVEMENTSYSTEM_H
