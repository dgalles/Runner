#include "Store.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayElement.h"
#include "OgreOverlayContainer.h"
#include "OgreTextAreaOverlayElement.h"



Store::Store(Ogre::String header, Ogre::String name, float xPos, float yPos, float ydelta, Menu *parent)

{
	mHighlightColor = Ogre::ColourValue(1.0f,0.0f, 0.0f);
	mUnHighlightColor = Ogre::ColourValue(1.0f,0.0f, 0.0f);


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
	mMenuOverlay = overlayManager.create( "Store"+name );

	// Create a panel
	mPanel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"Panel" ) );
	mPanel->setPosition(xPos, yPos );
	mPanel->setDimensions( 0.9f, 0.9f );
	mPanel->setMaterialName( "Menu/Background/Blue" );
	// Add the panel to the overlay
	mMenuOverlay->add2D( mPanel );

	mPanelText =  static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"TextPanel" ) );
	mPanelText->setPosition(0,0);
	mPanel->addChild(mPanelText);

	mMenuHighlight = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"HighlightPanel" ) );
	mMenuHighlight->setDimensions(0.9f, mItemHeight * 1.5f);
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
	mMenuOverlay->hide();

}





Store::Store(void)
{
}


Store::~Store(void)
{
}



void Store::AddStoreElem(Ogre::String name,  std::function<int(void)> getValue, std::function<void(int)> setValue,
						 int minValue, int maxValue, std::vector<int> prices, bool canSell /* = false */)
{

	float x = mStartingX;
	float y = mStartingY + (mNumMenuItems + 1.5f) * mItemSpacing; 

	AddMenuItem(new Store::StoreItem(name, mName + "_" + std::to_string(mNumMenuItems),this,x,y, getValue, setValue, minValue, maxValue, prices, canSell));

}


void Store::StoreItem::resetStoreItem()
{
	int currentValue = mGetValue();
	if (currentValue < mMaxValue)
	{
		mItemText->setCaption(mText + " (Cost = " + std::to_string(mPrices[currentValue]) + ")");
	}
	else
	{
		mItemText->setCaption(mText + " (at Maximum)");
	}
	for (int i = 0; i < mMaxValue; i++)
	{
		if (i <= currentValue)
		{
			mBlocks[i]->show();
		}
		else
		{
			mBlocks[i]->hide();
		}

	}
}

Store::StoreItem::StoreItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<int(void)> getValue, std::function<void(int)> setvalue,
							int minValue, int maxVal, std::vector<int> prices, bool canSell) : 
Menu::MenuItem(text, name, parent, x , y, false), mMinValue(minValue), mMaxValue(maxVal), mPrices(prices), mCanSell(canSell), mGetValue(getValue), mSetValue(setvalue)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	mBlockPanel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"BlockPanel" ) );
	getPanelText()->addChild(mBlockPanel);

	float width = 0.6f / (float) (mMaxValue);

	for (int i = 0; i <= mMaxValue; i++)
	{

		Ogre::OverlayContainer *nextPanel;
		nextPanel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", name +"BlockPanel_" + std::to_string(i) ) );
		nextPanel->setPosition(x + i * (width +  0.01), y + mParent->getItemSpacing());
		nextPanel->setWidth(width);
		nextPanel->setMaterialName("simpleRed");
		nextPanel->setHeight(mParent->getItemSpacing() * 0.4f);
		mBlockPanel->addChild(nextPanel);
		mBlocks.push_back(nextPanel);

	}



	resetStoreItem();

}

void Store::StoreItem::Enter()
{

}
void Store::StoreItem::Increase()
{



}

