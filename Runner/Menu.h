#ifndef __Menu_h_
#define __Menu_h_

#include "OgrePrerequisites.h"
#include "OgreColourValue.h"

namespace Ogre
{
	class OverlayElement;
	class Overlay;
	class TextAreaOverlayElement;
	class OverlayContainer;
}

typedef void (*SelectType)(void);  
typedef void (*ChooseInt)(int);  
typedef void (*ChooseBool)(bool);  

class InputHandler;


class Menu 
{

public:
	Menu();

	Menu(Ogre::String header, Ogre::String name, float xpos, float ypos);

	void enable();

	void disable();

	void AddSelectElement(Ogre::String name, SelectType);

	void AddChooseInt(Ogre::String name, ChooseInt);

	void AddChooseBool(Ogre::String name, ChooseBool);

	void think(float time);

	void setItemHeight(float height);
	void setItemSpacing(float spacing);

	Ogre::String name() { return mName; }


protected:
	class MenuItem;

	bool mEnabled;
	Ogre::String mName;

	float mItemHeight;
	float mItemSpacing;

	Ogre::Overlay *mMenuOverlay;
	Ogre::OverlayElement* mMenuTitle;
	Ogre::OverlayContainer *mPanel;

	float mStartingX;
	float mStartingY;

	std::vector<MenuItem *> mMenuItems;
	int mCurrentMenuItem;
	int mNumMenuItems; 
	Ogre::ColourValue mHighlightColor;
	Ogre::ColourValue mUnHighlightColor;

	class MenuItem
	{
	public:

		MenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y);

		virtual void Select();
		virtual void Deselect();
		virtual void Enter() { }
		virtual void Increase() { }
		virtual void Decrease() { }

	protected:
		Ogre::TextAreaOverlayElement *mItemText;
		Menu *mParent;

	};

	class SelectMenuItem : MenuItem
	{
	public:
		SelectMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, SelectType callback);
		virtual void Enter();

	protected:
		SelectType mCallback;

	};

	class ChooseIntMenuItem : public MenuItem
	{
	public:
		ChooseIntMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, ChooseInt callback);
		virtual void Increase();
		virtual void Decrease();
	protected:
		int mIntValue;
		ChooseInt mCallback;

	};

	class ChooseBoolMenuItem : public MenuItem
	{
	public:
		ChooseBoolMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, ChooseBool callback);

		virtual void Increase();
		virtual void Decrease();
		virtual void Enter();


	protected:
		bool mIntValue;
		ChooseBool mCallback;

	};



};

class MenuManager
{

public:
	static MenuManager* getInstance();
	void addMenu(Menu *menu, Ogre::String);
	Menu *getMenu(Ogre::String);
	Menu *getActiveMenu();
	void deactivateCurrentMenu();
	void activateMeu(Ogre::String);
	void think(float time);

private:
	MenuManager() { }
	static MenuManager *mInstance;
	std::map<Ogre::String, Menu*> mMenus;

};

#endif