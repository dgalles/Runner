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
#include "Achievements.h"
#include <OIS/OIS.h>

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "Kinect.h"
#include "Menu.h"
#include "OgreOverlaySystem.h"
#include "OgreFontManager.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "Sound.h"
#include "LoginWrapper.h"
#include "Logger.h"

#include "JsonUtils.h"

#include <iostream>
#include <fstream>
#include <iostream>

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
	mFrameListener = new MainListener(mWindow, mAIManager, mWorld, mRunnerCamera, mKinect, mGamepad, mPlayer, mHUD, mAchievements);
	//mFrameListener = new MainListener(mWindow, mAIManager, mWorld, mRunnerCamera, mGamepad, mPlayer, mHUD);
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

	SoundBank::getInstance()->setup();
	mHUD = new HUD();
	mAchievements = new Achievements("Achievements.txt");
	mWorld = new World(mSceneMgr, mHUD, this);
	mAIManager = new AIManager(mWorld);
	mRunnerCamera = new RunnerCamera(mCamera, mWorld);
	InputHandler::getInstance()->initialize(mWindow);
	InputHandler::getInstance()->setEventCallback(MenuManager::getInstance());

	mKinect = new Kinect();
	mKinect->initSensor();
	mGamepad = new XInputManager();
	mPlayer = new Player(mWorld, mGamepad, mKinect, mAchievements);
	mRunnerCamera->TrackObject(mPlayer);
	mWorld->addCamera(mRunnerCamera);

	mLogin = new LoginWrapper();
	mLogger = new Logger(mLogin);
	mLogger->Connect();
	
	mKinect->addSkelListener(mLogger);

}
void
	Runner::startGame()
{
	mWorld->reset(); 
	mPlayer->reset(); 
	mAchievements->ResetActive(); 
	mPlayer->startGame();
	mLogger->StartSession();
	mKinect->StartSession();
}


void Runner::endGame()
{
	mLogger->EndSession();
	mKinect->EndSession();
}


void Runner::writeConfigStr()
{
	MenuManager *menus = MenuManager::getInstance();

	std::string result = getConfigString();
	//std::ofstream configFile;
	//configFile.open ("config.txt", std::ios::out);

//	configFile << result;
//	configFile.close();
	mLogin->sendProfileData(result);

}


