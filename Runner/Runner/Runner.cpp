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
#include "Store.h"
#include "OgreOverlaySystem.h"
#include "OgreFontManager.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "Sound.h"
#include "LoginWrapper.h"
#include "Logger.h"
#include "Ghost.h"

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
	mCamera[0] = mSceneMgr->createCamera("Player1Cam");
	mCamera[0]->setPosition(Ogre::Vector3(0,10,3000));
	mCamera[0]->lookAt(Ogre::Vector3(0,0,0));
		mCamera[1] = mSceneMgr->createCamera("Player2Cam");
	mCamera[1]->setPosition(Ogre::Vector3(0,10,3000));
	mCamera[1]->lookAt(Ogre::Vector3(0,0,0));

}



// We will create a single frame listener, to handle our main event loop.  While we could
// add as many frame listeners as we want (and let the main rendering loop call them),
// we would not have as much control over the order that the listeners are called --
//  so we will just have one listener and handle everything ourselves.
void 
Runner::createFrameListener(void)
{
	mFrameListener = new MainListener(mWindow, mAIManager, mWorld, mRunnerCamera, mKinect, mGamepad, mPlayer, mHUD, mAchievements, mGhost);
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

#ifdef TEST
        Ogre::Viewport* vp = mWindow->addViewport(mCamera[0],0,0,0,0.5, 1);
        vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
        // Alter the camera aspect ratio to match the viewport
        mCamera[0]->setAspectRatio(Ogre::Real(vp->getActualWidth() / 2) / Ogre::Real(vp->getActualHeight()));    
        Ogre::Viewport* vp2 = mWindow->addViewport(mCamera[1],1,0.5f,0,0.5f,1);
        vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
        // Alter the camera aspect ratio to match the viewport
        mCamera[1]->setAspectRatio(Ogre::Real(vp->getActualWidth() /2 ) / Ogre::Real(vp->getActualHeight()));    

#else
        Ogre::Viewport* vp = mWindow->addViewport(mCamera[0]);
        vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
        // Alter the camera aspect ratio to match the viewport
        mCamera[0]->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));    

#endif
}

// Here is where we set up all of the non-rendering stuff (our world, various managers, etc)
void 
Runner::createScene() 
{
	//Ogre::FontPtr f = Ogre::FontManager::getSingleton().getByName("Big");
	//f->load();

	SoundBank::getInstance()->setup();
	mHUD = new HUD();
	mAchievements[0] = new Achievements("Achievements.txt");
	mAchievements[1] = new Achievements("Achievements.txt");
	mWorld = new World(mSceneMgr, mHUD, this, false);
	//mWorld[1] = new World(mSceneMgr, mHUD, this, true, mWorld[0]->trackPath);
	mRunnerCamera[0] = new RunnerCamera(mCamera[0], mWorld);
	//mRunnerCamera[1] = new RunnerCamera(mCamera[1], mWorld[1]);
	InputHandler::getInstance()->initialize(mWindow);
	InputHandler::getInstance()->setEventCallback(MenuManager::getInstance());

	mKinect = new Kinect();
	mKinect->initSensor();
	mGamepad = new XInputManager();
	mPlayer[0] = new Player(mWorld, mGamepad, mKinect, mAchievements[0]);
	//mPlayer[1] = new Player(mWorld[1], mGamepad, mKinect, mAchievements[1]);
	mRunnerCamera[0]->TrackObject(mPlayer[0]);
	//mRunnerCamera[1]->TrackObject(mPlayer[1]);
	mWorld->addCamera(mRunnerCamera[0]);

	mLogin = new LoginWrapper();
	mLogger = new Logger(mLogin);
	// mLogger->Connect();
	mGhost = new Ghost(mWorld);

	mPlayer[0]->setGhost(mGhost);
	
	mKinect->addSkelListener(mLogger);
	mPlayer[0]->addPlayerListener(mLogger);

}
void
	Runner::startGame()
{
	mWorld->reset(); 
	//mWorld[1]->reset(); 
	mPlayer[0]->reset(); 
	//mPlayer[1]->reset(); 
	mWorld->setGhostInfo(mGhost->getData());
	mPlayer[0]->setGhostInfo(mGhost->getData());

	mAchievements[0]->ResetActive(); 
	//mAchievements[1]->ResetActive(); 
	mPlayer[0]->startGame();
	//mPlayer[1]->startGame();
	mGhost->startRecording();
	mLogger->StartSession();
	mKinect->StartSession();
}


