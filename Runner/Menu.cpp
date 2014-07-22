
#include "Menu.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayElement.h"
#include "OgreOverlayContainer.h"
#include "OgreTextAreaOverlayElement.h"
#include "InputHandler.h"
#include <sstream>
#include "JsonUtils.h"

MenuManager* MenuManager::mInstance = 0;




MenuManager* MenuManager::getInstance()
{
	if (mInstance == 0)
	{
		mInstance = new MenuManager();
	}
	return mInstance;
}



bool MenuManager::keyPressed(const OIS::KeyEvent &e)
{
		for (std::map<Ogre::String, Menu*>::iterator it = mMenus.begin();
		 it != mMenus.end();
		 it++)
		{
			if ((*it).second->enabled())
			{
				(*it).second->handleKeypress(e);
				break;
			}
		}
		return true;
}
bool  MenuManager::keyReleased(const OIS::KeyEvent &e)
{
	return true;
}

void MenuManager::resetMenus()
{
	for (std::map<Ogre::String, Menu*>::iterator it = mMenus.begin();
		it != mMenus.end();
		it++)
	{
		delete (*it).second;
	}
	mMenus.clear();
}



std::string  MenuManager::getMenuConfig()
{
	bool first = true;
	std::string result = "{";
	std::map<Ogre::String, Menu*>::iterator it = mMenus.begin();
	while (it != mMenus.end())
	{
		std::string key = (*it).first;
		Menu* menu = (*it).second;
		if (first)
		{
			first = false;
		}
		else
		{
			result = result + ",";
		}

		result = result + "\"" + key + "\" : " + menu->getMenuConfig();
		it++;

	}
	result = result + "}";
	return result;

}
void MenuManager::setMenuConfig(std::string configString)
{
	std::size_t braceIndex = configString.find_first_of('{');

	std::string remainder = configString.substr(braceIndex+1);

	std::size_t nextIndex = remainder.find_first_not_of("\t \n");
	while (nextIndex != std::string::npos && remainder[nextIndex] != '}')
	{
		std::string nextKey = JSON_UTIL::firstItem(remainder);
		nextKey = JSON_UTIL::stripQuotes(nextKey);
		remainder = JSON_UTIL::removeFirstitem(remainder);
	
		size_t colonIndex = remainder.find_first_of(":");
		if (colonIndex == std::string::npos)
		{
			// ERROR!
			nextIndex = std::string::npos;
			break;
		}
		remainder = remainder.substr(colonIndex+1);
		std::string value = JSON_UTIL::firstItem(remainder);
		remainder = JSON_UTIL::removeFirstitem(remainder);
		if (mMenus.find(nextKey) != mMenus.end())
		{
			mMenus[nextKey]->setMenuConfig(value);
		}
		nextIndex = remainder.find_first_not_of("\t \n");
		if (nextIndex != std::string::npos && remainder[nextIndex] == ',')
			nextIndex++;
		remainder = remainder.substr(nextIndex);
		nextIndex = remainder.find_first_not_of("\t \n");
	}

}



std::string Menu::getMenuConfig()
{
	bool first = true;
	std::string result = "{";
	for (unsigned int i = 0; i < mMenuItems.size(); i++)
	{
		if (mMenuItems[i]->isSaved())
		{
			if (first)
			{
				first = false;
			}
			else
			{
				result = result + ",";
			}
			result = result + "\"" + mMenuItems[i]->name() + "\" : " + mMenuItems[i]->getValueAsString();
		}
	}
	result = result + "}";
	return result;
}

