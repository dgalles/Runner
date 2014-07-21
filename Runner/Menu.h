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

	~Menu();

	void enable();

	void disable();

    bool enabled() { return mEnabled; }
	void AddSelectElement(Ogre::String name, std::function<void(void)> callback, bool save = false);

	void AddChooseInt(Ogre::String name,  std::function<void(int)> callback,  int minValue, int maxValue, int initialValue,  int delta = 1, bool save = false);
	void AddChooseFloat(Ogre::String name,  std::function<void(float)> callback, float minValue, float maxValue, float initialValue,  float delta, bool save = false);

	void AddChooseBool(Ogre::String name,  std::function<void(bool)> callback, bool intialValue = false, bool save = false);

	void AddChooseEnum(Ogre::String name, std::vector<Ogre::String> enumNames, std::vector<std::function<void()>> callbacks, int initialVal = 0, bool save = false);

	void AddChooseString(Ogre::String name, std::function<void(Ogre::String)> callback, Ogre::String initialValue, int maxlength, bool isPassword = false, bool save = false);

	void think(float time);

	float getItemSpacing() { return mItemSpacing; }

	void setItemHeight(float height);
	void setItemSpacing(float spacing);

	Ogre::String name() { return mName; }

	std::string getMenuConfig();
	void setMenuConfig(std::string configString);

	virtual void handleKeypress(const OIS::KeyEvent &e);
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
	std::map<std::string, int> mNameToIndexMap;

	class MenuItem 
	{
	public:

		MenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, bool saved);
		~MenuItem();
		virtual void Select();
		virtual void Deselect();
		virtual void Enter() { }
		virtual void Increase() { }
		virtual void Decrease() { }
		virtual void HandleKeypress(const OIS::KeyEvent &e) { }

		virtual void setValueFromString(std::string valueString) { }
		virtual std::string getValueAsString() { return ""; }
		std::string name() { return mText; }


		bool isSaved() { return mSaved;}

	protected:
		Ogre::TextAreaOverlayElement *mItemText;
		Menu *mParent;
		float mX;
		float mY;
		bool mSaved;
		bool mChanged;
		Ogre::String mText;
		Ogre::OverlayContainer *getPanelText() { return mParent->mPanelText;}

	};

	class SelectMenuItem : public MenuItem
	{
	public:
		SelectMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(void)> callback, bool save);
		virtual void Enter();

	protected:
		 std::function<void(void)> mCallback;

	};

	class ChooseIntMenuItem : public MenuItem
	{
	public:
		ChooseIntMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(int)> callback, int minValue, int maxValue, int initialValue, int delta, bool save);
		virtual void Increase();
		virtual void Decrease();
		virtual void setValueFromString(std::string valueString);
		virtual std::string getValueAsString();


	protected:
		int mIntValue;
		int mMinValue;
		int mMaxValue;
        int mDelta;
		std::function<void(int)>  mCallback;

	};



	class ChooseEnumMenuItem : public MenuItem
	{
	public:
		ChooseEnumMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::vector<Ogre::String> choiceNames, std::vector<std::function<void()>> callbacks, int initialValue, bool save);
		virtual void Increase();
		virtual void Decrease();
		virtual void setValueFromString(std::string valueString);
		virtual std::string getValueAsString();

		virtual void Enter() { Increase(); }

	protected:
		std::vector<Ogre::String> mChoiceNames;
		std::vector<std::function<void()>> mCallbacks;
		int mCurrentValue;
		std::function<void(int)>  mCallback;


	};




	class ChooseStringMenuItem : public MenuItem
	{
	public:
		ChooseStringMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(Ogre::String)> callback, Ogre::String initial, int maxLength, bool password, bool save);
		virtual void HandleKeypress(const OIS::KeyEvent &e);
		virtual void Enter();
		virtual void Deselect();
		virtual void Select();
		virtual void Increase();
		virtual void Decrease();

		virtual void setValueFromString(std::string valueString);
		virtual std::string getValueAsString();

	protected:
		Ogre::String mValue;
		Ogre::String mVisual;
		int mMaxLength;
		std::function<void(Ogre::String)>  mCallback;
		bool mPassword;

	};




	class ChooseFloatMenuItem : public MenuItem
	{
	public:
		ChooseFloatMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(float)> callback, float minValue, float maxValue, float initialValue, float delta, bool save);
		virtual void Increase();
		virtual void Decrease();
		virtual void setValueFromString(std::string valueString);
		virtual std::string getValueAsString();

	protected:
		float mFloatValue;
		float mMinValue;
		float mMaxValue;
		float mDelta;
		void updateOverlayFromValue();
		std::function<void(float)>  mCallback;

	};


	class ChooseBoolMenuItem : public MenuItem
	{
	public:
		ChooseBoolMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(bool)> callback, bool initialValue, bool save);

		virtual void Increase();
		virtual void Decrease();
		virtual void Enter();

				virtual void setValueFromString(std::string valueString);
		virtual std::string getValueAsString();



	protected:
		bool mBoolValue;
		std::function<void(bool)> mCallback;

	};



};



class ScrollSelectMenu : public Menu
{
public:

	ScrollSelectMenu(Ogre::String header, Ogre::String name,float xPos, float yPos, float ydelta, Menu *parent);
	~ScrollSelectMenu();
	void handleKeypress(const OIS::KeyEvent &e);
	void reset(std::vector<Ogre::String> items,  std::function<void(Ogre::String)> callback);

protected:
	std::vector<Ogre::TextAreaOverlayElement *> mElems;
	std::vector<Ogre::String> mNames;
	std::function<void(Ogre::String)> mSelectCallback;

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
	void resetMenus();

	bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);
	std::string getMenuConfig();
	void setMenuConfig(std::string configString);


private:
	MenuManager() { }
	static MenuManager *mInstance;
	std::map<std::string, Menu*> mMenus;
};

#endif