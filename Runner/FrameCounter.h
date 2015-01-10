#pragma once

#define TIME_PER_SAMPLE  1.0f
namespace Ogre
{
	class Overlay;
	class OverlayElement;
}

class FrameCounter
{
public:
	FrameCounter(void);

	void update(float time);
	~FrameCounter(void);

protected:
	Ogre::OverlayElement *mFrameCounterText;
	Ogre::Overlay *mFrameCounterOverlay;

	float mTimeSinceLastCount;
	int mNumFrames;
		bool mShowFPS;

	

};

