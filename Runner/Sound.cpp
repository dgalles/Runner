//
//#include "SDL.h"
//#include "SDL_audio.h"
//0
//#include "Sound.h"
 #include "OgreResourceGroupManager.h"
//
//CSoundBank CSoundBank::SoundControl;
// 
//CSoundBank::CSoundBank() {
//}
// 
//int CSoundBank::OnLoad(char* File) {
//    Mix_Chunk* TempSound = NULL;
// 
//    if((TempSound = Mix_LoadWAV(File)) == NULL) {
//        return -1;
//    }
// 
//    SoundList.push_back(TempSound);
// 
//    return (SoundList.size() - 1);
//}
// 
//void CSoundBank::OnCleanup() {
//    for(int i = 0;i < SoundList.size();i++) {
//        Mix_FreeChunk(SoundList[i]);
//    }
// 
//    SoundList.clear();
//}
// 
//void CSoundBank::Play(int ID) {
//    if(ID < 0 || ID >= SoundList.size()) return;
//    if(SoundList[ID] == NULL) return;
// 
//    Mix_PlayChannel(-1, SoundList[ID], 0);
//}
//
//
//
//
//
//
//Sound::Sound(void)
//{
//}
//
//
//
//void Sound::play(Ogre::String sound)
//{
//
//	std::string foundPath = sound;
//	Ogre::ResourceGroupManager* groupManager = Ogre::ResourceGroupManager::getSingletonPtr() ;
//	Ogre::String group = groupManager->findGroupContainingResource(sound) ;
//	Ogre::FileInfoListPtr fileInfos = groupManager->findResourceFileInfo(group,foundPath);
//	Ogre::FileInfoList::iterator it = fileInfos->begin();
//	if(it != fileInfos->end())
//	{
//		foundPath = it->archive->getName() + "/" + foundPath;
//	}
//	else
//	{
//		foundPath = "";
//	}
//
//	PlaySound(TEXT(foundPath.c_str()),NULL,SND_ASYNC);
//
//}
//
//void Sound::stopPlaying()
//{
//	PlaySound(NULL,NULL,SND_ASYNC);
//
//}
//Sound::~Sound(void)
//{
//}

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
	if (repeat)
	{
		 mCurrentChanel = Mix_FadeInChannel(-1,mChunk,-1, ms);
	}
	else
	{
		 mCurrentChanel = Mix_FadeInChannel(-1,mChunk, 0, ms);
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


	openFile("BuzzsawRight1.wav", "BuzzsawRight1");
	openFile("BuzzsawLeft1.wav", "BuzzsawLeft1");
	openFile("BuzzsawCenter1.wav", "BuzzsawCenter1");
	openFile("BuzzsawCenterHigh1.wav", "BuzzsawCenterHigh1");
	openFile("coin.wav", "coin");
	openFile("crash.wav", "crash");
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
	std::map<std::string, SoundChunk*>::iterator iter = mChunks.begin();
	while(iter != mChunks.end()){
		delete(iter->second);
		iter++;
	}
	mChunks.clear();
}

void SoundBank::play(std::string id)
{
	if(mSoundEnabled)
	{
		mChunks[id]->play();
	}
}

void SoundBank::fadeIn(std::string id, int ms, bool repeat)
{
	if (mSoundEnabled)
	{
		mChunks[id]->fadeIn(ms, repeat);
	}
}

void SoundBank::fadeOut(std::string id, int ms)
{
	mChunks[id]->fadeOut(ms);
}

void SoundBank::addSound(SoundChunk* s, std::string id)
{
	mChunks[id] = s;
}

void SoundBank::removeSound(std::string id){
	//  TODO:: Do removechunks->erase(id);
}

void SoundBank::openFile(std::string path, std::string id)
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

	this->addSound(new SoundChunk(foundPath), id);
}
