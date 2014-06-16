#include "Achievements.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
 #include <OgreTextAreaOverlayElement.h>
#include "OgreFontManager.h"
#include "OgreOverlayElement.h"
#include "InputHandler.h"

Achievements::Achievements(void) : mAchievmentsLong(), mAchievmentsShort(), mAchievmentsCleared(),mAchievmentsActive(), mShortNameToIndex(), mShowActiveTimeRemaining(0)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mActiveAchievements = om.getByName("Achievements/Active");
	for (int i = 1; i <= ACTIVE_ACHIEVEMENTS; i++)
	{
		mCompletedAchievement[i-1] =  om.getByName("Achievements/Completed" + std::to_string(i));
		mActiveText[i-1] =om.getOverlayElement("Achievements/Active/Text" + std::to_string(i));
		mCompletedText[i-1] = om.getOverlayElement("Achievements/Completed" +  std::to_string(i)  + "/Text");
		mCompletedAchievement[i-1]->hide();
	}
	mActiveAchievements->hide();
	ReadAchievements("Dummy");
	ResetActive();
	for (int i = 0; i < ACTIVE_ACHIEVEMENTS; i ++)
	{
		mShowCompletedTimeRemaining[i] = 0;
	}
	mCompletedActive = 0;
	mAllOverlay = 0;
	mViewAllAchievements = false;
}


void Achievements::clearUI()
{
	for (int i = 0; i < ACTIVE_ACHIEVEMENTS; i++)
	{
		mCompletedAchievement[i]->hide();
	}
	mActiveAchievements->hide();
}

Achievements::~Achievements(void)
{
}

void Achievements::ShowAllAchievements(bool show)
{
	mViewAllAchievements = show;
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	if (show == false)
	{
		if (mAllOverlay != 0)
		{
			mAllOverlay->hide();
			return;
		}

	}
	if (mAllOverlay)
	{
		Ogre::Overlay::Overlay2DElementsIterator it =  mAllOverlay->get2DElementsIterator();

		while (it.hasMoreElements())
		{
			Ogre::OverlayContainer *e = it.getNext();
			Ogre::OverlayContainer::ChildIterator it2 = e->getChildIterator();
			while (it.hasMoreElements())
			{
				Ogre::OverlayElement * elem = it.getNext();
				overlayManager.destroyOverlayElement(elem);
			}
			overlayManager.destroyOverlayElement(e);
		}
		overlayManager.destroy("AchievementOverlay");
	}
	mAllOverlay = overlayManager.create("AchievementOverlay");

 
	 // Create a panel
	 Ogre::OverlayContainer *achievementPanel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "AchievementPanel" ) );
	 achievementPanel->setPosition(0, 0 );
	 achievementPanel->setDimensions( 0.8f, 0.8f );
	 //mPanel->setMaterialName( "Kinect/Blue" );
	 // Add the panel to the overlay
	 mAllOverlay->add2D( achievementPanel );

	 float currentY = 0.1f;
	 float currentX = 0.1f;

	 float 	itemHeight = 0.05f;
	 float itemSpacing = 0.1f;
	 Ogre::ColourValue color(0,1,0,1);

	 Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(
	 overlayManager.createOverlayElement("TextArea","CurrentAchievements"));
	textArea->setPosition(currentX, currentY);
	textArea->setCaption("Active Goals");
	textArea->setCharHeight(itemHeight);
	textArea->setFontName("Big");
	textArea->setColour(color);
	achievementPanel->addChild(textArea);

	currentY += itemSpacing;

	int acvCount = 1;

	for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (mAchievmentsActive[i])
		{
			textArea = static_cast<Ogre::TextAreaOverlayElement*>(
				overlayManager.createOverlayElement("TextArea","AllAchiev" + std::to_string(acvCount++)));
			textArea->setPosition(currentX, currentY);
			textArea->setCaption(mAchievmentsShort[i] + ": " + mAchievmentsLong[i]);
			textArea->setCharHeight(itemHeight);
			textArea->setFontName("Big");
			textArea->setColour(color);
			achievementPanel->addChild(textArea);
			currentY += itemSpacing;
		}

	}

	textArea = static_cast<Ogre::TextAreaOverlayElement*>(
		overlayManager.createOverlayElement("TextArea","AllAchiev" + std::to_string(acvCount++)));
	textArea->setPosition(currentX, currentY);
	textArea->setCaption("Completed Goals");
	textArea->setCharHeight(itemHeight);
	textArea->setFontName("Big");
	textArea->setColour(color);
	achievementPanel->addChild(textArea);
	currentY += itemSpacing;

		for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (mAchievmentsCleared[i])
		{
			textArea = static_cast<Ogre::TextAreaOverlayElement*>(
				overlayManager.createOverlayElement("TextArea","AllAchiev" + std::to_string(acvCount++)));
			textArea->setPosition(currentX, currentY);
			textArea->setCaption(mAchievmentsShort[i] + ": " + mAchievmentsLong[i]);
			textArea->setCharHeight(itemHeight);
			textArea->setFontName("Big");
			textArea->setColour(color);
			achievementPanel->addChild(textArea);
			currentY += itemSpacing;
		}

	}

		
	textArea = static_cast<Ogre::TextAreaOverlayElement*>(
		overlayManager.createOverlayElement("TextArea","AllAchiev" + std::to_string(acvCount++)));
	textArea->setPosition(currentX, currentY);
	textArea->setCaption("Future Goals");
	textArea->setCharHeight(itemHeight);
	textArea->setFontName("Big");
	textArea->setColour(color);
	achievementPanel->addChild(textArea);
	currentY += itemSpacing;


			for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (!mAchievmentsCleared[i] && !mAchievmentsActive[i])
		{
			textArea = static_cast<Ogre::TextAreaOverlayElement*>(
				overlayManager.createOverlayElement("TextArea","AllAchiev" + std::to_string(acvCount++)));
			textArea->setPosition(currentX, currentY);
			textArea->setCaption(mAchievmentsShort[i] + ": " + mAchievmentsLong[i]);
			textArea->setCharHeight(itemHeight);
			textArea->setFontName("Big");
			textArea->setColour(color);
			achievementPanel->addChild(textArea);
			currentY += itemSpacing;
		}

	}


	mAllOverlay->show();


}


