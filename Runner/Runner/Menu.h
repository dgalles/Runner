#ifndef __Menu_h_
#define __Menu_h_

#include "OgrePrerequisites.h"
#include "OgreColourValue.h"
#include <OIS/OIS.h>

namespace Ogre
{
	class OverlayElement;
	class Overlay;
	class TextAreaOverlayElement;
	class OverlayContainer;
}

class InputHandler;


class Menu 
{

public:
	Menu();

	Menu(Ogre::String header, Ogre::String name, float xpos, float ypos, float ydelta = 0.1f, Menu *parent = NULL, bool beginEnabled = false);

	void enable();

	void disable();

    bool enabled() { return mEnabled; }
	void AddSelectElement(Ogre::String name, std::function<void(void)> callback);

	void AddChooseInt(Ogre::String name,  std::function<void(int)> callback,  int minValue, int maxValue, int initialValue,  int delta = 1);
	void AddChooseFloat(Ogre::String name,  std::function<void(float)> callback, float minValue, float maxValue, float initialValue,  float delta);

	void AddChooseBool(Ogre::String name,  std::function<void(bool)> callback, bool intialValue = false);

	void AddChooseEnum(Ogre::String name, std::vector<Ogre::String> enumNames, std::vector<std::function<void()>> callbacks, int initialVal = 0);

	void AddChooseString(Ogre::String name, std::function<void(Ogre::String)> callback, Ogre::String initialValue, int maxlength, bool isPassword = false);

	void think(float time);

	void setItemHeight(float height);
	void setItemSpacing(float spacing);

	Ogre::String name() { return mName; }



	void handleKeypress(const OIS::KeyEvent &e);

protected:
	class MenuItem;

	bool mEnabled;
	Ogre::String mName;

	float mItemHeight;
	float mItemSpacing;

	Ogre::Overlay *mMenuOverlay;
	Ogre::OverlayContainer *mMenuHighlight;
	Ogre::OverlayElement* mMenuTitle;
	Ogre::OverlayContainer *mPanel;
	Ogre::OverlayContainer *mPanelText;

	float mStartingX;
	float mStartingY;

	std::vector<MenuItem *> mMenuItems;
	int mCurrentMenuItem;
	int mNumMenuItems; 
	Ogre::ColourValue mHighlightColor;
	Ogre::ColourValue mUnHighlightColor;

	Menu *mParent;

	void AddMenuItem(MenuItem *item);
	void moveSelectUp();
	void moveSelectDown();


	class MenuItem 
	{
	public:

		MenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y);

		virtual void Select();
		virtual void Deselect();
		virtual void Enter() { }
		virtual void Increase() { }
		virtual void Decrease() { }
		virtual void HandleKeypress(const OIS::KeyEvent &e) { }

	protected:
		Ogre::TextAreaOverlayElement *mItemText;
		Menu *mParent;
		float mX;
		float mY;
	};

	class SelectMenuItem : public MenuItem
	{
	public:
		SelectMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(void)> callback);
		virtual void Enter();

	protected:
		 std::function<void(void)> mCallback;

	};

	class ChooseIntMenuItem : public MenuItem
	{
	public:
		ChooseIntMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(int)> callback, int minValue, int maxValue, int initialValue, int delta);
		virtual void Increase();
		virtual void Decrease();

	protected:
		int mIntValue;
		int mMinValue;
		int mMaxValue;
        int mDelta;
		Ogre::String mText;
		std::function<void(int)>  mCallback;

	};



	class ChooseEnumMenuItem : public MenuItem
	{
	public:
		ChooseEnumMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::vector<Ogre::String> choiceNames, std::vector<std::function<void()>> callbacks, int initialValue);
		virtual void Increase();
		virtual void Decrease();
		virtual void Enter() { Increase(); }

	protected:
		std::vector<Ogre::String> mChoiceNames;
		std::vector<std::function<void()>> mCallbacks;
		int mCurrentValue;
		Ogre::String mText;
		std::function<void(int)>  mCallback;

	};




	class ChooseStringMenuItem : public MenuItem
	{
	public:
		ChooseStringMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(Ogre::String)> callback, Ogre::String initial, int maxLength, bool password);
		virtual void HandleKeypress(const OIS::KeyEvent &e);
		virtual void Enter();
		virtual void Deselect();

		virtual void Increase();
		virtual void Decrease();
	protected:
		Ogre::String mValue;
		Ogre::String mVisual;
		Ogre::String mText;
		int mMaxLength;
		std::function<void(Ogre::String)>  mCallback;
		bool mPassword;

	};




    class ChooseFloatMenuItem : public MenuItem
	{
	public:
		ChooseFloatMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(float)> callback, float minValue, float maxValue, float initialValue, float delta);
		virtual void Increase();
		virtual void Decrease();
	protected:
		float mFloatValue;
		float mMinValue;
		float mMaxValue;
        float mDelta;
        void updateOverlayFromValue();
		Ogre::String mText;
		std::function<void(float)>  mCallback;

	};


	class ChooseBoolMenuItem : public MenuItem
	{
	public:
		ChooseBoolMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(bool)> callback, bool initialValue);

		virtual void Increase();
		virtual void Decrease();
		virtual void Enter();


	protected:
		bool mBoolValue;
		Ogre::String mText;

		std::function<void(bool)> mCallback;

	};



};

class MenuManager : public  OIS::KeyListener
{

public:
	static MenuManager* getInstance();
	void addMenu(Menu *menu);
	Menu *getMenu(Ogre::String);
	Menu *getActiveMenu();
	void deactivateCurrentMenu();
	void activateMeu(Ogre::String);
	void think(float time);

	bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);


private:
	MenuManager() { }
	static MenuManager *mInstance;
	std::map<Ogre::String, Menu*> mMenus;
};

#endif