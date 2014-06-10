#include "OgreCamera.h"
#include "Camera.h"
#include "World.h"
#include "Bezier.h"
#include "OgreVector3.h"
#include "Player.h"

RunnerCamera::RunnerCamera(Ogre::Camera *renderCamera, World *world) :
mRenderCamera(renderCamera), mWorld(world), mCurrentTrackingObject(0)
{
	currentSegment = 0;
	currentPercent = 0.3f;
	direction = 1;
	mRenderCamera->setNearClipDistance(2);
	mFollowDistance = 200;
	mType = FollowType::NORMAL;

	// Any other initialization that needs to happen
}


void
		RunnerCamera::SetFollowType(FollowType typ)
{



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

		float size = mWorld->trackPath->pathLength(segment);
		float fDist = mFollowDistance;
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
		


		mWorld->trackPath->getPointAndRotaionMatrix(segment, cameraPercent, cameraPos, cameraForward, cameraRight,cameraUp);

		cameraPos += cameraUp *50;

		cameraForward= trackPos - cameraPos;
		cameraForward.normalise();
		cameraRight = cameraForward.crossProduct(cameraUp);
		cameraUp = cameraRight.crossProduct(cameraForward);

		
		mRenderCamera->setPosition(cameraPos);
		mRenderCamera->setOrientation(Ogre::Quaternion(cameraRight, cameraUp, -cameraForward ));
	}

	// Any code needed here to move the camera about per frame
	//  (use mRenderCamera to get the actual render camera, of course!)
}
