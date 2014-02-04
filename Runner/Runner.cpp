#include "Runner.h"
#include "World.h"
#include "InputHandler.h"
#include "AIManager.h"
#include "MainListener.h"
#include "Camera.h"
#include "XInputManager.h"
#include "Player.h"
#include "HUD.h"
#include "Menu.h"

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "Kinect.h"
#include "Menu.h"
#include "OgreOverlaySystem.h"
#include "OgreFontManager.h"

Runner::Runner()
{
    mFrameListener = 0;
    mRoot = 0;
    // Provide a nice cross platform solution for locating the configuration files
    // On windows files are searched for in the current working directory, on OS X however
    // you must provide the full path, the helper function macBundlePath does this for us.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    mResourcePath = macBundlePath() + "/Contents/Resources/";
#else
    mResourcePath = "";
#endif
}

/// Standard destructor
Runner::~Runner()
{
    if (mFrameListener)
        delete mFrameListener;
    if (mRoot)
        delete mRoot;
}



void
Runner::createCamera()
{
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setPosition(Ogre::Vector3(0,10,3000));
	mCamera->lookAt(Ogre::Vector3(0,0,0));
}



// We will create a single frame listener, to handle our main event loop.  While we could
// add as many frame listeners as we want (and let the main rendering loop call them),
// we would not have as much control over the order that the listeners are called --
//  so we will just have one listener and handle everything ourselves.
void 
Runner::createFrameListener(void)
{
	mFrameListener = new MainListener(mWindow, mAIManager, mWorld, mPongCamera, mKinect,mGamepad, mPlayer, mHUD, mMenuManager);
	mRoot->addFrameListener(mFrameListener);
	// mFrameListener->showDebugOverlay(true);

}

// We will have a single viewport.  If you wanted splitscreen, or a rear-view mirror, then
// you may want multiple viewports.
void 
Runner::createViewports(void)
{
	    // Create one viewport, entire window
        Ogre::Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));    
}

// Here is where we set up all of the non-rendering stuff (our world, various managers, etc)
void 
Runner::createScene() 
{
	//Ogre::FontPtr f = Ogre::FontManager::getSingleton().getByName("Big");
	//f->load();

	mHUD = new HUD();
	mWorld = new World(mSceneMgr, mHUD);
	mAIManager = new AIManager(mWorld);
	mPongCamera = new PongCamera(mCamera, mWorld);
	InputHandler::getInstance()->initialize(mWindow);
	mKinect = new Kinect();
	mKinect->initSensor();
	mGamepad = new XInputManager();
	mPlayer = new Player(mWorld, mGamepad, mKinect);
	mPongCamera->TrackObject(mPlayer);
	mWorld->addCamera(mPongCamera);
	mMenuManager =MenuManager::getInstance();
//	Menu *m = new Menu("Menu Test", "name");
}

bool 
Runner::setup(void)
{

    Ogre::String pluginsPath;
	Ogre::String configPath;
    // only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB

#if OGRE_DEBUG_MODE == 1 && (OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
	// add _d suffix to config files
	pluginsPath = mResourcePath + "plugins_d.cfg";
	configPath = mResourcePath + "ogre_d.cfg";
#else
	pluginsPath = mResourcePath + "plugins.cfg";
	configPath = mResourcePath + "ogre.cfg";

#endif


#endif

    mRoot = new Ogre::Root(pluginsPath, 
        configPath, mResourcePath + "Ogre.log");

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    // Create the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "RunnerSMInstance");
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    createScene();

    // When we start the renderer, it will run forever, making callbacks every
    // frame to the frame listeners.  These listeners are where all of the non-
    // rendering work is done.  
    createFrameListener();

	mKinect->callibrate(4);


	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    return true;

}
/** Configures the application - returns false if the user chooses to abandon configuration. */
bool 
Runner::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true);
        return true;
    }
    else
    {
        return false;
    }
}

void
Runner::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;

	#if OGRE_DEBUG_MODE == 1 && (OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS)
			// add _d suffix to config files
    cf.load(mResourcePath + "resources_d.cfg");
#else
	cf.load(mResourcePath + "resources.cfg");
#endif
		

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                String(macBundlePath() + "/" + archName), typeName, secName);
#else
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
#endif
        }
    }
}

void
Runner::destroyScene()
{
    delete mWorld;
    delete mAIManager;
    delete mPongCamera;
	InputHandler::destroyInstance();
}

void 
Runner::go(void)
{
    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    destroyScene();
}


