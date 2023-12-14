#ifndef GAME_H
#define GAME_H
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include <SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
private:
	bool isRunning;
	int millisecPreviousFrame = 0;
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetManager> assetManager;

public:
	Game();
	~Game();
	void Start();
	void SetUp();
	void ProcessInput();
	void LoadLevel(int level);
	void Update();
	void Render();
	void Run();
	void Stop();

	int windowWidth;
	int windowHeight;
};

#endif // !GAME_H