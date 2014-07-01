 #include "OgreResourceGroupManager.h"


#include "Sound.h"
#include "SDL.h"

SoundChunk::SoundChunk(Mix_Chunk* c)
{
	mChunk = c;
}

SoundChunk::SoundChunk(std::string path)
{
	mCurrentChanel = -1;
	if((mChunk = Mix_LoadWAV(path.c_str())) == NULL)
	{
		// Should probably throw here ...
		int x = 0;
		x++;
	}
}

SoundChunk::~SoundChunk()
{
	Mix_FreeChunk(mChunk);
}

void SoundChunk::play()
{
	mCurrentChanel = Mix_PlayChannel(-1, mChunk, 0);
}


void SoundChunk::fadeIn(int ms, bool repeat)
{
	// Don't start playing if we are already playing this chanel ...
	if (mCurrentChanel < 0)
	{
	if (repeat)
	{
		 mCurrentChanel = Mix_FadeInChannel(-1,mChunk,-1, ms);
	}
	else
	{
		 mCurrentChanel = Mix_FadeInChannel(-1,mChunk, 0, ms);
	}
	}
}

void SoundChunk::fadeOut(int ms)
{
	if (mCurrentChanel >= 0)
	{
		Mix_FadeOutChannel(mCurrentChanel, ms);
		mCurrentChanel = -1;
	}
}


SoundBank* SoundBank::instance =NULL;


void SoundBank::setVolume(int newVol)
{
	if (newVol > 128)
	{
		newVol = 128;
	}
	if (newVol < 0)
	{
		newVol = 0;
	}
	Mix_Volume(-1, newVol);
	mVolume = newVol;

}

SoundBank::SoundBank() : mChunks()
{
	mHasBeenSetup= false;
	mSoundEnabled = true;
	mSoundIndex = 0;
	mNumSounds = 1;
	mVolume = 100;
}

SoundBank::~SoundBank()
{
	this->free();
}



void SoundBank::setup()
{
	SDL_Init(SDL_INIT_AUDIO);



	// load support for the OGG and MOD sample/music formats
	int flags=MIX_INIT_OGG|MIX_INIT_MP3;
	int initted=Mix_Init(flags);
	if(initted != flags) {
		std::string error = Mix_GetError();
		int x = 0;
		x = x +1;
	}


	mHasBeenSetup = true;
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	// TODO:  The following should all be read in from a file in Content.
	///       not hardcorded.

	mNumSounds = 2;

	std::map<std::string, SoundChunk*> nextChunk;

	for (int i = 0; i < mNumSounds; i++)
	{
		mChunks.push_back(nextChunk);
	}


	openFile("BuzzsawRight1.wav", "left", 0);
	openFile("BuzzsawLeft1.wav", "right", 0);
	openFile("BuzzsawCenter1.wav", "center", 0);
	openFile("BuzzsawCenterHigh1.wav", "down", 0);
	openFile("empty.wav", "up", 0);
	openFile("coin.wav", "coin", 0);
	openFile("crash.wav", "crash", 0);

	openFile("bellRight.wav", "left", 1);
	openFile("bellLeft.wav", "right", 1);
	openFile("bellCenter.wav", "center", 1);
	openFile("descending.wav", "down", 1);
	openFile("ascending.wav", "up", 1);
	openFile("coin.wav", "coin", 1);
	openFile("crash.wav", "crash", 1);



	Mix_Volume(-1, mVolume);

}


void SoundBank::shutdown()
{
	delete instance;
	Mix_CloseAudio();
	SDL_Quit();
	instance = NULL;
}
SoundBank* SoundBank::getInstance()
{
	if (SoundBank::instance == NULL)
	{
		instance = new SoundBank();
	}
	return SoundBank::instance;
}

void SoundBank::free()
{
	for (unsigned int i = 0; i < mChunks.size(); i++)
	{
		std::map<std::string, SoundChunk*>::iterator iter = mChunks[i].begin();
		while(iter != mChunks[i].end())
		{
			delete(iter->second);
			iter++;
		}
		mChunks[i].clear();
	}
	mChunks.clear();
}

void SoundBank::play(std::string id)
{
	if(mSoundEnabled)
	{
		mChunks[mSoundIndex][id]->play();
	}
}

void SoundBank::fadeIn(std::string id, int ms, bool repeat)
{
	if (mSoundEnabled)
	{
		mChunks[mSoundIndex][id]->fadeIn(ms, repeat);
	}
}

void SoundBank::fadeOut(std::string id, int ms)
{
	mChunks[mSoundIndex][id]->fadeOut(ms);
}

void SoundBank::addSound(SoundChunk* s, std::string id, int index)
{
	mChunks[index][id] = s;
}

void SoundBank::removeSound(std::string id){
	//  TODO:: Do removechunks->erase(id);
}

void SoundBank::openFile(std::string path, std::string id, int index)
{
	std::string foundPath = path;
	Ogre::ResourceGroupManager* groupManager = Ogre::ResourceGroupManager::getSingletonPtr() ;
	Ogre::String group = groupManager->findGroupContainingResource(path) ;
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

	this->addSound(new SoundChunk(foundPath), id, index);
}