void Achievements::Think(float time)
{
	for (int i = ACTIVE_ACHIEVEMENTS-1; i >= 0; i--)
	{
		if (mShowCompletedTimeRemaining[i] > 0)
		{
			if (mShowCompletedTimeRemaining[i] <= time)
			{
				mCompletedActive--;
				if (mCompletedActive < 0)
				{
					mCompletedActive = 0;
				}
				mShowCompletedTimeRemaining[i] = 0;
				mCompletedAchievement[i]->hide();
				for (int j = i + 1; j < ACTIVE_ACHIEVEMENTS; j++)
				{
					if (mShowCompletedTimeRemaining[j] > 0)
					{
						mCompletedText[j-1]->setCaption(mCompletedText[j]->getCaption());
						mShowCompletedTimeRemaining[j-1] = mShowCompletedTimeRemaining[j];
						mCompletedAchievement[j-1]->show();
						mCompletedAchievement[j]->hide();
						mShowCompletedTimeRemaining[j] = 0;
					}
				}
			}
			else
			{
				mShowCompletedTimeRemaining[i] -= time;
			}
		}
	}
	if (mShowActiveTimeRemaining > 0)
	{
		if (mShowActiveTimeRemaining <= time)
		{
			mActiveAchievements->hide();
			mShowActiveTimeRemaining = 0;
		}
		else
		{
			mShowActiveTimeRemaining -= time;
		}
	}
	if (mViewAllAchievements)
	{
		InputHandler *ih = InputHandler::getInstance();


		if (ih->IsKeyDown(OIS::KC_DOWN))
		{
			float scrollY = mAllOverlay->getScrollY();
			float scrollX =  mAllOverlay->getScrollX();
			mAllOverlay->setScroll(scrollX, scrollY + time * 0.3);

		} 
		else if (ih->IsKeyDown(OIS::KC_UP))
		{


		}
		else if (ih->IsKeyDown(OIS::KC_ESCAPE))
		{


		}




	}



}

