#pragma once
#include <vector>

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

	void setCoins(int newScore);
    void setDistance(int distance);
    void setSpeed(int speed);

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

protected:
	Ogre::Overlay *mArrowOverlay[(int) HUD::last];
	Ogre::Overlay *mScoreOverlay;
	Ogre::OverlayElement *mCoinsText;
	Ogre::OverlayElement *mDistanceText;
	Ogre::OverlayElement *mSpeedText;
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
	int mSpeed;
};