#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

// Constructor
Game::Game() {
	isRunning = false;
	registry = std::make_unique<Registry>();
	assetManager = std::make_unique<AssetManager>();
	Logger::Log("Game constructor called!");
}

// Destructor
Game::~Game() {
	Logger::Log("Game destructor called!");
}

// Function that will set up the scene of the game
void Game::Start() {
	// If we can't initialize SDL, then display error message.
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Err("Error initializing SDL.");
		return;
	}

	// Create operating system window
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS
	);

	if (!window) {
		Logger::Err("Error creating SDL window.");
		return;
	}

	// Create operating system renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		Logger::Err("Error creating SDL renderer.");
		return;
	}

	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	// After it initializes SDL, create window and render, then it is ready to run
	isRunning = true;
}

// The function includes the loop
void Game::Run() {
	SetUp();

	// Game Loop
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput() {

	// pure struct
	SDL_Event sdlEvent;

	// not passing entire struct, just a reference (address)
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type) {
			// When user closes the window, isRunning is false to stop the game loop
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
				break;
		}
	}
}

void Game::LoadLevel(int level) {
	// Add the systems that need to be processed in our game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();

	// Add assets to the asset manager
	assetManager->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetManager->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetManager->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

	// Load the tilemap
	int tileSize = 32;
	double tileScale = 3.0;
	int mapNumCols = 25;
	int mapNumRows = 20;

	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>
				(glm::vec2(x * (tileScale * tileSize),
				y * (tileScale * tileSize)), 
				glm::vec2(tileScale, tileScale), 
				0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();


	// Create an Entity and Add components to that entity
	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(5.0, 5.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);

	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(5.0, 5.0), 90.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 30.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
}

// Initialize Game Objects positions, callers, etc. at the start of the game
void Game::SetUp() {
	LoadLevel(1);
}

void Game::Update() {
	// if we are too fast, waste some time until we reach the MILLISECS_PER_FRAME
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	// The difference in ticks since the last frame, converted to seconds
	double deltaTime = (SDL_GetTicks() - millisecPreviousFrame) / 1000.0;

	// Store the previous frame time	
	millisecPreviousFrame = SDL_GetTicks();

	// Update the registry to process the entities that are waiting to be created/deleted
	registry->Update();

	// Call all the systems that need to update
	registry->GetSystem<MovementSystem>().Update(deltaTime);

	// TODO: registry->GetSystem<CollisionSystem>().Update();
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// Call all the systems that need to render
	registry->GetSystem<RenderSystem>().Update(renderer, assetManager);

	SDL_RenderPresent(renderer);
}

// Destroy window, game objects
void Game::Stop() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}