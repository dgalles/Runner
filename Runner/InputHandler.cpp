#include "InputHandler.h"
#include "Ogre.h"
#include "OgreStringConverter.h"
#include <OIS/OIS.h>

#include <stdio.h>


InputHandler* InputHandler::mInstance = 0;


void 
InputHandler::destroyInstance()
{
	if (mInstance != 0)
	{
		delete mInstance;
		mInstance = 0;
	}
}


InputHandler* InputHandler::getInstance()
{
	if (mInstance == 0)
	{
		mInstance = new InputHandler();
	}
	return mInstance;
}

InputHandler::InputHandler() : 
	 mInitialized(false)
{

}

void 
InputHandler::initialize(Ogre::RenderWindow* win)
{
	if (mInitialized)
	{
		// Should probably throw an exception here ...
		return;
	}
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	mCurrentKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, false /* not buffered */ ));
	mInitialized = true;
}


bool
InputHandler::IsKeyDown(OIS::KeyCode key)
{
	if (!mInitialized)
	{
		// Throw exception here ...
		return false;
	}
	return mCurrentKeyboard->isKeyDown(key);
}

bool
InputHandler::WasKeyDown(OIS::KeyCode key)
{
	if (!mInitialized)
	{
		// Throw exception here ...
		return false;
	}

	return mOldKeys[key] != '\0';
}

void 
InputHandler::Think(float time)
{
	if (!mInitialized)
	{
		// Throw exception here ...
		return;
	}

	int x = 0;
	mCurrentKeyboard->copyKeyStates(mOldKeys);
    if (mCurrentKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		x++;
	}
	mCurrentKeyboard->capture();
}


InputHandler::~InputHandler()
{
	if (mInitialized)
	{
		mInputManager->destroyInputObject(mCurrentKeyboard);
	}
}

