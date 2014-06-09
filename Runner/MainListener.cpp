#include "MainListener.h"
#include "AIManager.h"
#include "InputHandler.h"
#include "World.h"
#include "Camera.h"
#include "Kinect.h"
#include "XInputManager.h"
#include "Player.h"
#include "HUD.h"
#include "Menu.h"
#include <stdio.h>


MainListener::MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, PongCamera *cam, Kinect *sensor, XInputManager *gamepad, Player *player,  HUD *hud) :
//MainListener::MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, PongCamera *cam, XInputManager *gamepad, Player *player,  HUD *hud) :
mRenderWindow(window), mAIManager(aiManager), mWorld(world), mPongCamera(cam), mGamepad(gamepad), mPlayer(player), mHUD(hud),  mKinect(sensor)
{
	mPaused = false;
    mQuit = false;
	//mInputHandler->setFrameListener(this);
}


// On every frame, call the appropriate managers
bool 
	MainListener::frameStarted(const Ogre::FrameEvent &evt)
{
	
	float time = evt.timeSinceLastFrame;
	if (time > 0.5)
	{
		time = 0.5;
	}


	
 	mKinect->update(evt.timeSinceLastFrame);
	mHUD->update(time);
	InputHandler::getInstance()->Think(time);

	if (!mPaused)
	{
		mAIManager->Think(time);
		//  The only reason we have the Think method of the InputHandler return
		//   a value, is for the escape key to cause our application to end.
		//   Feel free to change this to something that makes more sense to you.
		mWorld->Think(time);
		mGamepad->update();
		mPlayer->Think(time);
		mPongCamera->Think(time);

		bool keepGoing = true;
	}
	MenuManager::getInstance()->think(time);

    bool keepGoing = true;

    if (InputHandler::getInstance()->IsKeyDown(OIS::KC_ESCAPE))
    {
        if (mKinect->callibrating())
        {
            mKinect->cancelCallibration();
            MenuManager::getInstance()->getMenu("main")->enable();
        }
        else if (MenuManager::getInstance()->getActiveMenu() != NULL)
        {
            // do nothing
        }
        else 
        {
            MenuManager::getInstance()->getMenu("pause")->enable();
            mPlayer->setPaused(true);
        }
    }

	// Ogre will shut down if a listener returns false.  We will shut everything down if
	// either the user presses escape or the main render window is closed.  Feel free to 
	// modify this as you like.
	if ( mRenderWindow->isClosed() || mQuit)
	{
		keepGoing = false;
	//	mKinect->shutdown();
	}
	return keepGoing;
}