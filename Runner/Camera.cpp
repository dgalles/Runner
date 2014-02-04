#include "OgreCamera.h"
#include "Camera.h"
#include "World.h"
#include "Bezier.h"
#include "OgreVector3.h"
#include "Player.h"

PongCamera::PongCamera(Ogre::Camera *renderCamera, World *world) :
mRenderCamera(renderCamera), mWorld(world), mCurrentTrackingObject(0)
{
	currentSegment = 0;
	currentPercent = 0.3;
	direction = 1;
	mRenderCamera->setNearClipDistance(2);
	mFollowDistance = 100;
	// Any other initialization that needs to happen
}

void 
PongCamera::Pause()
{
	mPaused = true;
}
void 
PongCamera::UnPause()
{
	mPaused = false;
}

void
PongCamera::TrackObject(TrackableObject *objectToTrack)
{
	mCurrentTrackingObject = objectToTrack;

}
void
PongCamera::Think(float time)
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

		cameraPos += cameraUp *20;

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