void  Achievements::setupCompletedUI(Ogre::String message, float timeToDisplay)
{
	mShowCompletedTimeRemaining[mCompletedActive] = timeToDisplay;
	mCompletedText[mCompletedActive]->setCaption(message);
	mCompletedAchievement[mCompletedActive]->show();
	mCompletedActive++;
}

void Achievements::AchievementCleared(int index, bool force /* = false*/, float timeToDisplay /* = 5 */)
{
	if (mAchievmentsActive[index] || force)
	{
		mAchievmentsCleared[index] = true;
		mAchievmentsActive[index] = false;
		if (timeToDisplay > 0)
		{
			setupCompletedUI(mAchievmentsShort[index] + ": " + mAchievmentsLong[index], timeToDisplay);
		}
	}
}

void 
	Achievements::AchievementCleared(Ogre::String achievement, bool force, /* = false */ float timeToDisplay /*  = 5 */)
{
	std::map<Ogre::String, int>::iterator iter = mShortNameToIndex.find(achievement);
	if (iter != mShortNameToIndex.end())
	{
		AchievementCleared(iter->second, force, timeToDisplay);
	}
	else
	{
		setupCompletedUI("Bad Achievement Name: " + achievement, timeToDisplay);
	}
}


// TODO:  Make this a file!!
void
	Achievements::ReadAchievements(Ogre::String filename)
{

	AddAchievement("Getting Started", "Run 100 Meters in one run");
	AddAchievement("Pennies From Heaven", "Collect 100 Coints in one run");
	AddAchievement("Making Money", "Collect 200 Coints in one run");
	AddAchievement("Looper", "Complete a Loop-de-loop");
	AddAchievement("Greedy I", "Get all coins in a 20 meter segment");
	AddAchievement("Buzzed", "Hit a Sawblade");
	AddAchievement("Booster", "Use a Boost");
	AddAchievement("Penniless I", "Run 10 Meters without picking up a coin");
	AddAchievement("Marathon I", "Run 5000 Meters Lifetime");
	AddAchievement("Snap", "Complete a Snap Turn");
	AddAchievement("Greedy II", "Get all coins in a 50 meter segment");
	AddAchievement("Need for Speed", "Use 2 Boosts on one run");
	AddAchievement("Getting Bank", "Collect 500 Coints in one run");
	AddAchievement("Penniless II", "Run 50 Meters without picking up a coin");
	AddAchievement("Blazin'", "Use 3 Boosts on one run");
	AddAchievement("Middle Distance", "Run 500 Meters in one run");
	AddAchievement("Greedy III", "Get all coins in a 100 meter segment");
	AddAchievement("Long Haul", "Run 1000 Meters in one run");
	AddAchievement("Building Bling", "Collect 5000 Coins Lifetime");
	AddAchievement("Penniless III", "Run 100 Meters without picking up a coin");
	AddAchievement("Marathon II", "Run 10000 Meters Lifetime");
	AddAchievement("Marathon III", "Run 50000 Meters Lifetime");
}

void Achievements::AddAchievement(Ogre::String shortName, Ogre::String longName)
{
	int index = mAchievmentsLong.size();
	mAchievmentsLong.push_back(longName);
	mAchievmentsShort.push_back(shortName);
	mAchievmentsCleared.push_back(false);
	mAchievmentsActive.push_back(false);
	mShortNameToIndex[shortName] = index;
}

void Achievements::ResetActive()
{
	int numActive = 0;
	for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (!mAchievmentsCleared[i] && numActive < ACTIVE_ACHIEVEMENTS)
		{
			mAchievmentsActive[i] = true;
			numActive++;
		}
		else
		{
			mAchievmentsActive[i] = false;
		}
	}
}

void Achievements::DisplayActiveAchievements(float time /* = -1*/)
{
	int activeIndex = 0;
	for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (mAchievmentsActive[i])
		{
			mActiveText[activeIndex]->setCaption(mAchievmentsShort[i] + ": " + mAchievmentsLong[i]);
			activeIndex++;
			if (activeIndex >= ACTIVE_ACHIEVEMENTS)
				break;
		}
	}
	
	mActiveAchievements->show();
	mShowActiveTimeRemaining = time;

}
