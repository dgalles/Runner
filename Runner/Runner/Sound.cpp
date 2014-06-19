#include "Sound.h"
#include "OgreResourceGroupManager.h"


#include "SDL.h"
#include "SDL_audio.h"

Sound::Sound(void)
{
}



void Sound::play(Ogre::String sound)
{

	std::string foundPath = sound;
	Ogre::ResourceGroupManager* groupManager = Ogre::ResourceGroupManager::getSingletonPtr() ;
	Ogre::String group = groupManager->findGroupContainingResource(sound) ;
	Ogre::FileInfoListPtr fileInfos = groupManager->findResourceFileInfo(group,foundPath);
	Ogre::FileInfoList::iterator it = fileInfos->begin();
	if(it != fileInfos->end())
	{
		foundPath = it->archive->getName() + "/" + foundPath;
	}
	else
	{
		foundPath = "";
	}

	PlaySound(TEXT(foundPath.c_str()),NULL,SND_ASYNC);

}

void Sound::stopPlaying()
{
	PlaySound(NULL,NULL,SND_ASYNC);

}
Sound::~Sound(void)
{
}
