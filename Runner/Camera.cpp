#include "OgreCamera.h"
#include "Camera.h"
#include "World.h"
#include "Bezier.h"
#include "OgreVector3.h"
#include "Player.h"

RunnerCamera::RunnerCamera(Ogre::Camera *renderCamera, World *world) :
mRenderCamera(renderCamera), mWorld(world), mCurrentTrackingObject(0), mReview(false)
{
	currentSegment = 0;
	currentPercent = 0.3f;
	direction = 1;
	mRenderCamera->setNearClipDistance(2);
	mType = FollowType::NORMAL;

	mDesiredFollowDistance = 200;
	mDesiredFollowHeight = 50;

	mCurrentFollowDistance = mDesiredFollowDistance;
	mCurrentFollowHeight = mDesiredFollowHeight;

	mShakeTime = 0;
	// Any other initialization that needs to happen
}


void
	RunnerCamera::SetFollowType(FollowType typ)
{
	if (typ == FollowType::NORMAL)
	{
		mDesiredFollowDistance = 200;
		mDesiredFollowHeight = 50;

	}
	else if (typ == FollowType::CLOSE)
	{
		mDesiredFollowDistance = 150;
		mDesiredFollowHeight = 15;

	}
	else if (typ = FollowType::HIGH)
	{
		mDesiredFollowDistance = 200;
		mDesiredFollowHeight = 80;
	}
}


void 
RunnerCamera::Pause()
{
	mPaused = true;
}
void 
RunnerCamera::UnPause()
{
	mPaused = false;
}

void
RunnerCamera::TrackObject(TrackableObject *objectToTrack)
{
	mCurrentTrackingObject = objectToTrack;

}

void RunnerCamera::Shake(float time)
{
	mShakeTime = time;
	mShakeRoll = 0;
	mShakePitch = 0;
	mShakeOffset = Ogre::Vector3::ZERO;
}


void
RunnerCamera::Think(float time)
{
	if (mCurrentTrackingObject != 0)
	{

		int segment = mCurrentTrackingObject->currentSegment();
		float percent = mCurrentTrackingObject->segmentPercent();
		Ogre::Vector3 trackPos = mCurrentTrackingObject->worldPosition();
		Ogre::Vector3 cameraPos;
		Ogre::Vector3 cameraForward;
		Ogre::Vector3 cameraRight;
		Ogre::Vector3 cameraUp;


		float cameraSpeed = 100;


		float horzDelta = mDesiredFollowDistance - mCurrentFollowDistance;
		float vertDelta = mDesiredFollowHeight - mCurrentFollowHeight;

		float vertSpeed;
		float horzSpeed;

		if (abs(horzDelta) > abs(vertDelta))
		{
			horzSpeed =cameraSpeed;
			vertSpeed = cameraSpeed * abs(vertDelta / horzDelta);
		}
		else
		{
			vertSpeed = cameraSpeed;
			if (vertDelta != 0)
			{
				horzSpeed = cameraSpeed * abs(horzDelta / vertDelta);
			}
			else
			{
				horzSpeed = cameraSpeed;
			}
		}

		if ((vertSpeed * time) > abs(mDesiredFollowHeight - mCurrentFollowHeight))
		{
			mCurrentFollowHeight = mDesiredFollowHeight;
			mCurrentFollowDistance = mDesiredFollowDistance;
		}
		else
		{
			mCurrentFollowHeight += vertSpeed * Ogre::Math::Sign(vertDelta) * time;
			mCurrentFollowDistance += horzSpeed * Ogre::Math::Sign(horzDelta) * time;
		}

		float size = mWorld->trackPath->pathLength(segment);
		float fDist = mCurrentFollowDistance;
		while (size * percent < fDist)
		{
			fDist -= size*percent;
			segment -= 1;
			percent = 1;
			if (segment < 0)
			{
				break;
			}
			size = mWorld->trackPath->pathLength(segment);
		}
		

		float cameraPercent		= percent - fDist / size;
		
		if (segment < 0)
		{
			segment = 0;
			cameraPercent = 0;
		}
		
		if (mReview)
		{
			segment++;
		}


		mWorld->trackPath->getPointAndRotaionMatrix(segment, cameraPercent, cameraPos, cameraForward, cameraRight,cameraUp);

		cameraPos += cameraUp *mCurrentFollowHeight;

		cameraForward= trackPos - cameraPos;
		cameraForward.normalise();
		cameraRight = cameraForward.crossProduct(cameraUp);
		cameraUp = cameraRight.crossProduct(cameraForward);

		
		mRenderCamera->setPosition(cameraPos);
		mRenderCamera->setOrientation(Ogre::Quaternion(cameraRight, cameraUp, -cameraForward ));

		if (mShakeTime >= time)
		{
			mShakeTime -= time;
			float RollDegreesPerSecond = 60;
			float PitchDegreesPerSecond = 60;
			float rTest1 = ((rand() / ((float) RAND_MAX / 2))  - 1);
			float rTest2 = ((rand() / ((float) RAND_MAX / 2))  - 1);
			mShakeRoll += rTest1 * time * RollDegreesPerSecond;
			mShakePitch += rTest2 * time * PitchDegreesPerSecond;
			mRenderCamera->roll(Ogre::Degree(mShakeRoll));
			mRenderCamera->pitch(Ogre::Degree(mShakePitch));
			mRenderCamera->moveRelative(mShakeOffset);
		}
		else
		{
			mShakeTime = 0;
		}

	}

	// Any code needed here to move the camera about per frame
	//  (use mRenderCamera to get the actual render camera, of course!)
}
