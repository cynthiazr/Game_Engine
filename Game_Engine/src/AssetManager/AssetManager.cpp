#include "AssetManager.h"
#include "../Logger/Logger.h"
#include "SDL_image.h"

AssetManager::AssetManager() {
	Logger::Log("AssetManager constructor called!");
}

AssetManager::~AssetManager() {
	ClearAssets();
	Logger::Log("AssetManager destructor called!");
}

void AssetManager::ClearAssets() {
	for (auto texture : textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& assetID, const std::string& filePath) {
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// add the texture to the map
	textures.emplace(assetID, texture);

	Logger::Log("New texture added to the Asset Manager with ID = " + assetID);
}

SDL_Texture* AssetManager::GetTexture(const std::string& assetID) {
	return textures[assetID];
}