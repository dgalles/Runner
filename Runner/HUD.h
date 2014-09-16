#pragma once
#include <vector>
#include "OgreString.h"

namespace Ogre
{
	class Overlay;
	class OverlayElement;
}


class HUD
{
public:
	HUD();

	enum Kind {none, left, right, center, down, up, last};

	void setCoins(int newScore, bool ghost = false, bool consec = false);
    void setDistance(int distance, bool ghost = false);
    void setSpeed(int speed, bool ghost = false);

	void setTime(float newTime);

	void update(float time);
    
    void showArrows(bool show) { mShowArrows = show; }
    bool arrowsShown() { return mShowArrows; }

	void startArrow(Kind type, float time = 60.0, float flashDelay = 0.5); 
    void stopArrow(Kind type);
	void stopAllArrows();

	void setShowIncreaseSpeed(bool show);
	void setShowDecreaseSpeed(bool show);

	void setArmorLevel(int level);

	void showHUDElements(bool show);

	void setRacingMesssage(Ogre::String message);
	void showRacingOverlay(bool show);

	void showGhost(bool show);

	void setFinalRaceTime(Ogre::String message, bool isGhost = false );

	void showRaceOver(bool show);

protected:
	Ogre::Overlay *mArrowOverlay[(int) HUD::last];
	Ogre::Overlay *mScoreOverlay;
	Ogre::Overlay *mRacingOverlay;
	Ogre::Overlay *mRacingDone;

	Ogre::OverlayElement *mCoinsText;
	Ogre::OverlayElement *mDistanceText;
	Ogre::OverlayElement *mSpeedText;
	Ogre::OverlayElement *mTimeText;
	Ogre::OverlayElement *mRacingMessage;


	Ogre::OverlayElement *mGhostCoinsText;
	Ogre::OverlayElement *mGhostDistanceText;
	Ogre::OverlayElement *mGhostSpeedText;


	Ogre::Overlay *mSpeedUpOverlay;
	Ogre::Overlay *mSlowDownOverlay;

	//	std::vector<bool> mAchievmentsCleared;


	std::vector<Ogre::OverlayElement *> mArmorIndicator;

	float mArrowFlashTime[(int) HUD::last];
	float mArrowFlashDelay[(int) HUD::last];
	bool mArrowStatus[(int) HUD::last];
	float mArrowTimeRemaining[(int) HUD::last];
    bool mShowArrows;
	bool mShowHUDElems;
	bool mShowGhostElements;
	int mSpeed;
};