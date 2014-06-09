#pragma once
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

	void update(float time);
    
    void showArrows(bool show) { mShowArrows = show; }
    bool arrowsShown() { return mShowArrows; }

	void startArrow(Kind type, float time = 60.0, float flashDelay = 0.5); 
    void stopArrow(Kind type);
	void stopAllArrows();

protected:
	Ogre::Overlay *mArrowOverlay[(int) HUD::last];
	Ogre::Overlay *mScoreOverlay;
	Ogre::OverlayElement *mCoinsText;
	Ogre::OverlayElement *mDistanceText;
	float mArrowFlashTime[(int) HUD::last];
	float mArrowFlashDelay[(int) HUD::last];
	bool mArrowStatus[(int) HUD::last];
	float mArrowTimeRemaining[(int) HUD::last];
    bool mShowArrows;
};