void Menu::setMenuConfig(std::string configString)
{
	std::size_t braceIndex = configString.find_first_of('{');

	std::string remainder = configString.substr(braceIndex+1);

	std::size_t nextIndex = remainder.find_first_not_of("\t \n");
	while (nextIndex != std::string::npos && remainder[nextIndex] != '}')
	{
		std::string nextKey = JSON_UTIL::stripQuotes(JSON_UTIL::firstItem(remainder));

		remainder = JSON_UTIL::removeFirstitem(remainder);
	
		size_t colonIndex = remainder.find_first_of(":");
		if (colonIndex == std::string::npos)
		{
			// ERROR!
			nextIndex = std::string::npos;
			break;
		}
		remainder = remainder.substr(colonIndex+1);
		std::string value = JSON_UTIL::firstItem(remainder);
		remainder = JSON_UTIL::removeFirstitem(remainder);
		if (mNameToIndexMap.find(nextKey) != mNameToIndexMap.end())
		{
			mMenuItems[mNameToIndexMap[nextKey]]->setValueFromString(value);
		}
		nextIndex = remainder.find_first_not_of("\t \n");
		if (nextIndex != std::string::npos && remainder[nextIndex] == ',')
			nextIndex++;
		remainder = remainder.substr(nextIndex);
		nextIndex = remainder.find_first_not_of("\t \n");

	}
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

void Menu::moveSelectDown()
{
	if (mCurrentMenuItem + 1 < mNumMenuItems)
	{
		mMenuItems[mCurrentMenuItem]->Deselect();
		mCurrentMenuItem++;
		mMenuItems[mCurrentMenuItem]->Select();
	}

}

void Menu::moveSelectUp()
{

	if (mCurrentMenuItem > 0)
	{
		mMenuItems[mCurrentMenuItem]->Deselect();
		mCurrentMenuItem--;
		mMenuItems[mCurrentMenuItem]->Select();
	}
}

void Menu::handleKeypress(const OIS::KeyEvent &e)
{
	if (e.key == OIS::KC_DOWN)
	{
			moveSelectDown();

		}
		else if (e.key == OIS::KC_UP)
		{
			moveSelectUp();
		}
		else if (e.key == OIS::KC_RETURN)
		{
			mMenuItems[mCurrentMenuItem]->Enter();
		}
		else if (e.key == OIS::KC_RIGHT)
		{
			mMenuItems[mCurrentMenuItem]->Increase();
		}
		else if (e.key == OIS::KC_LEFT)
		{
			mMenuItems[mCurrentMenuItem]->Decrease();
		}
		else if (e.key == OIS::KC_ESCAPE)
		{
			if (mParent != NULL)
			{
				disable();
				mParent->enable();

			}
		}
		else 
		{
			mMenuItems[mCurrentMenuItem]->HandleKeypress(e);
		}



}

void Menu::think(float time)
{	
	if (mEnabled)
	{

	}

}


Menu::~Menu()
{


	for (unsigned int i = 0; i < mMenuItems.size(); i++)
	{
		delete mMenuItems[i];	
	}

	mMenuItems.clear();

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	overlayManager.destroyOverlayElement( mName + "Header");
	overlayManager.destroyOverlayElement(mMenuHighlight);
	overlayManager.destroyOverlayElement(mPanelText);
	overlayManager.destroyOverlayElement(mPanel);
	overlayManager.destroy(mMenuOverlay);
}


ScrollSelectMenu::ScrollSelectMenu(Ogre::String header, Ogre::String name,  
								    float xPos, float yPos, float ydelta, Menu *parent)
	: Menu(header, name, xPos, yPos, ydelta, parent, false), mNames()
{


}

void ScrollSelectMenu::reset(std::vector<Ogre::String> items,  std::function<void(Ogre::String)> callback)
{

	mSelectCallback = callback;
	mNames = items;

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();


	for (unsigned int i = 0; i < mElems.size(); i++)
	{
		mElems[i]->setCaption("");
	}
	for (unsigned int i = mElems.size(); i < items.size()+1; i++)
	{

		Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(
			overlayManager.createOverlayElement("TextArea", mName + "_" + std::to_string(i)));

		textArea->setCaption("");
		textArea->setCharHeight(mItemHeight * 1.5f);
		textArea->setFontName("Big");
		textArea->setCharHeight(mItemHeight);
		mPanelText->addChild(textArea);
		mElems.push_back(textArea);
	}
	for (unsigned int i = 0; i < items.size(); i++)
	{
		mElems[i+1]->setCaption(items[i]);
		float x = mStartingX;
		float y = mStartingY + ((i+1) + 1.5f) * mItemSpacing; 
		mElems[i+1]->setPosition(x, y);
		mElems[i+1]->setColour(mUnHighlightColor);
		mElems[i+1]->setCharHeight(mItemHeight);
	}
	mElems[0]->setCaption("Cancel Loading Ghost");

	float yPos =  mStartingY + ((0) + 1.5f) * mItemSpacing;	
	mElems[0]->setCharHeight(mItemHeight * 1.5f);
	mElems[0]->setColour(mHighlightColor);
	mElems[0]->setPosition(mStartingX, yPos -mItemHeight * 0.5f / 2.0f);

	mCurrentMenuItem = 0;
	mNumMenuItems = items.size()+1;
	mElems[mCurrentMenuItem]->setColour(mHighlightColor);	
	mMenuHighlight->setPosition(0, mStartingY + (mCurrentMenuItem + 1.5f) * mItemSpacing -mItemHeight * 0.5f / 2.0f - 0.01f);

	float height = (mNumMenuItems + 1.5f) * mItemSpacing + mItemHeight + mStartingY;
	mPanel->setHeight(height);
}


void ScrollSelectMenu::handleKeypress(const OIS::KeyEvent &e)
{
	if (e.key == OIS::KC_DOWN || e.key == OIS::KC_UP)
	{
		int oldIndex = mCurrentMenuItem;

		if (e.key == OIS::KC_DOWN)
		{
			mCurrentMenuItem++;
			mCurrentMenuItem = std::min(mCurrentMenuItem, mNumMenuItems - 1);
		}
		else
		{
			mCurrentMenuItem--;
			mCurrentMenuItem = std::max(mCurrentMenuItem, 0);
		}

		mElems[oldIndex]->setColour(mUnHighlightColor);
		mElems[oldIndex]->setCharHeight(mItemHeight);
		mElems[oldIndex]->setPosition(mStartingX,  mStartingY + ((oldIndex) + 1.5f) * mItemSpacing);

		float yPos =  mStartingY + ((mCurrentMenuItem) + 1.5f) * mItemSpacing;	
		mElems[mCurrentMenuItem]->setCharHeight(mItemHeight * 1.5f);
		mElems[mCurrentMenuItem]->setColour(mHighlightColor);
		mElems[mCurrentMenuItem]->setPosition(mStartingX, yPos -mItemHeight * 0.5f / 2.0f);

		mMenuHighlight->setPosition(0, yPos - mItemHeight * 0.5f / 2.0f- 0.01f);


		float bottomPos = 2 * mStartingY + (mCurrentMenuItem +1.5f) * mItemSpacing + mItemSpacing;
		if 	 (bottomPos > 0.95f)
		{
			mMenuOverlay->setScroll(0, (bottomPos- .95f) *  2);
		}
		else 
		{
			mMenuOverlay->setScroll(0,0);
		}
	}

	else if (e.key == OIS::KC_RETURN)
	{
		if (mCurrentMenuItem == 0)
		{
			disable();
			if (mParent != NULL)
			{
				mParent->enable();
			}

		}
		else
		{
			mSelectCallback(mNames[mCurrentMenuItem - 1]);

		}
	}
	else if (e.key == OIS::KC_ESCAPE)
	{
		if (mParent != NULL)
		{
			disable();
			mParent->enable();

		}
	}
}

ScrollSelectMenu::~ScrollSelectMenu()
{


}



Menu::Menu(Ogre::String header, Ogre::String name, float xPos, float yPos, float ydelta, Menu *parent, bool beginEnabled)
	: mHighlightColor(1.0f,0.0f, 0.0f), mUnHighlightColor(1.0f, 1.0f, 1.0f)
{
	mItemHeight = 0.05f;
	mItemSpacing = ydelta;
	mCurrentMenuItem = 0;
	mNumMenuItems = 0;
	mStartingX = xPos;
	mStartingY = yPos;
	mName = name;
	mParent = parent;


	mItemHeight = 0.05f;
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
         // Create an overlay
     mMenuOverlay = overlayManager.create( "Menu"+name );
 
	 // Create a panel
	 mPanel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"Panel" ) );
	 mPanel->setPosition(xPos, yPos );
	 mPanel->setDimensions( 1 - 2*mStartingX, 0.9f );
	 mPanel->setMaterialName( "Menu/Background/Blue" );
	 // Add the panel to the overlay
	 mMenuOverlay->add2D( mPanel );

	 mPanelText =  static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"TextPanel" ) );
	 mPanelText->setPosition(0,0);
	 mPanel->addChild(mPanelText);

	 mMenuHighlight = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"HighlightPanel" ) );
	 mMenuHighlight->setDimensions( 1 - 2*mStartingX, mItemHeight * 1.5f);
	 mMenuHighlight->setMaterialName( "Menu/Highlight/Blue" );
	 mPanel->addChild(mMenuHighlight);


	 Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(
		 overlayManager.createOverlayElement("TextArea", name + "Header"));
	textArea->setPosition(mStartingX, mStartingY);
	textArea->setCaption(header);
	textArea->setCharHeight(mItemHeight * 1.5f);
	textArea->setFontName("Big");
	textArea->setColour(mUnHighlightColor);
	mPanel->addChild(textArea);

	 // Show the overlay
	mEnabled = beginEnabled;
	if (beginEnabled)
	{
		mMenuOverlay->show();
	}

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


