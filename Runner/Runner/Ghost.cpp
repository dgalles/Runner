#include "Ghost.h"
#include "RunnerObject.h"
#include "World.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include <OgreTextAreaOverlayElement.h>
#include <iostream>
#include <fstream>

Ghost::Ghost(World *w) : mWorld(w)
{
	mSampleLength = 0.1f;
	mPlayingBack= false;
	mGhostInfo = new Ghost::GhostInfo();
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
	// mCurrentTime += time;


}

void Ghost::updateGhost(float time)
{
	if (mAlive)
	{

		while (mDataindex < (int) mData.size() - 1 && mData[mDataindex+1].mTime <= time)
		{
			mDataindex++;
		}
		if (mDataindex >= (int) mData.size() - 1)
		{
			mWorld->getHUD()->setCoins(mData[mData.size() - 1].mCoins, true);
			mWorld->getHUD()->setDistance(mData[mData.size() - 1].mDistance, true);
			return;
		}


		float percent = (time - mData[mDataindex].mTime) / (mData[mDataindex+1].mTime - mData[mDataindex].mTime);

		percent = std::min(percent, 1.0f);
		percent = std::max(percent, 0.0f);


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

		float lean2 =  mData[mDataindex].mUpDown.valueDegrees() + ( mData[mDataindex + 1].mUpDown.valueDegrees() -  mData[mDataindex].mUpDown.valueDegrees()) * percent;

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

		mWorld->getHUD()->setSpeed( (int) ( mData[mDataindex].mSpeed + ( mData[mDataindex+1].mSpeed - mData[mDataindex].mSpeed ) * percent), true);
		mWorld->getHUD()->setCoins((int) (mData[mDataindex].mCoins + (mData[mDataindex+1].mCoins - mData[mDataindex].mCoins) * percent), true);
		mWorld->getHUD()->setDistance(dist, true);


		if (lean2 > 0 && mGhostInfo->mLeanEqualsDuck)
		{
			mRunnerObject->pitch(Ogre::Radian(Ogre::Degree(-lean2)));
			mRunnerObject->setPosition(pos  + up *( - Ogre::Math::Sin(Ogre::Degree(lean2)) * mRunnerObject->minPointLocalScaled().z* 0.8f) + yPos);
		}

		if (lean2 <  0 &&  mGhostInfo->mLeanEqualsDuck)
		{
			float diff = (lean2 / 10) + 6;
			diff = std::max(diff, 0.5f);
			mRunnerObject->setScale(Ogre::Vector3(5,diff,10));
		}


		mRunnerObject->roll(Ogre::Radian(Ogre::Degree(lean)));

	}
	else  // !mAlive 
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
	else if (mPlayingBack)
	{
		updateGhost(time);


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

	//mRunnerObject->setAlpha(0.0);
	//mRunnerObject->setAlpha(0.3f);
	mPlayingBack = true;
	mAlive = true;
}

void  Ghost::readFile(std::string filename)
{
	std::ifstream myfile (filename, std::ios::in );


	myfile >> mGhostInfo->mInitialSpeed;
	myfile >> mGhostInfo->mInitialSpeed;
	myfile >> mGhostInfo->mMaxSpeed;
	myfile >> mGhostInfo->mUseFrontBack;
	myfile >> mGhostInfo->mAutoAceelerateRate;
	myfile >> mGhostInfo->mManualAceelerateRate;
	myfile >> mGhostInfo->mInitialArmor;
	myfile >> mGhostInfo->mBoostDuration;
	myfile >> mGhostInfo->mShieldDuration;
	myfile >> mGhostInfo->mMagnetDuration;
	myfile >> mGhostInfo->mObsGap;
	myfile >> mGhostInfo->mObjsFreq;
	myfile >> mGhostInfo->mBoostFreq;
	myfile >> mGhostInfo->mShieldFreq;
	myfile >> mGhostInfo->mMagnetFreq;
	myfile >> mGhostInfo->mSeed;
	myfile >> mGhostInfo->mLeanEqualsDuck;

	int size;
	myfile >> size;

	GhostData nextData;

	mData.clear();

	for (int i = 0; i < size; i++)
	{
		myfile >> nextData.mTime;
		myfile >> nextData.mSegment;
		myfile >> nextData.mPercent;
		myfile >> nextData.mXdelta;
		myfile >> nextData.mYDelta;
		float upDown, lean;
		myfile >> upDown;
		myfile >> lean;
		nextData.mUpDown = Ogre::Degree(upDown);
		nextData.mLean = Ogre::Degree(lean);
		myfile >> nextData.mCoins;
		myfile >> nextData.mDistance;
		myfile >> nextData.mSpeed;
		
		mData.push_back(nextData);
	}

}
void  Ghost::writeFile(std::string filename)
{
	std::ofstream myfile (filename, std::ios::out | std::ios::trunc);


	myfile << mGhostInfo->mInitialSpeed << " ";
	myfile << mGhostInfo->mInitialSpeed << " ";
	myfile << mGhostInfo->mMaxSpeed << " ";
	myfile << mGhostInfo->mUseFrontBack << " ";
	myfile << mGhostInfo->mAutoAceelerateRate << " ";
	myfile << mGhostInfo->mManualAceelerateRate << " ";
	myfile << mGhostInfo->mInitialArmor << " ";
	myfile << mGhostInfo->mBoostDuration << " ";
	myfile << mGhostInfo->mShieldDuration << " ";
	myfile << mGhostInfo->mMagnetDuration << " ";
	myfile << mGhostInfo->mObsGap << " ";
	myfile << mGhostInfo->mObjsFreq << " ";
	myfile << mGhostInfo->mBoostFreq << " ";
	myfile << mGhostInfo->mShieldFreq << " ";
	myfile << mGhostInfo->mMagnetFreq << " ";
	myfile << mGhostInfo->mSeed << " ";
	myfile << mGhostInfo-> mLeanEqualsDuck << " ";




	myfile << mData.size() << " ";

	for (int i = 0; i < mData.size(); i++)
	{
		myfile << mData[i].mTime << " ";
		myfile << mData[i].mSegment << " ";
		myfile << mData[i].mPercent << " ";
		myfile << mData[i].mXdelta << " ";
		myfile << mData[i].mYDelta << " ";
		myfile << mData[i].mUpDown.valueDegrees() << " ";
		myfile << mData[i].mLean.valueDegrees() << " ";
		myfile << mData[i].mCoins << " ";
		myfile << mData[i].mDistance << " ";
		myfile << mData[i].mSpeed << " ";
	}

}


void Ghost::playerDead(int distance, int coins)
{
	if (mRecording)
	{
		GhostData g = mData[mData.size() - 1];
		g.mCoins = coins;
		g.mDistance = distance;
		mData.push_back(g);
	}
	else if (mPlayingBack)
	{
		mWorld->getHUD()->setCoins(mData[mData.size() - 1].mCoins, true);
		mWorld->getHUD()->setDistance(mData[mData.size() - 1].mDistance, true);
	}
}