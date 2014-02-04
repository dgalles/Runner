#include "AIManager.h"
#include "World.h"

AIManager::AIManager(World *world) : mWorld(world)
{
    // Any initialization that needs to be done ...
}

AIManager::~AIManager()
{
    // Clean up after yourself ...
}

void 
AIManager::Think(float time)
{
	// Do any "Global" AI thinking, and then call think for all of the actual AI's running about
}

