#ifndef __InputHandler_h_
#define __InputHandler_h_

#include <ois/ois.h>
#include <Ogre.h>
#include "OgreRenderWindow.h"

class World;
class PongCamera;
class InputHandler;


class InputHandler // : public OIS::MouseListener, public OIS::KeyListener
{
public:
	static InputHandler *getInstance();
	static void destroyInstance();
	void initialize(Ogre::RenderWindow* win);
	~InputHandler();
	void Think(float time);

	bool IsKeyDown(OIS::KeyCode key);
	bool WasKeyDown(OIS::KeyCode key);

protected:
	
	bool mInitialized;
	InputHandler();
	OIS::InputManager* mInputManager;
	Ogre::RenderWindow *mRenderWindow;
	OIS::Keyboard *mCurrentKeyboard;
	char mOldKeys[256];
	static InputHandler *mInstance;

};

#endif