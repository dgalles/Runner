#pragma once
#include "OgreString.h"
#include "Windows.h"
#include "MMSystem.h"

class Sound
{
public:
	Sound(void);


	static void play(Ogre::String sound);
	static void stopPlaying();

	~Sound(void);
};

