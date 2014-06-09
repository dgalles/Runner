
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


void MenuManager::addMenu(Menu *menu) 
{
	mMenus[menu->name()] = menu;
}

Menu *MenuManager::getMenu(Ogre::String name) 
{ 
	return mMenus[name];
}

Menu *MenuManager::getActiveMenu() 
{	

	for (std::map<Ogre::String, Menu*>::iterator it = mMenus.begin();
		 it != mMenus.end();
		 it++)
	{
        if ((*it).second->enabled())
        {
		   return (*it).second;
        }
	}
    return 0;
}

void MenuManager::deactivateCurrentMenu() {}
void MenuManager::activateMeu(Ogre::String){}
void 
MenuManager::think(float time)
{
	for (std::map<Ogre::String, Menu*>::iterator it = mMenus.begin();
		 it != mMenus.end();
		 it++)
	{
        if ((*it).second->enabled())
        {
		    (*it).second->think(time);
            break;
        }
	}
}




Menu::Menu()
{


}

void Menu::think(float time)
{
	if (mEnabled)

	{
		InputHandler *ih = InputHandler::getInstance();
		if (ih->KeyPressedThisFrame(OIS::KC_DOWN))
		{
			if (mCurrentMenuItem + 1 < mNumMenuItems)
			{
				mMenuItems[mCurrentMenuItem]->Deselect();
				mCurrentMenuItem++;
				mMenuItems[mCurrentMenuItem]->Select();
			}
		}
		if (ih->KeyPressedThisFrame(OIS::KC_UP))
		{
			if (mCurrentMenuItem > 0)
			{
				mMenuItems[mCurrentMenuItem]->Deselect();
				mCurrentMenuItem--;
				mMenuItems[mCurrentMenuItem]->Select();
			}
		}
		if (ih->KeyPressedThisFrame(OIS::KC_RETURN))
		{
			mMenuItems[mCurrentMenuItem]->Enter();
		}
		if (ih->KeyPressedThisFrame(OIS::KC_RIGHT))
		{
			mMenuItems[mCurrentMenuItem]->Increase();
		}
		if (ih->KeyPressedThisFrame(OIS::KC_LEFT))
		{
			mMenuItems[mCurrentMenuItem]->Decrease();
		}

	}

}

Menu::Menu(Ogre::String header, Ogre::String name, float xPos, float yPos, float ydelta)
	: mHighlightColor(1.0f,0.0f, 0.0f), mUnHighlightColor(1.0f, 1.0f, 1.0f)
{
	mItemHeight = 0.05f;
	mItemSpacing = ydelta;
	mCurrentMenuItem = 0;
	mNumMenuItems = 0;
	mStartingX = xPos;
	mStartingY = yPos;
	mName = name;



	mItemHeight = 0.05f;
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
         // Create an overlay
     mMenuOverlay = overlayManager.create( "Menu"+name );
 
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
	if (mCurrentMenuItem != 0)
	{
		mMenuItems[mCurrentMenuItem]->Deselect();
		mCurrentMenuItem = 0;
		mMenuItems[mCurrentMenuItem]->Select();
	}
}

void Menu::disable()
{
	mMenuOverlay->hide();
	mEnabled = false;
}


void Menu::AddMenuItem(MenuItem *item)
{
	mNumMenuItems++;
	mMenuItems.push_back(item);

	if (mNumMenuItems == 1)
	{
		item->Select();
	}
}

void Menu::AddSelectElement(Ogre::String text,  std::function<void(void)> callback)
{
	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems + 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::SelectMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback));
}

void Menu::AddChooseInt(Ogre::String text,  std::function<void(int)> callback,  int minValue, int maxValue, int initialValue, int delta)
{

	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems + 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseIntMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback,  minValue, maxValue,initialValue, delta));
}


void Menu::AddChooseFloat(Ogre::String text,  std::function<void(float)> callback,  float minValue, float maxValue, float initialValue, float delta)
{

	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems + 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseFloatMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback,  minValue, maxValue, initialValue, delta));
}



