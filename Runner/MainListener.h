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

class MainListener : public Ogre::FrameListener
{
public:
 	MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, PongCamera *pongCam, Kinect *sensor, XInputManager *gamepad, Player *player, HUD *hud);
	//MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, PongCamera *pongCam, XInputManager *gamepad, Player *player, HUD *hud);

	bool frameStarted(const Ogre::FrameEvent &evt);

	bool paused() { return mPaused; }
	void setPaused(bool paused) { mPaused = paused;}
    void quit() { mQuit = true;}

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
    bool mQuit;
};