Menu::MenuItem::~MenuItem()
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	overlayManager.destroyOverlayElement(mItemText);
}
void Menu::disable()
{
	mMenuOverlay->hide();
	mEnabled = false;
}


void Menu::AddMenuItem(MenuItem *item)
{
	mNameToIndexMap[item->name()] = mNumMenuItems;
	mNumMenuItems++;
	mMenuItems.push_back(item);


	if (mNumMenuItems == 1)
	{
		item->Select();
	}
	float height = (mNumMenuItems + 1.5f) * mItemSpacing + mItemHeight + mStartingY;
	mPanel->setHeight(height);
}

void Menu::AddSelectElement(Ogre::String text,  std::function<void(void)> callback, bool save /* = false */)
{
	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems + 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::SelectMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback, save));
}

void Menu::AddChooseInt(Ogre::String text,  std::function<void(int)> callback,  int minValue, int maxValue, int initialValue, int delta, bool save /* = false */)
{

	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems+ 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseIntMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback,  minValue, maxValue,initialValue, delta, save));
}

void Menu::AddChooseString(Ogre::String name, std::function<void(Ogre::String)> callback, Ogre::String initialValue, int maxlength, bool isPassword /* = false */, bool save /* = false */)
{
	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems+ 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseStringMenuItem(name, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback,initialValue,maxlength, isPassword, save));


}



