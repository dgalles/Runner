#ifndef __InputHandler_h_
#define __InputHandler_h_

// #include <ois/ois.h>
#include "OIS/OISKeyboard.h"
namespace Ogre
{
    class RenderWindow;
}
class World;
class RunnerCamera;
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
	bool KeyPressedThisFrame(OIS::KeyCode key);

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