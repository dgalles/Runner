#include "Ghost.h"
#include "RunnerObject.h"
#include "World.h"

Ghost::Ghost(World *w) : mWorld(w)
{
	mSampleLength = 1.0f;
	mPlayingBack= false;
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
	void Ghost::kill(float time)
	{
		mKillTime = mCurrentTime + time;
	}


void Ghost::think(float time)
{
	mCurrentTime += time;

	if (mPlayingBack && mAlive)
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
		float lean = mData[mDataindex].mLean.valueDegrees() + ( mData[mDataindex + 1].mLean.valueDegrees() -  mData[mDataindex].mLean.valueDegrees()) * segPercent;

		float yPos = mData[mDataindex].mYDelta + ( mData[mDataindex + 1].mYDelta-  mData[mDataindex].mYDelta) * segPercent;
		
		Ogre::Vector3 pos;
		Ogre::Vector3 forward;
		Ogre::Vector3 right;
		Ogre::Vector3 up;
		segment++;
		mWorld->getWorldPositionAndMatrix(segment, segPercent, xPos, yPos, pos,forward, right, up, false);
		Ogre::Quaternion q(-right,up,forward);

		mRunnerObject->setOrientation(q);
		mRunnerObject->setPosition(pos);

		//if (angle2 > Ogre::Degree(0) && mLeanEqualsDuck)
		//{
		//	mPlayerObject->pitch(Ogre::Radian(-angle2));
		//	mPlayerObject->setPosition(pos  + up *( - Ogre::Math::Sin(angle2) * mPlayerObject->minPointLocalScaled().z* 0.8f) + mDeltaY);
		//}


		mRunnerObject->roll(Ogre::Radian(lean));

	}
	else if (mPlayingBack)
	{


	}
}
void  Ghost::record(float time, int segment, float percent, float xdelta, float ydelta, Ogre::Degree lean, Ogre::Degree upDown)
{
	if (mRecording)
	{
		if (mLastRecordTime + mSampleLength <= mCurrentTime)
		{
			mLastRecordTime = mCurrentTime;
			mData.push_back(GhostData(mLastRecordTime,segment,percent,xdelta,ydelta, lean, upDown));
		}
				mCurrentTime += time;

	}
}


void  Ghost::startRecording()
{
	mData.clear();
	mCurrentTime = 0;
	mLastRecordTime = -3;
	mRecording = true;
}
void  Ghost::startPlayback()
{
	mDataindex = 0;
	mCurrentTime = 0;
	mRunnerObject = new RunnerObject(RunnerObject::ObjectType::PLAYER);
	mRunnerObject->loadModel("car.mesh", mWorld->SceneManager());
	mRunnerObject->setScale(Ogre::Vector3(5,6,10));
	mRunnerObject->setMaterial("ghostMaterial"); 
	// mRunnerObject->setAlpha(0.0);
	mRunnerObject->setAlpha(0.3f);
	mPlayingBack = true;
	mAlive = true;
}

void  Ghost::readFile(std::string filename)
{

}
void  Ghost::writeFile(std::string filename)
{


}