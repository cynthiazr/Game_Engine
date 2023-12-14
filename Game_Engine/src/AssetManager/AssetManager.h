#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <map>
#include <string>
#include <SDL.h>

class AssetManager {
private:
	std::map<std::string, SDL_Texture*> textures;
	// TODO: create a map for fonts
	// TODO: create a map for audio

public:
	AssetManager();
	~AssetManager();

	void ClearAssets();
	void AddTexture(SDL_Renderer* renderer, const std::string& assetID, const std::string& filePath);
	SDL_Texture* GetTexture(const std::string& assetID);
};

#endif // !ASSETMANAGER_H
