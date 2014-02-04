namespace Ogre
{
	class Overlay;
	class OverlayElement;
}


class HUD
{
public:
	HUD();

	enum Kind {left, right, center, last};

	void setScore(int newScore);

	void update(float time);

	void startArrow(Kind type, float time = 5.0, float flashDelay = 0.5); 

protected:
	Ogre::Overlay *mArrowOverlay[(int) HUD::last];
	Ogre::Overlay *mScoreOverlay;
	Ogre::OverlayElement *mScoreText;
	float mArrowFlashTime[(int) HUD::last];
	float mArrowFlashDelay[(int) HUD::last];
	bool mArrowStatus[(int) HUD::last];
	float mArrowTimeRemaining[(int) HUD::last];
};