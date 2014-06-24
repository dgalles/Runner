//#pragma once
//#include "OgreString.h"
//#include "Windows.h"
//#include "MMSystem.h"
//
//#include <SDL.h>
//#include <SDL_mixer.h>
//#include <vector>
//class CSoundBank {
//    public:
//        static CSoundBank           SoundControl;
// 
//        std::vector<Mix_Chunk*>     SoundList;
// 
//    public:
//        CSoundBank();
// 
//        int OnLoad(char* File);
// 
//        void OnCleanup();
// 
//    public:
//        void Play(int ID);
//};
//
//
//class Sound
//{
//public:
//	Sound(void);
//
//
//	static void play(Ogre::String sound);
//	static void stopPlaying();
//
//	~Sound(void);
//};
//
#ifndef __SOUNDCHUNK_H
#define __SOUNDCHUNK_H

#include <SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <string>
#include <stdexcept>




class SoundChunk
{
public:
	SoundChunk(Mix_Chunk* c);
	SoundChunk(std::string path);
	~SoundChunk();
	void fadeIn(int ms, bool repeat);
	void fadeOut(int ms);
	void play();
protected:
	Mix_Chunk* mChunk;
	int mCurrentChanel;
};



class SoundBank {
public:
	static void shutdown();
	static SoundBank* getInstance();

	void setup();
	void play(std::string id);
	void addSound(SoundChunk* s, std::string id);
	void removeSound(std::string id);
	void openFile(std::string path, std::string id);
	void free();
	void fadeIn(std::string id, int ms, bool repeat);
    void fadeOut(std::string id, int ms);
	bool getEnableSound() { return mSoundEnabled; }
	void setEnableSound(bool newVal) { mSoundEnabled = newVal; }
	int getVolume() { return mVolume;}
	void setVolume(int newVol);

private:
	std::map<std::string, SoundChunk*> mChunks;
	static SoundBank* instance;
	SoundBank();
	~SoundBank();
	bool mHasBeenSetup;
	bool mSoundEnabled;
	int mVolume;
};

#endif