void Menu::AddChooseBool(Ogre::String text,  std::function<void(bool)> callback, bool initialValue /* = false */)
{
	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems + 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseBoolMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback, initialValue));
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

Menu::SelectMenuItem::SelectMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(void)> callback)
	: Menu::MenuItem(text, name, parent, x , y), mCallback(callback)
{

}

void  Menu::SelectMenuItem::Enter() 
{ 
	mCallback();
}



    //using namespace std::placeholders; // for `_1`

    //private_x = 5;
    //handler->addHandler(std::bind(&MyClass::Callback, this, _1));

// or

// handler->addHandler([](int x) { std::cout << "x is " << x << '\n'; });


Menu::ChooseIntMenuItem::ChooseIntMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(int)> callback,  int minValue, int maxValue, int initialValue, int delta)
	: Menu::MenuItem(text, name, parent, x, y), mCallback(callback), mIntValue(initialValue), mText(text), mMinValue(minValue), mMaxValue(maxValue), mDelta(delta)
{
	mItemText->setCaption(mText + ":" + "  " + std::to_string(mIntValue));
}
void Menu::ChooseIntMenuItem::Increase() 
{ 
	if (mIntValue < mMaxValue)
	{
		mIntValue+= mDelta;
        mIntValue = std::min(mIntValue,  mMaxValue);
		mItemText->setCaption(mText + ":" + "  " + std::to_string(mIntValue));
		mCallback(mIntValue);
	}
}
void Menu::ChooseIntMenuItem::Decrease() 
{ 
    if (mIntValue > mMinValue)
    {
        mIntValue -= mDelta;
        mIntValue = std::max(mIntValue,  mMinValue);

        mItemText->setCaption(mText + ":" + "  " + std::to_string(mIntValue));
        mCallback(mIntValue);
	}
}



Menu::ChooseFloatMenuItem::ChooseFloatMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(float)> callback, 
                                               float minValue, float maxValue, float initialValue, float delta)
	: Menu::MenuItem(text, name, parent, x, y), mCallback(callback), mFloatValue(initialValue), mText(text), mMinValue(minValue), mMaxValue(maxValue),mDelta(delta)
{

    updateOverlayFromValue();
}


void Menu::ChooseFloatMenuItem::updateOverlayFromValue()
{
    char buff[100];
    sprintf(buff,"%.2f",mFloatValue);
	mItemText->setCaption(mText + ":" + "  " +Ogre::String(buff));
}
void Menu::ChooseFloatMenuItem::Increase() 
{ 
	if (mFloatValue < mMaxValue)
	{
		mFloatValue += mDelta;
        mFloatValue = std::min(mFloatValue, mMaxValue);
		updateOverlayFromValue();
		mCallback(mFloatValue);
	}
}
void Menu::ChooseFloatMenuItem::Decrease() 
{ 
	if (mFloatValue > mMinValue)
	{
        mFloatValue -= mDelta;
        mFloatValue = std::max(mFloatValue, mMinValue);
        updateOverlayFromValue();
        mCallback(mFloatValue);
    }
}



Menu::ChooseBoolMenuItem::ChooseBoolMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(bool)> callback, bool initialValue)
	: Menu::MenuItem(text, name, parent,  x, y), mCallback(callback), mBoolValue(initialValue), mText(text)
{
	if (mBoolValue)
	{
		mItemText->setCaption(mText + ": True");

	}
	else
	{
		mItemText->setCaption(mText + ": False");

	}
}
void Menu::ChooseBoolMenuItem::Increase() 
{ 
	Enter();
}
void Menu::ChooseBoolMenuItem::Decrease() 
{
	Enter();
}
void Menu::ChooseBoolMenuItem::Enter() 
{ 
	mBoolValue = !mBoolValue;
	if (mBoolValue)
	{
		mItemText->setCaption(mText + ": True");
	}
	else
	{
		mItemText->setCaption(mText + ": False");
	}
    mCallback(mBoolValue);
}