#include "FrameCounter.h"
#include "OgreOverlayManager.h"
#include "OgreOverlay.h"
#include "OgreOverlayElement.h"
#include "OgreString.h"
#include "InputHandler.h"

FrameCounter::FrameCounter(void)
{
	mFrameCounterOverlay = Ogre::OverlayManager::getSingleton().getByName("FrameCounter");
	mFrameCounterText =  Ogre::OverlayManager::getSingleton().getOverlayElement("FrameCounter/Text");

	mTimeSinceLastCount = 0;
	mNumFrames = 0;
	mShowFPS = false;
}

void FrameCounter::update(float time)
{
	mNumFrames++;
	mTimeSinceLastCount += time;

	if (mTimeSinceLastCount >= TIME_PER_SAMPLE)
	{
		char buffer[150];
		sprintf(buffer, "FPS = %.1f", mNumFrames / mTimeSinceLastCount);
		mFrameCounterText->setCaption("FPS: " + Ogre::String(buffer));
		mNumFrames = 0;
		mTimeSinceLastCount =0;
	}

	if (InputHandler::getInstance()->KeyPressedThisFrame(OIS::KC_F))
	{
		mShowFPS = !mShowFPS;
		if (mShowFPS)
		{
			mFrameCounterOverlay->show();
		}
		else
		{
			mFrameCounterOverlay->hide();
		}

	}

}


FrameCounter::~FrameCounter(void)
{
}
