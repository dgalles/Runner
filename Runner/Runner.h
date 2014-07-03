#ifndef __OgrePong_h_
#define __OgrePong_h_

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//#include "../res/resource.h"
#endif

// Including header files within header files is frowned upon,
// but this one is requried for Ogre Strings.
#include "OgrePrerequisites.h"
#include "OgreWindowEventUtilities.h"

namespace Ogre {
    class Root;
    class RenderWindow;
    class Camera;
    class SceneManager;
	class OverlaySystem;
}
class AIManager;
class InputHandler;
class World;
class MainListener;
class RunnerCamera;
 class Kinect;
class XInputManager;
class Player;
class HUD;
class MenuManager;
class Achievements;
class LoginWrapper;
class Logger;
class Store;
class Menu;

class Runner :  public Ogre::WindowEventListener
{
public:
    Runner();
    ~Runner();

    // Do all the application setup
    bool setup(void);

    // Start runn
    void go(void);
		void endGame();

protected:

    ///////////////////////////////////////////////////
    /// The following methods are all called by the public
    ///   setup method, to handle various initialization tasks
    //////////////////////////////////////////////////

    //  Load all the requrired resources (Mostly reading the file paths
    //  of various resources from the .cfg files)
    void setupResources(void);

    // Invoke the startup window for all of the Ogre settings (resolution, etc)
    bool configure(void);

    // Create all of the required classes and do setup for all non-rendering tasks
    void createScene(void);

    // Free up all memory & resources created in createScene
    void destroyScene(void);

    // Create the rendering camera (separate from the game camera -- the game camera
    //   contains the logic of how the camera should be moved, the rendering camera
    //   is used by the rendering system to render the scene.  So, the game camera 
    //   decides where the camera should be, and then makes calls to the rendering camera
    //   to move the camera
	void createCamera(void);

	void createViewports(void);

    // The FrameListener is what receives callbacks from the rendering loop to handle 
    //  game logic
	void createFrameListener(void);


	void startGame();
    void setupMenus(bool login = true) ;
	Store * createStore(Menu *parent);

	AIManager *mAIManager;
	InputHandler *mInputHandler;
	World *mWorld;
    RunnerCamera *mRunnerCamera[2];
    Kinect *mKinect;
	XInputManager *mGamepad;
	Player *mPlayer[2];
	HUD *mHUD;
	Achievements *mAchievements[2];
	LoginWrapper *mLogin;
	Logger *mLogger;


	void writeConfigStr(int player = 0);
	void setFromConfigString(std::string, int player = 0);

	std::string getConfigString(int player = 0);
	void readConfigStr(int player = 0);
	void setSingleConfig(std::string key, std::string value, int player = 0);


    Ogre::Root *mRoot;
    Ogre::Camera* mCamera[2];
    Ogre::SceneManager* mSceneMgr;
    MainListener* mFrameListener;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcePath;
	Ogre::OverlaySystem *mOverlaySystem;
};



#endif // #ifndef __OgrePong_h_