void Menu::AddChooseFloat(Ogre::String text,  std::function<void(float)> callback,  float minValue, float maxValue, float initialValue, float delta, bool save /* = false */)
{

	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems+ 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseFloatMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback,  minValue, maxValue, initialValue, delta, save));
}



void Menu::AddChooseBool(Ogre::String text,  std::function<void(bool)> callback, bool initialValue /* = false */, bool save /* = false */)
{
	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems+ 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseBoolMenuItem(text, mName + "_" + std::to_string(mNumMenuItems),this,x,y,callback, initialValue, save));
}

void Menu::AddChooseEnum(Ogre::String name, std::vector<Ogre::String> enumNames, std::vector<std::function<void()>> callbacks, int initialVal /* = 0 */, bool save /* = false */)
{
	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems+ 1.5f) * mItemSpacing; 

	AddMenuItem(new Menu::ChooseEnumMenuItem(name, mName + "_" + std::to_string(mNumMenuItems),this,x,y, enumNames, callbacks, initialVal, save));

}




void  Menu::MenuItem::Select() 
{
	mItemText->setColour(mParent->mHighlightColor);
	mItemText->setPosition(mX, mY - mParent->mItemHeight * 0.5f / 2.0f);
	mItemText->setCharHeight(mParent->mItemHeight * 1.5f);
	mParent->mMenuHighlight->setPosition(0, mY - mParent->mItemHeight * 0.5f / 2.0f - 0.01f);
}

void  Menu::MenuItem::Deselect() 
{
	mItemText->setColour(mParent->mUnHighlightColor);
	mItemText->setHeight(mParent->mItemHeight);
	mItemText->setPosition(mX, mY);
	mItemText->setCharHeight(mParent->mItemHeight);
}


