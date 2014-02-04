#include "Ogre.h"
#include "OgreFrameListener.h"
#include "Kinect.h"

class InputHandler;
class AIManager;
class World;
class PongCamera;
class Kinect;
class XInputManager;
class Player;
class HUD;
class MenuManager;

namespace Ogre
{
	class RenderWindow;
}

class MainListener : public Ogre::FrameListener, public KinectMessageReceiver
{
public:
	MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, PongCamera *pongCam, Kinect *sensor, XInputManager *gamepad, Player *player, HUD *hud, MenuManager *menus);

	bool frameStarted(const Ogre::FrameEvent &evt);

	virtual void callibrationStarted();
	virtual void callibrationCompleted();
	bool paused() { return mPaused; }
	void setPaused(bool paused) { mPaused = paused;}

	void callibrateKinect(float time) { mKinect->callibrate(time); }

protected:
	InputHandler *mInputHandler;
	AIManager *mAIManager;
	World *mWorld;
    PongCamera *mPongCamera;
	Kinect     *mKinect;
	XInputManager *mGamepad;
	Player *mPlayer;
	HUD *mHUD;
	Ogre::RenderWindow *mRenderWindow;
	bool mPaused;
	MenuManager *mMenus;
};
