#include "Menu.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayElement.h"
#include "OgreOverlayContainer.h"
#include "OgreTextAreaOverlayElement.h"
#include "InputHandler.h"

MenuManager* MenuManager::mInstance = 0;


MenuManager* MenuManager::getInstance()
{
	if (mInstance == 0)
	{
		mInstance = new MenuManager();
	}
	return mInstance;
}


void MenuManager::addMenu(Menu *menu, Ogre::String) {}
Menu *MenuManager::getMenu(Ogre::String) { return 0; }
Menu *MenuManager::getActiveMenu() { return 0;}
void MenuManager::deactivateCurrentMenu() {}
void MenuManager::activateMeu(Ogre::String){}
void 
	MenuManager::think(float time)
{
	for (std::map<Ogre::String, Menu*>::iterator it = mMenus.begin();
		 it != mMenus.end();
		 it++)
	{
		(*it).second->think(time);

	}
}


Menu::Menu()
{
	mItemHeight = 0.05f;
	mItemSpacing = 0.1f;

}

void Menu::think(float time)
{
	if (mEnabled)

	{


	}

}

Menu::Menu(Ogre::String header, Ogre::String name, float xPos, float yPos)
	: mHighlightColor(1.0f,0.0f, 0.0f), mUnHighlightColor(1.0f, 1.0f, 1.0f)
{

	mStartingX = xPos;
	mStartingY = yPos;
	mNumMenuItems = 0;
	mName = name;



	mItemHeight = 0.05f;
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
         // Create an overlay
     mMenuOverlay = overlayManager.create( "OverlayName" );
 
	 // Create a panel
	 mPanel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"Panel" ) );
	 mPanel->setPosition(xPos, yPos );
	 mPanel->setDimensions( 0.8f, 0.8f );
	 mPanel->setMaterialName( "Kinect/Blue" );
	 // Add the panel to the overlay
	 mMenuOverlay->add2D( mPanel );


	 Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(
		 overlayManager.createOverlayElement("TextArea", name + "Header"));
	textArea->setPosition(mStartingX, mStartingY);
	textArea->setCaption(header);
	textArea->setCharHeight(mItemHeight);
	textArea->setFontName("Big");
	textArea->setColour(mUnHighlightColor);
	mPanel->addChild(textArea);

	 // Show the overlay
	 mMenuOverlay->show();

}

void Menu::enable()
{
	mEnabled = true;
	mMenuOverlay->show();
}

void Menu::disable()
{
	mMenuOverlay->show();
	mEnabled = false;
}

void Menu::AddSelectElement(Ogre::String text, SelectType callback)
{
	mNumMenuItems++;

	float x = mStartingX;
	float y = mStartingY + mNumMenuItems * mItemSpacing; 

	Menu::SelectMenuItem *item = new Menu::SelectMenuItem(text, mName + std::to_string(mNumMenuItems),this,x,y,callback); 
}

void Menu::AddChooseInt(Ogre::String name, ChooseInt)
{
}


void Menu::AddChooseBool(Ogre::String name, ChooseBool)
{
}

void  Menu::MenuItem::Select() 
{
	mItemText->setColour(mParent->mHighlightColor);
}

void  Menu::MenuItem::Deselect() 
{
	mItemText->setColour(mParent->mUnHighlightColor);
}


Menu::MenuItem::MenuItem(Ogre::String text, Ogre::String name,  Menu *parent, float x, float y)
	: mParent(parent)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	mItemText = static_cast<Ogre::TextAreaOverlayElement*>(
	overlayManager.createOverlayElement("TextArea", name));
	mItemText->setPosition(x, y);
	mItemText->setCaption(text);
	mItemText->setCharHeight(parent->mItemHeight);
	mItemText->setFontName("Big");
	mItemText->setColour(Ogre::ColourValue(1,1,1));
	mParent->mPanel->addChild(mItemText);
}

Menu::SelectMenuItem::SelectMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, SelectType callback)
	: Menu::MenuItem(text, name, parent, x , y), mCallback(callback)
{

}

void  Menu::SelectMenuItem::Enter() { }


Menu::ChooseIntMenuItem::ChooseIntMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, ChooseInt callback)
	: Menu::MenuItem(text, name, parent, x, y), mCallback(callback)
{
}
void Menu::ChooseIntMenuItem::Increase() { }
void Menu::ChooseIntMenuItem::Decrease() { }



Menu::ChooseBoolMenuItem::ChooseBoolMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, ChooseBool callback)
	: Menu::MenuItem(text, name, parent,  x, y), mCallback(callback)
{
}
void Menu::ChooseBoolMenuItem::Increase() { }
void Menu::ChooseBoolMenuItem::Decrease() { }
void Menu::ChooseBoolMenuItem::Enter() { }