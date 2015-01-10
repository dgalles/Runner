#include "Ogre.h"
#include "OgreFrameListener.h"
#include "Kinect.h"

class InputHandler;
class AIManager;
class World;
class RunnerCamera;
class Kinect;
class XInputManager;
class Player;
class HUD;
class MenuManager;
class Achievements;
class Ghost;
class FrameCounter;

namespace Ogre
{
	class RenderWindow;
}

class MainListener : public Ogre::FrameListener
{
public:
 	MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, RunnerCamera *cams[], Kinect *sensor, XInputManager *gamepad, Player *player[], HUD *hud, Achievements *ach[], Ghost *ghost);
	//MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, RunnerCamera *pongCam, XInputManager *gamepad, Player *player, HUD *hud);

	bool frameStarted(const Ogre::FrameEvent &evt);

	bool paused() { return mPaused; }
	void setPaused(bool paused) { mPaused = paused;}
    void quit() { mQuit = true;}

protected:
	InputHandler *mInputHandler;
	AIManager *mAIManager;
	World *mWorld;
    RunnerCamera *mRunnerCamera[2];
	Kinect     *mKinect;
	XInputManager *mGamepad;
	Player *mPlayer[2];
	Achievements *mAchievements[2];
	HUD *mHUD;
	Ogre::RenderWindow *mRenderWindow;
	bool mPaused;
	MenuManager *mMenus;
    bool mQuit;
	Ghost *mGhost;
	FrameCounter *mFrameCounter;
};
