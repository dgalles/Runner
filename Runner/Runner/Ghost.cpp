#include "Ghost.h"
#include "RunnerObject.h"
#include "World.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
 #include <OgreTextAreaOverlayElement.h>


Ghost::Ghost(World *w) : mWorld(w)
{
	mSampleLength = 0.3;
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
		while (mDataindex < (int) mData.size() - 1 && mData[mDataindex+1].mTime <= mCurrentTime)
		{
			mDataindex++;
		}
		if (mDataindex >= (int) mData.size() - 1)
		{
			return;
		}
		float percent = (mCurrentTime - mData[mDataindex].mTime) / (mData[mDataindex+1].mTime - mData[mDataindex].mTime);
		float segmentDelta =  (mData[mDataindex+1].mSegment + mData[mDataindex+1].mPercent) - 
			                  ((mData[mDataindex].mSegment + mData[mDataindex].mPercent)) * percent;


		float prev = mData[mDataindex].mSegment + mData[mDataindex].mPercent;
		float next = mData[mDataindex+1].mSegment + mData[mDataindex+1].mPercent;

		float totalDelta = next - prev;
		segmentDelta = totalDelta * percent;

		int segment = mData[mDataindex].mSegment + (int) (segmentDelta + mData[mDataindex].mPercent);
		float segPercent = segmentDelta  +  mData[mDataindex].mPercent - (int) (segmentDelta  +  mData[mDataindex].mPercent);

		float xPos = mData[mDataindex].mXdelta + ( mData[mDataindex + 1].mXdelta -  mData[mDataindex].mXdelta) * percent;
		float lean = mData[mDataindex].mLean.valueDegrees() + ( mData[mDataindex + 1].mLean.valueDegrees() -  mData[mDataindex].mLean.valueDegrees()) * percent;

		float yPos = mData[mDataindex].mYDelta + ( mData[mDataindex + 1].mYDelta-  mData[mDataindex].mYDelta) * percent;
		
		Ogre::Vector3 pos;
		Ogre::Vector3 forward;
		Ogre::Vector3 right;
		Ogre::Vector3 up;
		mWorld->getWorldPositionAndMatrix(segment, segPercent, xPos, yPos, pos,forward, right, up, false);
		Ogre::Quaternion q(-right,up,forward);

		mRunnerObject->setOrientation(q);
		mRunnerObject->setPosition(pos);

		int deltaDist = (int) ((mData[mDataindex+1].mDistance - mData[mDataindex].mDistance) * percent);
		int dist = (int) (mData[mDataindex].mDistance + deltaDist);

		//mWorld->getHUD()->setSpeed( (int) ( mData[mDataindex].mSpeed + ( mData[mDataindex+1].mSpeed - mData[mDataindex].mSpeed ) * percent), true);
		//mWorld->getHUD()->setCoins((int) (mData[mDataindex].mCoins + (mData[mDataindex+1].mCoins - mData[mDataindex].mCoins) * percent), true);
		//mWorld->getHUD()->setDistance(dist, true);

		mWorld->getHUD()->setCoins(mData[mDataindex].mXdelta*10, true);
		mWorld->getHUD()->setDistance(mData[mDataindex+1].mXdelta*10, true);
		mWorld->getHUD()->setSpeed(xPos*10, true);

		//if (angle2 > Ogre::Degree(0) && mLeanEqualsDuck)
		//{
		//	mPlayerObject->pitch(Ogre::Radian(-angle2));
		//	mPlayerObject->setPosition(pos  + up *( - Ogre::Math::Sin(angle2) * mPlayerObject->minPointLocalScaled().z* 0.8f) + mDeltaY);
		//}


		mRunnerObject->roll(Ogre::Radian(Ogre::Degree(lean)));

	}
	else if (mPlayingBack)
	{


	}
}
void  Ghost::record(float time, int segment, float percent, float xdelta, float ydelta, Ogre::Degree lean, Ogre::Degree upDown,
					   int coins, int distance, int speed)
{
	if (mRecording)
	{
		if (mLastRecordTime + mSampleLength <= time)
		{
			mLastRecordTime = time;
			mData.push_back(GhostData(mLastRecordTime,segment,percent,xdelta,ydelta, lean, upDown, coins, distance, speed));
		}
				mCurrentTime = time;

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