void Runner::setSingleConfig(std::string key, std::string value)
{
	key = JSON_UTIL::stripQuotes(key);
	if (key == "menus")
	{
		MenuManager::getInstance()->setMenuConfig(value);
	}
	else if (key == "achievements")
	{
		mAchievements->setCompletedAchievements(value);
	}
	else if (key == "coins")
	{
		mPlayer->setTotalCoins(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
	else if (key == "lifetimeCoins")
	{
		mPlayer->setLifetimeCoins(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
	else if (key == "totalMeters")
	{
		mPlayer->setTotalMeters((float)atof(JSON_UTIL::stripQuotes(value).c_str()));

	}
	else if (key == "armor")
	{
		mPlayer->setInitialArmor(atoi(JSON_UTIL::stripQuotes(value).c_str()));

	}
}

void Runner::setFromConfigString(std::string configString)
{

	///TODO:  This parsing of JSON dictionaries really needs to be refactored ..

	std::size_t braceIndex = configString.find_first_of('{');
	if (braceIndex == std::string::npos)
	{
		return;
	}

	std::string remainder = configString.substr(braceIndex+1);

	std::size_t nextIndex = remainder.find_first_not_of("\t \n");
	while (nextIndex != std::string::npos && remainder[nextIndex] != '}')
	{
		std::string nextKey = JSON_UTIL::stripQuotes(JSON_UTIL::firstItem(remainder));

		remainder = JSON_UTIL::removeFirstitem(remainder);
	
		size_t colonIndex = remainder.find_first_of(":");
		if (colonIndex == std::string::npos)
		{
			// ERROR!
			nextIndex = std::string::npos;
			break;
		}
		remainder = remainder.substr(colonIndex+1);
		std::string value = JSON_UTIL::firstItem(remainder);
		remainder = JSON_UTIL::removeFirstitem(remainder);
		setSingleConfig(nextKey, value);

		nextIndex = remainder.find_first_not_of("\t \n");
		if (nextIndex != std::string::npos && remainder[nextIndex] == ',')
			nextIndex++;
		remainder = remainder.substr(nextIndex);
		nextIndex = remainder.find_first_not_of("\t \n");

	}



}

std::string Runner::getConfigString()
{
	std::string configStr = "{ \"menus\" : " + MenuManager::getInstance()->getMenuConfig(); 
	configStr += ", \"achievements\" : " + mAchievements->getCompletedAchievements();
	configStr += ", \"coins\" : \"" + std::to_string(mPlayer->getTotalCoins()) +"\"";
	configStr += ", \"lifetimeCoins\" : \"" + std::to_string(mPlayer->getLifetimeCoins())+"\"";
	configStr += ", \"totalMeters\" : \"" + std::to_string(mPlayer->getTotalMeters())+"\"";
	configStr += ", \"armor\" : \"" + std::to_string(mPlayer->getInitialArmor())+"\"";
	configStr += "}";

	return configStr;
}

void Runner::readConfigStr()
{
	MenuManager *menus = MenuManager::getInstance();

	std::string config = mLogin->getProfileData();
	
	if (config.size() > 0)
	{

		setFromConfigString(config);
	}
}

void
Runner::setupMenus()
{
    MenuManager *menus = MenuManager::getInstance();

    HUD *h = mHUD;
    MainListener *l = mFrameListener;
    Player *p = mPlayer;
    World *w = mWorld;
    Kinect *k = mKinect;
	Achievements *a = mAchievements;
	SoundBank *sb = SoundBank::getInstance();
	LoginWrapper *lm = mLogin;

    Menu *mainMenu = new Menu("Main Menu", "main", 0.05f, 0.1f);
    Menu *options = new Menu("Options", "options", 0.05f, 0.1f, 0.1f, mainMenu);
    Menu *controlOptions = new Menu("Control Options", "controloptions", 0.05f, 0.1f, 0.07f, options);
    Menu *gameplayOptions = new Menu("Gameplay Options", "gameplayoptions", 0.05f, 0.05f, 0.07f, options);
    Menu *soundOptions = new Menu("Sound Options", "soundOptions", 0.05f, 0.1f,0.1f, options);
    Menu *advancedOptions = new Menu("Advanced Options", "advancedOptions", 0.05f, 0.1f,0.1f, options);
    Menu *login = new Menu("Login", "login", 0.05f, 0.1f,0.1f, mainMenu);
    Menu *pauseMenu = new Menu("Pause Menu", "pause", 0.05f, 0.1f);

    pauseMenu->disable();
    options->disable();
	controlOptions->disable();
	gameplayOptions->disable();
	advancedOptions->disable();
	soundOptions->disable();
	mainMenu->disable();
	login->enable();

	menus->addMenu(mainMenu);
    menus->addMenu(options);
    menus->addMenu(pauseMenu);
	menus->addMenu(gameplayOptions);
	menus->addMenu(controlOptions);
	menus->addMenu(soundOptions);
	menus->addMenu(advancedOptions);
	menus->addMenu(login);


	login->AddChooseString("Username",[lm](Ogre::String s) {lm->changeUsername(s); },"",15,false);
	login->AddChooseString("Password",[lm, this](Ogre::String s) {this->setFromConfigString(lm->changePassword(s));},"",15,true);
	login->AddSelectElement("Return to Main Menu", [login, mainMenu]() {login->disable(); mainMenu->enable();});
	

    options->AddSelectElement("Control Options", [options, controlOptions]() {options->disable(); controlOptions->enable();});
    options->AddSelectElement("Gameplay Options", [options, gameplayOptions]() {options->disable(); gameplayOptions->enable();});
    options->AddSelectElement("Sound Options", [options, soundOptions]() {options->disable(); soundOptions->enable();});
    options->AddSelectElement("Advanced Options", [options, advancedOptions]() {options->disable(); advancedOptions->enable();});
	options->AddSelectElement("Return to Main Menu", [options, mainMenu]() {options->disable(); mainMenu->enable();});

    gameplayOptions->AddChooseBool("Arrow Indicators", [h](bool show) {h->showArrows(show);}, h->arrowsShown(), true);
    gameplayOptions->AddChooseInt("Starting Armor", [p](int x) {p->setInitialArmor(x); }, 1, 8, p->getInitialArmor(), 1, true);
	gameplayOptions->AddChooseBool("Use Forward / Backward Leaning", [w, p](bool use) {w->setUseFrontBack(use); p->setUseFrontBack(use);} , p->getUseFrontBack(), true);
    gameplayOptions->AddChooseFloat("Obstacle Frequency", [w](float x) {w->setObstacleFrequency(x); }, 0.0f, 1.0f,w->getObstacleFrequency(), 0.1f, true);
    gameplayOptions->AddChooseInt("Minimum Obstacle Separation", [w](int x) {w->setObstacleSeparation(x); }, 0, 15, w->getObstacleSeparation(), 1, true);
    gameplayOptions->AddChooseInt("Initial Speed", [p](int x) {p->setInitialSpeed(x); }, 5, 100, p->getInitialSpeed(), 5, true);
    gameplayOptions->AddChooseInt("Max Speed", [p](int x) {p->setMaxSpeed(x); }, 30, 100, p->getMaxSpeed(), 5, true);
    gameplayOptions->AddChooseInt("Auto Speed Increase Rate", [p](int x) {p->setAutoAceelerateRate(x); }, 0, 20, p->getAutoAccelerateRate(), 1, true);

	std::vector<Ogre::String> names;
	std::vector<std::function<void()>> callbacks;
	names.push_back("Duck / Lean");
	callbacks.push_back([p, w]() { p->setLeanEqualsDuck(true); w->setUseFrontBack(p->getUseFrontBack()); });
	
	names.push_back("Change Speed");
	callbacks.push_back([p,w]() {  p->setLeanEqualsDuck(false); w->setUseFrontBack(false); });

	gameplayOptions->AddChooseEnum("Forward / Back Controls",names,callbacks,0, true);	
    gameplayOptions->AddChooseInt("Manual Speed Change Rate", [p](int x) {p->setManualAceelerateRate(x); }, 0, 20, p->getManualAccelerateRate(), 1, true);
    gameplayOptions->AddSelectElement("Return to Options Menu", [gameplayOptions,options]() {gameplayOptions->disable(); options->enable();});

    controlOptions->AddChooseBool("Callibrate Kinect Every Game", [p](bool x) {p->setAutoCallibrate(x); }, p->getAutoCallibrate(), true);
    controlOptions->AddChooseFloat("Kinect Sensitivity Left / Right", [p](float x) {p->setKinectSentitivityLR(x); }, 0.7f, 1.5f, 1.f, 0.1f, true);
    controlOptions->AddChooseFloat("Kinect Sensitivity Front / Back", [p](float x) {p->setKinectSentitivityFB(x); }, 0.7f, 1.5f, 1.f, 0.1f, true);
    controlOptions->AddSelectElement("Callibrate Kinect Now", [controlOptions, k]() {controlOptions->disable(); k->callibrate(4.0f, [controlOptions]() {controlOptions->enable();});});
    controlOptions->AddChooseBool("Invert Front/Back Controls", [p](bool x) {p->setInvertControls(x); }, p->getInvertControls(), true);
	controlOptions->AddChooseBool("Enable Kinect", [p](bool x) { p->setEnableKinect(x);  if (!x) p->setAutoCallibrate(false); }, p->getEnableKinect(), true);
	controlOptions->AddChooseBool("Enable Keyboard", [p](bool x) { p->setEnableKeyboard(x);}, p->getEnableKeyboard(), true);
	// controlOptions->AddChooseBool("Enable Gamepad", [p](bool x) { p->setEnableGamepad(x);}, p->getEnableGamepad());


    controlOptions->AddSelectElement("Return to Options Menu", [controlOptions,options]() {controlOptions->disable(); options->enable();});


    soundOptions->AddChooseBool("Enalbe Sound", [sb](bool x) {sb->setEnableSound(x); }, sb->getEnableSound(), true);
	soundOptions->AddChooseInt("Volume", [sb](int x) {sb->setVolume(x); }, 0, 128, sb->getVolume(), 5, true);
    soundOptions->AddSelectElement("Return to Options Menu", [soundOptions,options]() {soundOptions->disable(); options->enable();});


    mainMenu->AddSelectElement("Start Game", [mainMenu,this]() {this->writeConfigStr(); mainMenu->disable(); this->startGame(); });

    mainMenu->AddSelectElement("Login", [mainMenu, login]() {mainMenu->disable(); login->enable();});
    mainMenu->AddSelectElement("Show Goals", [mainMenu, a]() {a-> ShowAllAchievements(true); mainMenu->disable();});

    mainMenu->AddSelectElement("Options", [options, mainMenu]() {options->enable(); mainMenu->disable();});
    mainMenu->AddSelectElement("Quit", [l, this]() {this->writeConfigStr(); l->quit();});

    pauseMenu->AddSelectElement("Continue", [pauseMenu, p]() {pauseMenu->disable(); p->setPaused(false); });
    pauseMenu->AddSelectElement("End Game (Return to Main Menu)", [pauseMenu,mainMenu, p, w, h, this]() {this->endGame(), h->showHUDElements(false); pauseMenu->disable();mainMenu->enable(); p->setPaused(true); });
    pauseMenu->AddSelectElement("Quit (Close Program)", [l]() {l->quit();});



	std::vector<Ogre::String> names2;
	std::vector<std::function<void()>> callbacks2;
	names2.push_back("Very Low");
	callbacks2.push_back([w]() { w->setUnitsPerPathLength(0.005f); });

	names2.push_back("Low");
	callbacks2.push_back([w]() { w->setUnitsPerPathLength(0.01f); });

	names2.push_back("Medium");
	callbacks2.push_back([w]() { w->setUnitsPerPathLength(0.05f); });


	names2.push_back("High");
	callbacks2.push_back([w]() { w->setUnitsPerPathLength(0.1f); });

	advancedOptions->AddChooseEnum("Track Resolution",names2,callbacks2,3, true);	


	advancedOptions->AddChooseInt("Track View Distance (segments)", [p](int x) {p->setTrackLookahed(x);}, 10, 200,  p->getTrackLookahead(), 5, true);
	    advancedOptions->AddSelectElement("Send Profile to Server", [this]() {this->writeConfigStr();});
    advancedOptions->AddSelectElement("Get Profile from Server", [this]() {this->readConfigStr();});

	advancedOptions->AddSelectElement("Return to Options Menu", [advancedOptions, options]() {advancedOptions->disable(); options->enable();});



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
    createFrameListener();
    setupMenus();


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
    delete mRunnerCamera;
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
	SoundBank::shutdown();
}


