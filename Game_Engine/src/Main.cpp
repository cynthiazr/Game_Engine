#include "./Game/Game.h"


int main(int argc, char* argv[]) { // Used if parameters are sent from the operating system to the program
	Game game;

	game.Start();
	game.Run();
	game.Stop();

	return 0;
}