void Runner::endGame()
{
	mLogger->EndSession();
	mKinect->EndSession();
}


void Runner::writeConfigStr(int player)
{
	MenuManager *menus = MenuManager::getInstance();

	std::string result = getConfigString(player);
	//std::ofstream configFile;
	//configFile.open ("config.txt", std::ios::out);

//	configFile << result;
//	configFile.close();
	mLogin->sendProfileData(result);

}


void Runner::setSingleConfig(std::string key, std::string value, int player)
{
	key = JSON_UTIL::stripQuotes(key);
	if (key == "menus")
	{
		MenuManager::getInstance()->setMenuConfig(value);
	}
	else if (key == "achievements")
	{
		mAchievements[player]->setCompletedAchievements(value);
	}
	else if (key == "coins")
	{
		mPlayer[player]->setTotalCoins(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
	else if (key == "lifetimeCoins")
	{
		mPlayer[player]->setLifetimeCoins(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
	else if (key == "totalMeters")
	{
		mPlayer[player]->setTotalMeters((float)atof(JSON_UTIL::stripQuotes(value).c_str()));

	}
	else if (key == "armor")
	{
		mPlayer[player]->setInitialArmor(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}

	else if (key == "boostFrequency")
	{
		mWorld->setBoostFreq(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
		else if (key == "boostDuration")
	{
		mPlayer[player]->setBoostDuration(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}

		else if (key == "shieldFrequency")
	{
		mWorld->setShieldFreq(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
		else if (key == "shieldDuration")
	{
		mPlayer[player]->setShieldDuration(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
				else if (key == "magnetFrequency")
	{
		mWorld->setShieldFreq(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}
		else if (key == "magnetDuration")
	{
		mPlayer[player]->setShieldDuration(atoi(JSON_UTIL::stripQuotes(value).c_str()));
	}



}

void Runner::setFromConfigString(std::string configString, int player)
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
		setSingleConfig(nextKey, value, player);

		nextIndex = remainder.find_first_not_of("\t \n");
		if (nextIndex != std::string::npos && remainder[nextIndex] == ',')
			nextIndex++;
		remainder = remainder.substr(nextIndex);
		nextIndex = remainder.find_first_not_of("\t \n");
	}

}

std::string Runner::getConfigString(int player)
{
	std::string configStr = "{ \"menus\" : " + MenuManager::getInstance()->getMenuConfig(); 
	configStr += ", \"achievements\" : " + mAchievements[player]->getCompletedAchievements();
	configStr += ", \"coins\" : \"" + std::to_string(mPlayer[player]->getTotalCoins()) +"\"";
	configStr += ", \"lifetimeCoins\" : \"" + std::to_string(mPlayer[player]->getLifetimeCoins())+"\"";
	configStr += ", \"totalMeters\" : \"" + std::to_string(mPlayer[player]->getTotalMeters())+"\"";
	configStr += ", \"armor\" : \"" + std::to_string(mPlayer[player]->getInitialArmor())+"\"";
	configStr += ", \"boostFrequency\" : \"" + std::to_string(mWorld->getBoostFreq())+"\"";
	configStr += ", \"boostDuration\" : \"" + std::to_string(mPlayer[player]->getBoostDuration())+"\"";
	configStr += ", \"shieldFrequency\" : \"" + std::to_string(mWorld->getShieldFreq())+"\"";
	configStr += ", \"shieldDuration\" : \"" + std::to_string(mPlayer[player]->getShieldDuration())+"\"";
	configStr += ", \"magnetFrequency\" : \"" + std::to_string(mWorld->getMagnetFrequency())+"\"";
	configStr += ", \"magnetDuration\" : \"" + std::to_string(mPlayer[player]->getMagnetDuration())+"\"";
	configStr += "}";

	return configStr;
}

void Runner::readConfigStr(int player)
{
	MenuManager *menus = MenuManager::getInstance();

	std::string config = mLogin->getProfileData();
	
	if (config.size() > 0)
	{

		setFromConfigString(config, player);
	}
}


void Runner::createStores(Menu *parent, std::vector<Store *> &stores)
{

	Player *p = mPlayer[0];
	World *w = mWorld;

	Store *store  = new Store("Store", "Store1", 0.05f, 0.05f, 0.12f, [p]() {return p->getTotalCoins();}, [p](int n) {p->setTotalCoins(n);}, parent);
	Store *store2  = new Store("Store", "Store2", 0.05f, 0.05f, 0.12f, [p]() {return p->getTotalCoins();}, [p](int n) {p->setTotalCoins(n);}, parent);

	std::vector<int> prices;
	prices.push_back(100);
	prices.push_back(200);
	prices.push_back(300);
	prices.push_back(400);
	prices.push_back(500);

	store->AddStoreElem("Armor", [p]() { return p->getInitialArmor();}, [p](int x) { p->setInitialArmor(x); },1, 5, prices);
	store->AddStoreElem("Boost Duration", [p]() { return p->getBoostDuration();}, [p](int x) { p->setBoostDuration(x); },1, 5, prices);
	store->AddStoreElem("Boost Frequency", [w]() { return w->getBoostFreq();}, [w](int x) { w->setBoostFreq(x); },1, 5, prices);
	store->AddSelectElement("More Store Items", [store, store2]() { store->disable(); store2->enable();});
	store->AddSelectElement("Return to Main Menu", [store, parent]() {store->disable(); parent->enable(); });

	stores.push_back(store);

	store2->AddStoreElem("Shield Duration", [p]() { return p->getShieldDuration();}, [p](int x) { p->setShieldDuration(x); },1, 5, prices);
	store2->AddStoreElem("Shield Frequency", [w]() { return w->getShieldFreq();}, [w](int x) { w->setShieldFreq(x); },1, 5, prices);
	store2->AddStoreElem("Magnet Duration", [p]() { return p->getMagnetDuration();}, [p](int x) { p->setMagnetDuration(x); },1, 5, prices);
	store2->AddStoreElem("Magnet Frequency", [w]() { return w->getMagnetFrequency();}, [w](int x) { w->setMagnetFrequency(x); },1, 5, prices);
	store2->AddSelectElement("More Store Items", [store, store2]() { store2->disable(); store->enable();});
	store2->AddSelectElement("Return to Main Menu", [store2, parent]() {store2->disable(); parent->enable(); });
	stores.push_back(store2);

}



void Runner::loadGhost()
{
	Ghost *ghost = mGhost;

	WIN32_FIND_DATA fileData;
   HANDLE hFindFile = INVALID_HANDLE_VALUE;
   DWORD dwError=0;

	  hFindFile = FindFirstFile("*.ghost", &fileData);
   
	std::vector<Ogre::String> filenames;

	if (INVALID_HANDLE_VALUE != hFindFile) 
	{
		do
		{
			//Skip directories
			if (FILE_ATTRIBUTE_DIRECTORY & fileData.dwFileAttributes)
				continue;

			Ogre::String filename = fileData.cFileName;
			filenames.push_back(filename);
		} while(FindNextFile(hFindFile, &fileData));

		FindClose(hFindFile);
	}    

	if (filenames.size() == 0)
	{
		
	}
	else
	{
		MenuManager *manager = MenuManager::getInstance();
		ScrollSelectMenu *ghostMenu = (ScrollSelectMenu *) manager->getMenu("ghostSelect");
		ghostMenu->reset(filenames,  [ghostMenu, ghost, this](Ogre::String filename) { 
			 ghostMenu->disable();
			 ghost->readFile(filename); 
			 this->replayGhost(); 
		});

		ghostMenu->enable();
	}

   // List all the files in the directory with some info about them.

}

void 
	Runner::replayGhost()
{
	Ghost::GhostInfo *data = mGhost->getData();

	mWorld->reset(data); 
	//mWorld[1]->reset(); 
	mPlayer[0]->reset(data); 
	//mPlayer[1]->reset(); 
	mAchievements[0]->ResetActive(); 
	//mAchievements[1]->ResetActive(); 
	mPlayer[0]->startGame();
	//mPlayer[1]->startGame();
	mGhost->startPlayback();
	mLogger->StartSession();
	mKinect->StartSession();
}

void
Runner::setupMenus(bool loginRequired)
{
    MenuManager *menus = MenuManager::getInstance();

    HUD *h = mHUD;
    MainListener *l = mFrameListener;
    Player *p = mPlayer[0];
    World *w = mWorld;
    Kinect *k = mKinect;
	Achievements *a = mAchievements[0];
	SoundBank *sb = SoundBank::getInstance();
	LoginWrapper *lm = mLogin;
	Ghost *ghost = mGhost;

    Menu *mainMenu = new Menu("Main Menu", "main", 0.05f, 0.1f, 0.08f);
    Menu *options = new Menu("Options", "options", 0.05f, 0.1f, 0.1f, mainMenu);
    Menu *controlOptions = new Menu("Control Options", "controloptions", 0.05f, 0.1f, 0.07f, options);
    Menu *gameplayOptions = new Menu("Gameplay Options", "gameplayoptions", 0.05f, 0.05f, 0.07f, options);
    Menu *soundOptions = new Menu("Sound Options", "soundOptions", 0.05f, 0.1f,0.1f, options);
    Menu *advancedOptions = new Menu("Advanced Options", "advancedOptions", 0.05f, 0.1f, 0.08f, options);
    Menu *login = new Menu("Login", "login", 0.05f, 0.1f,0.1f, mainMenu);
    Menu *pauseMenu = new Menu("Pause Menu", "pause", 0.05f, 0.1f);
    Menu *obstacleMenu = new Menu("Obstacle Options", "obstacle", 0.05f, 0.1f, 0.1f, options);
    Menu *confirmMenu = new Menu("Confirm Profile Reset", "profleReset", 0.1f, 0.1f, 0.1f, advancedOptions);
	Menu *endGameMenu = new Menu("Game Over!", "gameOver", 0.1f, 0.1f, 0.1f, NULL);
	Menu *awkGhostSave  = new Menu("Ghost Saved", "ghostSaved", 0.1f, 0.1f, 0.1f, NULL);
	Menu *ghostSelect = new ScrollSelectMenu("Select Ghost Run", "ghostSelect", 0.05f, 0.1f, 0.1f, mainMenu);



	std::vector<Store *> stores;
	createStores(mainMenu, stores);
	for (unsigned int i = 0; i < stores.size(); i++)
	{
		menus->addMenu(stores[i]);

	}
	Store *store = stores[0];



	menus->addMenu(mainMenu);
    menus->addMenu(options);
    menus->addMenu(pauseMenu);
	menus->addMenu(gameplayOptions);
	menus->addMenu(controlOptions);
	menus->addMenu(soundOptions);
	menus->addMenu(advancedOptions);
	menus->addMenu(login);
	menus->addMenu(obstacleMenu);
	menus->addMenu(endGameMenu);
	menus->addMenu(confirmMenu);
	menus->addMenu(awkGhostSave);
	menus->addMenu(ghostSelect);
	


	/////////////////////////////////////////////////
	// Login Menu 
	//////////////////////////////////////////////////

	login->AddChooseString("Username",[lm](Ogre::String s) {lm->changeUsername(s); },"",15,false);
	login->AddChooseString("Password",[lm, this](Ogre::String s) {this->setFromConfigString(lm->changePassword(s));},"",15,true);
	login->AddSelectElement("Return to Main Menu", [login, mainMenu]() {login->disable(); mainMenu->enable();});
	
	/////////////////////////////////////////////////
	// Options Menu 
	//////////////////////////////////////////////////

    options->AddSelectElement("Control Options", [options, controlOptions]() {options->disable(); controlOptions->enable();});
    options->AddSelectElement("Gameplay Options", [options, gameplayOptions]() {options->disable(); gameplayOptions->enable();});
    options->AddSelectElement("Sound Options", [options, soundOptions]() {options->disable(); soundOptions->enable();});
    options->AddSelectElement("Advanced Options", [options, advancedOptions]() {options->disable(); advancedOptions->enable();});
	options->AddSelectElement("Return to Main Menu", [options, mainMenu]() {options->disable(); mainMenu->enable();});


	/////////////////////////////////////////////////
	// Options Submenu: Gameplay /  Obstacle 
	//////////////////////////////////////////////////

	obstacleMenu->AddChooseFloat("Obstacle Frequency", [w](float x) {w->setObstacleFrequency(x); }, 0.0f, 1.0f,w->getObstacleFrequency(), 0.1f, true);
    obstacleMenu->AddChooseInt("Minimum Obstacle Separation", [w](int x) {w->setObstacleSeparation(x); }, 0, 15, w->getObstacleSeparation(), 1, true);
	 obstacleMenu->AddChooseBool("Arrow Indicators", [h](bool show) {h->showArrows(show);}, h->arrowsShown(), true);
	std::vector<Ogre::String> namesArrowDist;
	std::vector<std::function<void()>> callbacksArrowDist;
	namesArrowDist.push_back("Close");
	callbacksArrowDist.push_back([p]() { p->setArrowDistance(1); });
	
	namesArrowDist.push_back("Medium");
	callbacksArrowDist.push_back([p,w]() {  { p->setArrowDistance(2); } });

	namesArrowDist.push_back("Far");
	callbacksArrowDist.push_back([p,w]() {  { p->setArrowDistance(3); } });

	namesArrowDist.push_back("Very Far");
	callbacksArrowDist.push_back([p,w]() {  { p->setArrowDistance(4); } });

	obstacleMenu->AddChooseEnum("Arrow Indicator Distance",namesArrowDist,callbacksArrowDist,p->getArrowDistance(), true);	


	std::vector<Ogre::String> namesTexture;
	std::vector<std::function<void()>> callbackTexture;
	namesTexture.push_back("Realistic");
	callbackTexture.push_back([w]() { w->setUsingSimpleMaterials(false); });
	
	namesTexture.push_back("Representational");
	callbackTexture.push_back([w]() {  w->setUsingSimpleMaterials(true); });

	obstacleMenu->AddChooseEnum("Blade Texture Type",namesTexture,callbackTexture,w->getUsingSimpleMaterials() ? 1 : 0, true);	
	obstacleMenu->AddSelectElement("Return to Gameplay Options", [obstacleMenu,gameplayOptions]() {obstacleMenu->disable(); gameplayOptions->enable();});

	
	/////////////////////////////////////////////////
	// Options Submenu:  Gameplay 
	//////////////////////////////////////////////////

   
    gameplayOptions->AddSelectElement("Obstacle Options", [gameplayOptions,obstacleMenu]() {gameplayOptions->disable(); obstacleMenu->enable();});
    gameplayOptions->AddChooseInt("Starting Armor", [p](int x) {p->setInitialArmor(x); }, 1, 8, p->getInitialArmor(), 1, true);
	gameplayOptions->AddChooseBool("Use Forward / Backward Leaning", [w, p](bool use) {w->setUseFrontBack(use); p->setUseFrontBack(use);} , p->getUseFrontBack(), true);
    gameplayOptions->AddChooseInt("Initial Speed", [p](int x) {p->setInitialSpeed(x); }, 5, 100, p->getInitialSpeed(), 5, true);
    gameplayOptions->AddChooseInt("Max Speed", [p](int x) {p->setMaxSpeed(x); }, 30, 100, p->getMaxSpeed(), 5, true);
    gameplayOptions->AddChooseInt("Auto Speed Increase Rate", [p](int x) {p->setAutoAceelerateRate(x); }, 0, 20, p->getAutoAccelerateRate(), 1, true);

	std::vector<Ogre::String> namesDuckLean;
	std::vector<std::function<void()>> callbacksDuckLean;
	namesDuckLean.push_back("Duck / Lean");
	callbacksDuckLean.push_back([p, w]() { p->setLeanEqualsDuck(true); w->setUseFrontBack(p->getUseFrontBack()); });
	
	namesDuckLean.push_back("Change Speed");
	callbacksDuckLean.push_back([p,w]() {  p->setLeanEqualsDuck(false); w->setUseFrontBack(false); });

	gameplayOptions->AddChooseEnum("Forward / Back Controls",namesDuckLean,callbacksDuckLean,0, true);	

    gameplayOptions->AddChooseInt("Manual Speed Change Rate", [p](int x) {p->setManualAceelerateRate(x); }, 0, 20, p->getManualAccelerateRate(), 1, true);

	gameplayOptions->AddSelectElement("Return to Options Menu", [gameplayOptions,options]() {gameplayOptions->disable(); options->enable();});



	
	/////////////////////////////////////////////////
	// Options Submenu:  Controls 
	//////////////////////////////////////////////////

    controlOptions->AddChooseBool("Callibrate Kinect Every Game", [p](bool x) {p->setAutoCallibrate(x); }, p->getAutoCallibrate(), true);
    controlOptions->AddChooseFloat("Kinect Sensitivity Left / Right", [p](float x) {p->setKinectSentitivityLR(x); }, 0.7f, 1.5f, 1.f, 0.1f, true);
    controlOptions->AddChooseFloat("Kinect Sensitivity Front / Back", [p](float x) {p->setKinectSentitivityFB(x); }, 0.7f, 1.5f, 1.f, 0.1f, true);
    controlOptions->AddSelectElement("Callibrate Kinect Now", [controlOptions, k]() {controlOptions->disable(); k->callibrate(4.0f, [controlOptions]() {controlOptions->enable();});});
    controlOptions->AddChooseBool("Invert Front/Back Controls", [p](bool x) {p->setInvertControls(x); }, p->getInvertControls(), true);
	controlOptions->AddChooseBool("Enable Kinect", [p](bool x) { p->setEnableKinect(x);  if (!x) p->setAutoCallibrate(false); }, p->getEnableKinect(), true);
	controlOptions->AddChooseBool("Enable Keyboard", [p](bool x) { p->setEnableKeyboard(x);}, p->getEnableKeyboard(), true);
	// controlOptions->AddChooseBool("Enable Gamepad", [p](bool x) { p->setEnableGamepad(x);}, p->getEnableGamepad());


    controlOptions->AddSelectElement("Return to Options Menu", [controlOptions,options]() {controlOptions->disable(); options->enable();});

	/////////////////////////////////////////////////
	// Options Submenu:  Sounds 
	//////////////////////////////////////////////////

    soundOptions->AddChooseBool("Enalbe Sound", [sb](bool x) {sb->setEnableSound(x); }, sb->getEnableSound(), true);
	soundOptions->AddChooseInt("Volume", [sb](int x) {sb->setVolume(x); }, 0, 128, sb->getVolume(), 5, true);

	std::vector<Ogre::String> namesSoundType;
	std::vector<std::function<void()>> callbacksSoundType;
	namesSoundType.push_back("Realistic (blades)");
	callbacksSoundType.push_back([sb]() {sb->setCurrentIndex(0); });

	namesSoundType.push_back("Representational (tones)");
	callbacksSoundType.push_back([sb]() {sb->setCurrentIndex(1); });

	soundOptions->AddChooseEnum("Sound Type",namesSoundType,callbacksSoundType,0, true);	
	soundOptions->AddSelectElement("Return to Options Menu", [soundOptions,options]() {soundOptions->disable(); options->enable();});

	/////////////////////////////////////////////////
	// Main Menu 
	//////////////////////////////////////////////////


	mainMenu->AddSelectElement("Start Game", [mainMenu,this]() { mainMenu->disable(); this->startGame(); });
	mainMenu->AddSelectElement("Play Against Saved Ghost", [mainMenu,this]() { mainMenu->disable(); this->loadGhost(); });
	mainMenu->AddSelectElement("Login", [mainMenu, login]() {mainMenu->disable(); login->enable();});
	mainMenu->AddSelectElement("Show Goals", [mainMenu, a]() {a-> ShowAllAchievements(true); mainMenu->disable();});
	mainMenu->AddSelectElement("Options", [options, mainMenu]() {options->enable(); mainMenu->disable();});
	mainMenu->AddSelectElement("Store", [store, mainMenu]() {store->enable(); mainMenu->disable();});
	mainMenu->AddSelectElement("Quit", [l, this]() {this->writeConfigStr(); l->quit();});

	/////////////////////////////////////////////////
	// Pause Menu 
	//////////////////////////////////////////////////


    pauseMenu->AddSelectElement("Continue", [pauseMenu, p]() {pauseMenu->disable(); p->setPaused(false); });
    pauseMenu->AddSelectElement("End Game (Return to Main Menu)", [pauseMenu,mainMenu, p, w, h, this]() {this->endGame(), h->showHUDElements(false); pauseMenu->disable();mainMenu->enable(); p->setPaused(true); });
    pauseMenu->AddSelectElement("Quit (Close Program)", [this, l]() {this->writeConfigStr();l->quit();});

	/////////////////////////////////////////////////
	// End Game / Ghost Menu 
	//////////////////////////////////////////////////


	endGameMenu->AddSelectElement("Replay Against Ghost", [this, endGameMenu]() {endGameMenu->disable(); this->replayGhost();});
	endGameMenu->AddSelectElement("Save Ghost", [ghost, awkGhostSave, endGameMenu]() {endGameMenu->disable(); ghost->writeFile(); awkGhostSave->enable();  });
    endGameMenu->AddSelectElement("Return to Main Menu", [endGameMenu,mainMenu, p, w, h, this]() {this->endGame(), h->showHUDElements(false); endGameMenu->disable();mainMenu->enable(); p->setPaused(true); });

	awkGhostSave->AddSelectElement("OK", [endGameMenu, awkGhostSave]() {endGameMenu->enable(); awkGhostSave->disable();});




	/////////////////////////////////////////////////
	// Options Submenu:  Advanced 
	//////////////////////////////////////////////////


	std::vector<Ogre::String> namesResolution;
	std::vector<std::function<void()>> callbacksResolution;
	namesResolution.push_back("Very Low");
	callbacksResolution.push_back([w]() { w->setUnitsPerPathLength(0.005f); });
	namesResolution.push_back("Low");
	callbacksResolution.push_back([w]() { w->setUnitsPerPathLength(0.01f); });
	namesResolution.push_back("Medium");
	callbacksResolution.push_back([w]() { w->setUnitsPerPathLength(0.05f); });
	namesResolution.push_back("High");
	callbacksResolution.push_back([w]() { w->setUnitsPerPathLength(0.1f); });
	advancedOptions->AddChooseEnum("Track Resolution",namesResolution,callbacksResolution,3, true);	
	advancedOptions->AddChooseInt("Track View Distance (segments)", [p](int x) {p->setTrackLookahed(x);}, 10, 200,  p->getTrackLookahead(), 5, true);
	    advancedOptions->AddSelectElement("Send Profile to Server", [this]() {this->writeConfigStr();});
    advancedOptions->AddSelectElement("Get Profile from Server", [this]() {this->readConfigStr();});
    advancedOptions->AddSelectElement("Reset Profile", [advancedOptions, confirmMenu]() {advancedOptions->disable();confirmMenu->enable();});
	advancedOptions->AddSelectElement("Return to Options Menu", [advancedOptions, options]() {advancedOptions->disable(); options->enable();});
    confirmMenu->AddSelectElement("Reset Profile (Cannot be undone!)", [this, p, w, a, advancedOptions, confirmMenu, menus]() {p->resetToDefaults();
																											   w->resetToDefaults(); 
																											   a->ResetAll();
																											   menus->resetMenus();
																											   this->setupMenus(false);});
    confirmMenu->AddSelectElement("Cancel Profile Reset", [advancedOptions, confirmMenu]() {advancedOptions->enable();confirmMenu->disable();});



	/////////////////////////////////////////////////
	// End of Menu Code
	//////////////////////////////////////////////////


	if (loginRequired)
	{
		login->enable();
	}
	else
	{
		mainMenu->enable();
	}

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
    // delete mWorld[1];
    // delete mAIManager;
    delete mRunnerCamera[0];
  //  delete mRunnerCamera[1];
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