Menu::MenuItem::MenuItem(Ogre::String text, Ogre::String name,  Menu *parent, float x, float y, bool saved)
	: mParent(parent), mText(text)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	mItemText = static_cast<Ogre::TextAreaOverlayElement*>(
	overlayManager.createOverlayElement("TextArea", name));
	mItemText->setPosition(x, y);
	mX = x;
	mY  = y;
	mItemText->setCaption(text);
	mItemText->setCharHeight(parent->mItemHeight);
	mItemText->setFontName("Big");
	mItemText->setColour(Ogre::ColourValue(1,1,1));
	mParent->mPanelText->addChild(mItemText);
	mSaved = saved;
}


Menu::SelectMenuItem::SelectMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(void)> callback, bool saved)
	: Menu::MenuItem(text, name, parent, x , y, saved), mCallback(callback)
{

}

void  Menu::SelectMenuItem::Enter() 
{ 
	mCallback();
}



Menu::ChooseEnumMenuItem::ChooseEnumMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::vector<Ogre::String> choiceNames, std::vector<std::function<void()>> callbacks, int initialValue, bool saved) 
	: Menu::MenuItem(text, name, parent, x, y, saved),mCallbacks(callbacks), mChoiceNames(choiceNames), mCurrentValue(initialValue)
{
	mItemText->setCaption(mText + ":" + "  " +mChoiceNames[mCurrentValue]);
}



void  Menu::ChooseEnumMenuItem::setValueFromString(std::string valueString)
{
	std::string val = JSON_UTIL::stripQuotes(valueString);
	for (unsigned int i = 0; i < mChoiceNames.size(); i++)
	{
		if (mChoiceNames[i] == val)
		{
			mCurrentValue = i;
			mItemText->setCaption(mText + ":" + "  " +mChoiceNames[mCurrentValue]);
			mCallbacks[mCurrentValue]();
			return;
		}
	}
	// TODO:  Log Failure
}
std::string  Menu::ChooseEnumMenuItem::getValueAsString()
{
	return "\"" + mChoiceNames[mCurrentValue] + "\"";
}


void Menu::ChooseEnumMenuItem::Increase() 
{ 
	mCurrentValue++;
	if (mCurrentValue >= (int) mChoiceNames.size())
	{
		mCurrentValue = 0;
	}
	mItemText->setCaption(mText + ":" + "  " +mChoiceNames[mCurrentValue]);
	mCallbacks[mCurrentValue]();
}
void Menu::ChooseEnumMenuItem::Decrease() 
{ 
	mCurrentValue--;
	if (mCurrentValue < 0)
	{
		mCurrentValue = mChoiceNames.size() - 1;
	}
	mItemText->setCaption(mText + ":" + "  " +mChoiceNames[mCurrentValue]);
	mCallbacks[mCurrentValue]();
}




Menu::ChooseStringMenuItem::ChooseStringMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<void(Ogre::String)> callback,Ogre::String initial, int maxLength, bool password, bool saved)
	: Menu::MenuItem(text, name, parent, x, y, saved), mCallback(callback), mMaxLength(maxLength), mPassword(password), mValue(initial), mVisual()

{
	if (!mPassword)
	{
		mVisual = mValue;
	}
	else
	{
		for (unsigned int i = 0; i < mValue.length(); i++)
		{
			mVisual.push_back('*');
		}
	}
	mItemText->setCaption(mText + ":" + " " +mVisual);
}

void Menu::ChooseStringMenuItem::setValueFromString(std::string valueString)
{
	mValue = JSON_UTIL::stripQuotes(valueString);
	if (!mPassword)
	{
		mVisual = mValue;
	}
	else
	{
		for (unsigned int i = 0; i < mValue.length(); i++)
		{
			mVisual.push_back('*');
		}
	}
	mItemText->setCaption(mText + ":" + " " +mVisual);
	mCallback(mValue);

}
std::string Menu::ChooseStringMenuItem::getValueAsString()
{
	return "\"" + mValue + "\"";
}



