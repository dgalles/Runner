#pragma once
#include <vector>

class RunnerObject;
class World;

class Ghost
{
public:
	Ghost(World *w);
	~Ghost(void);

	void think(float time);
	void record(float time, int segment, float percent, float xdelta, float lean);
	void startRecording();
	void stopRecording();
	void startPlayback();
	void stopPlayback();
	void setSeed(float seed) { mSeed = seed;}
	float getSeed() { return mSeed; }

	void readFile(std::string filename);
	void writeFile(std::string filename);

	void setSampleLength(float newLength) {mSampleLength = newLength; }

protected:

	class GhostData
	{
	public:
		GhostData();

		GhostData(float time, int segment, float percent, float xdelta, float lean):
			mTime(time), mSegment(segment), mPercent(percent), mXdelta(xdelta), mLean(lean) {}

		float mTime;
		int mSegment;
		float mPercent;
		float mXdelta;
		float mLean;


	};

	bool mPlayingBack;
	bool mRecording;
	float mCurrentTime;
	float mLastRecordTime;
	float mSeed;
	RunnerObject *mRunnerObject;

	float mSampleLength;

	int mDataindex;
	std::vector<GhostData> mData;
	World *mWorld;

};

