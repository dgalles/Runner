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
#include "Achievements.h"
#include <stdio.h>


MainListener::MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world[], RunnerCamera *cams[], Kinect *sensor, XInputManager *gamepad, Player *player[],  HUD *hud, Achievements *ach[]) :
//MainListener::MainListener(Ogre::RenderWindow *window, AIManager *aiManager, World *world, RunnerCamera *cam, XInputManager *gamepad, Player *player,  HUD *hud) :
mRenderWindow(window), mAIManager(aiManager), mGamepad(gamepad), mHUD(hud),  mKinect(sensor)
{
	for (int i = 0; i < 2; i++)
	{
		mRunnerCamera[i] = cams[i];
		mWorld[i] = world[i];
		mPlayer[i] = player[i];
		mAchievements[i] = ach[i];
	}
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
		for (int i = 0; i < 1; i++)
		{
			mWorld[i]->Think(time);
			mPlayer[i]->Think(time);
			mRunnerCamera[0]->Think(time);
			mAchievements[i]->Think(time);
		}
		mGamepad->update();
	}
	MenuManager::getInstance()->think(time);

    bool keepGoing = true;

    if (InputHandler::getInstance()->IsKeyDown(OIS::KC_ESCAPE))
    {
        if (mKinect->callibrating())
        {
            mKinect->cancelCallibration();
        }
        else if (MenuManager::getInstance()->getActiveMenu() != NULL)
        {
            // do nothing
        }
        else if (mAchievements[0]->ShowingAllAchievements())
		{
			mAchievements[0]->ShowAllAchievements(false);
            MenuManager::getInstance()->getMenu("main")->enable();

		}
		else
        {
			mAchievements[0]->clearUI();
            MenuManager::getInstance()->getMenu("pause")->enable();
            mPlayer[0]->setPaused(true);
          //  mPlayer[1]->setPaused(true);
			mWorld[0]->getHUD()->setShowDecreaseSpeed(false);
			mWorld[0]->getHUD()->setShowIncreaseSpeed(false);
			//mWorld[1]->getHUD()->setShowDecreaseSpeed(false);
			//mWorld[1]->getHUD()->setShowIncreaseSpeed(false);
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