void Menu::ChooseStringMenuItem::HandleKeypress(const OIS::KeyEvent &e)
{

	if (e.key == OIS::KC_BACK || e.key == OIS::KC_DELETE)
	{
		if (mValue.length() > 0)
	
		{
			mChanged = true;
			mVisual.pop_back();
			mVisual.pop_back();
			mVisual.push_back('|');
			mValue.pop_back();
			mItemText->setCaption(mText + ":" + " " +mVisual);
		}
	}
	else if ((int) mValue.length() < mMaxLength && isprint(e.text))
	{
		mValue.push_back((char) e.text);
		mChanged = true;
		if (mPassword)
		{
			mVisual.pop_back();
			mVisual.push_back('*');
			mVisual.push_back('|');
		}
		else
		{
			mVisual.pop_back();
			mVisual.push_back((char) e.text);
			mVisual.push_back('|');
		}
		mItemText->setCaption(mText + ":" + " " +mVisual);
	}
}

void Menu::ChooseStringMenuItem::Select()
{
	MenuItem::Select();
	mChanged = false;
	mVisual.push_back('|');
	mItemText->setCaption(mText + ":" + " " +mVisual);
}

void Menu::ChooseStringMenuItem::Enter()
{
	mParent->moveSelectDown();
}
void Menu::ChooseStringMenuItem:: Deselect()
{
		mVisual.pop_back();
		mItemText->setCaption(mText + ":" + " " +mVisual);

		if (mChanged)
		{
			mCallback(mValue);
		}
		MenuItem::Deselect();
}

void Menu::ChooseStringMenuItem::Increase()
{

}
void Menu::ChooseStringMenuItem::Decrease()
{
	if(mValue.length() > 0)
	{
		mVisual.pop_back();
		mVisual.pop_back();
		mVisual.push_back('|');
		mValue.pop_back();
		mItemText->setCaption(mText + ":" + " " +mVisual);
	}
}





Menu::ChooseIntMenuItem::ChooseIntMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(int)> callback,  int minValue, int maxValue, int initialValue, int delta, bool saved)
	: Menu::MenuItem(text, name, parent, x, y, saved), mCallback(callback), mIntValue(initialValue), mMinValue(minValue), mMaxValue(maxValue), mDelta(delta)
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


void Menu::ChooseIntMenuItem::setValueFromString(std::string valueString)
{
	mIntValue = atoi(JSON_UTIL::stripQuotes(valueString).c_str());
	mItemText->setCaption(mText + ":" + "  " + std::to_string(mIntValue));
	mCallback(mIntValue);

}
std::string Menu::ChooseIntMenuItem::getValueAsString()
{
	return "\"" + std::to_string(mIntValue) + "\"";
}



Menu::ChooseFloatMenuItem::ChooseFloatMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(float)> callback, 
                                               float minValue, float maxValue, float initialValue, float delta, bool saved)
	: Menu::MenuItem(text, name, parent, x, y, saved), mCallback(callback), mFloatValue(initialValue), mMinValue(minValue), mMaxValue(maxValue),mDelta(delta)
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

void  Menu::ChooseFloatMenuItem::setValueFromString(std::string valueString)
{
	mFloatValue = std::stof(JSON_UTIL::stripQuotes(valueString).c_str());
	updateOverlayFromValue();
	mCallback(mFloatValue);
}
std::string  Menu::ChooseFloatMenuItem::getValueAsString()
{
	return "\"" + std::to_string(mFloatValue) + "\"";
}


Menu::ChooseBoolMenuItem::ChooseBoolMenuItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y,  std::function<void(bool)> callback, bool initialValue, bool saved)
	: Menu::MenuItem(text, name, parent,  x, y, saved), mCallback(callback), mBoolValue(initialValue)
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


void Menu::ChooseBoolMenuItem::setValueFromString(std::string valueString)
{
	std::string val = JSON_UTIL::stripQuotes(valueString);
	if (val == "true")
	{
		mBoolValue = true;
	}
	else if (val == "false")
	{
		mBoolValue = false;
	}
	else
	{
		// Error case?
	}
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
std::string Menu::ChooseBoolMenuItem::getValueAsString()
{
	if (mBoolValue)
	{
		return "\"true\"";
	}
	else
	{
		return "\"false\"";
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