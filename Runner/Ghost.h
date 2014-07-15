#pragma once
#include <vector>
#include "OgreMath.h"

class RunnerObject;
class World;

class Ghost
{
public:
	Ghost(World *w);
	~Ghost(void);

	void think(float time);
	void record(float time, int segment, float percent, float xdelta, float ydelta, Ogre::Degree lean, Ogre::Degree upDown);
	void kill(float time);
	void startRecording();
	void stopRecording();
	void startPlayback();
	void stopPlayback();
	void setSeed(int seed) { mSeed = seed;}
	int getSeed() { return mSeed; }

	void readFile(std::string filename);
	void writeFile(std::string filename);

	void setSampleLength(float newLength) {mSampleLength = newLength; }

protected:

	class GhostData
	{
	public:
		GhostData();

		GhostData(float time, int segment, float percent, float xdelta, float ydelta, Ogre::Degree lean, Ogre::Degree upDown):
			mTime(time), mSegment(segment), mPercent(percent), mXdelta(xdelta), mYDelta(ydelta), mLean(lean), mUpDown(upDown) {}

		float mTime;
		int mSegment;
		float mPercent;
		float mXdelta;
		float mYDelta;
		Ogre::Degree mUpDown;
		Ogre::Degree mLean;


	};

	bool mPlayingBack;
	bool mRecording;
	float mCurrentTime;
	float mLastRecordTime;
	int mSeed;
	RunnerObject *mRunnerObject;
	float mKillTime;
	float mSampleLength;
	bool mAlive;
	int mDataindex;
	std::vector<GhostData> mData;
	World *mWorld;

};

