#include "Ghost.h"
#include "RunnerObject.h"
#include "World.h"

Ghost::Ghost(World *w) : mWorld(w)
{
	mRunnerObject = new RunnerObject(RunnerObject::ObjectType::PLAYER);
	mRunnerObject->loadModel("car.mesh", mWorld->SceneManager());
	mRunnerObject->setScale(Ogre::Vector3(5,6,10));
	// mRunnerObject->setMaterial(); TODO:  Set this to something else!
	mRunnerObject->setAlpha(0.0);
	mSampleLength = 1.0f;
}



void Ghost::stopRecording()
{
	mRecording = false;
}
void Ghost::stopPlayback()
{
	mPlayingBack = false;
	mRunnerObject->setAlpha(0);
}

Ghost::~Ghost(void)
{
}


void Ghost::think(float time)
{
	mCurrentTime += time;

	if (mPlayingBack)
	{
		while (mDataindex < (int) mData.size() + 1 && mData[mDataindex+1].mTime < mCurrentTime)
		{
			mDataindex++;
		}
		if (mDataindex >= (int) mData.size() - 1)
		{
			return;
		}
		float percent = (mCurrentTime - mData[mDataindex].mTime) / (mData[mDataindex+1].mTime - mData[mDataindex].mTime);
		float segmentDelta =  (mData[mDataindex+1].mSegment + mData[mDataindex+1].mPercent) - 
			                  ((mData[mDataindex].mSegment + mData[mDataindex].mPercent));

		int segment = mData[mDataindex].mSegment + (int) segmentDelta;
		float segPercent = segmentDelta - (int) segmentDelta;

		float xPos = mData[mDataindex].mXdelta + ( mData[mDataindex + 1].mXdelta -  mData[mDataindex].mXdelta) * segPercent;
		float lean = mData[mDataindex].mLean + ( mData[mDataindex + 1].mLean -  mData[mDataindex].mLean) * segPercent;

	}
}
void  Ghost::record(float time, int segment, float percent, float xdelta, float lean)
{
	mCurrentTime += time;
	if (mLastRecordTime + mSampleLength <= mCurrentTime)
	{
		mLastRecordTime = mCurrentTime;
		mData.push_back(GhostData(mLastRecordTime,segment,percent,xdelta,lean));
	}
}


void  Ghost::startRecording()
{
	mData.clear();
	mCurrentTime = 0;
	mLastRecordTime = -3;
}
void  Ghost::startPlayback()
{
	mDataindex = 0;
	mCurrentTime = 0;
	mRunnerObject->setAlpha(0.3f);
}

void  Ghost::readFile(std::string filename)
{

}
void  Ghost::writeFile(std::string filename)
{


}