#include "Achievements.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
 #include <OgreTextAreaOverlayElement.h>
#include "OgreFontManager.h"
#include "OgreOverlayElement.h"
#include "InputHandler.h"
#include "OgreResourceGroupManager.h"
#include "JsonUtils.h"
#include "Sound.h"
#include <iostream>
#include <fstream>
#include <string>

std::string findFilePath(const std::string& filename)
{
	std::string foundPath = filename;
	Ogre::ResourceGroupManager* groupManager = Ogre::ResourceGroupManager::getSingletonPtr() ;
	Ogre::String group = groupManager->findGroupContainingResource(filename) ;
	Ogre::FileInfoListPtr fileInfos = groupManager->findResourceFileInfo(group,foundPath);
	Ogre::FileInfoList::iterator it = fileInfos->begin();
	if(it != fileInfos->end())
	{
		foundPath = it->archive->getName() + "/" + foundPath;
	}
	else
	{
		foundPath = "";
	}
	return foundPath;
}






Achievements::Achievements(Ogre::String achievmentFilename) : mAchievmentsLong(), mAchievmentsShort(), mAchievmentsCleared(),mAchievmentsActive(), 
	                               mShortNameToIndex(), mShowActiveTimeRemaining(0), mAchievemntTextAreas(), mTitles()
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
	for (int i = 0; i < ACTIVE_ACHIEVEMENTS; i ++)
	{
		mShowCompletedTimeRemaining[i] = 0;
	}
	mCompletedActive = 0;
	mAllOverlay = 0;
	mViewAllAchievements = false;
	if (achievmentFilename.length() > 0)
	{
		ReadAchievements(achievmentFilename);
	}
}


void Achievements::clearUI()
{
	for (int i = 0; i < ACTIVE_ACHIEVEMENTS; i++)
	{
		mCompletedAchievement[i]->hide();
	}
	mActiveAchievements->hide();
	ShowAllAchievements(false);
}

Achievements::~Achievements(void)
{
}

void Achievements::ShowAllAchievements(bool show)
{
	mViewAllAchievements = show;
	if (show == false)
	{
		if (mAllOverlay != 0)
		{
			mAllOverlay->hide();
		}
			return;
	}
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	float titleHeight = 0.08f;
	float nonTitleHeight = 0.05f;
	float titleSpacing = 0.1f;
	float nonTitleSpacing = 0.065f;

	if (!mAllOverlay)
	{
		mAllOverlay = overlayManager.create("AchievementOverlay");
		mAllOverlay->setZOrder(600);
		Ogre::ColourValue color(0, 0.1f, 0, 1);

		// Create a panel

		float panelHeight = 0.1f + titleSpacing * 3 + nonTitleSpacing * mAchievmentsLong.size();
		mMaxScroll = panelHeight + 0.5f;

		Ogre::OverlayContainer* border =  static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "AchievementPanelBorder" ) );
		border->setPosition(0.02f, 0.02f );
		border->setDimensions( 0.96f, panelHeight + 0.06f);
		border->setMaterialName("Achievement/Background/AllBorder" );

		mAllOverlay->add2D(border);

		mAchievementPanel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "AchievementPanel" ) );
		mAchievementPanel->setPosition(0.05f, 0.05f );
		mAchievementPanel->setDimensions( 0.9f, panelHeight);
		mAchievementPanel->setMaterialName("Achievement/Background/All" );
		// Add the panel to the overlay
		mAllOverlay->add2D( mAchievementPanel );



		for (int i = 0; i < 3; i++)
		{
			Ogre::TextAreaOverlayElement *nextElem = static_cast<Ogre::TextAreaOverlayElement*>(
				overlayManager.createOverlayElement("TextArea","AchieveTitle" + std::to_string(i)));
			mTitles.push_back(nextElem);
			nextElem->setCharHeight(0.08f);
			nextElem->setFontName("Big");
			nextElem->setColour(color);
			mAchievementPanel->addChild(nextElem);

		}
		mTitles[0]->setCaption("Active Goals");
		mTitles[1]->setCaption("Completed Goals");
		mTitles[2]->setCaption("Future Goals");



		for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
		{
			Ogre::TextAreaOverlayElement *nextElem = static_cast<Ogre::TextAreaOverlayElement*>(
				overlayManager.createOverlayElement("TextArea","AchieveText" + std::to_string(i)));
			nextElem->setCharHeight(nonTitleHeight);
			nextElem->setFontName("Big");
			nextElem->setColour(color);

			nextElem->setCaption(mAchievmentsShort[i] + ": " + mAchievmentsLong[i]);
			mAchievementPanel->addChild(nextElem);
			mAchievemntTextAreas.push_back(nextElem);
		}

	}
	ResetActive();
	float currentY = 0.05f;
	float currentX = 0.05f;

	mTitles[0]->setPosition(currentX, currentY);
	currentY += titleSpacing;

	for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (mAchievmentsActive[i])
		{
			mAchievemntTextAreas[i]->setPosition(currentX, currentY);
			currentY += nonTitleSpacing;
		}
	}

	mTitles[1]->setPosition(currentX, currentY);
	currentY += titleSpacing;

	for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (mAchievmentsCleared[i])
		{
			mAchievemntTextAreas[i]->setPosition(currentX, currentY);
			currentY += nonTitleSpacing;
		}
	}

	mTitles[2]->setPosition(currentX, currentY);
	currentY += titleSpacing;

	for (unsigned int i = 0; i < mAchievmentsLong.size(); i++)
	{
		if (!mAchievmentsCleared[i] && !mAchievmentsActive[i])
		{
			mAchievemntTextAreas[i]->setPosition(currentX, currentY);
			currentY += nonTitleSpacing;
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
			scrollY += time * 0.5f;
			scrollY = std::min(scrollY, mMaxScroll);
			mAllOverlay->setScroll(scrollX,scrollY);

		} 
		else if (ih->IsKeyDown(OIS::KC_UP))
		{
			float scrollY = mAllOverlay->getScrollY();
			float scrollX =  mAllOverlay->getScrollX();
			scrollY -= time * 0.5f;
			scrollY = std::max(scrollY, 0.0f);
			mAllOverlay->setScroll(scrollX, scrollY);
		}


	}



}

void  Achievements::setupCompletedUI(Ogre::String message, float timeToDisplay)
{
	if (mCompletedActive >= ACTIVE_ACHIEVEMENTS)
	{
		// This is an error ..
		return;
	}
	SoundBank::getInstance()->play("clearAchievement");
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


std::string Achievements::getCompletedAchievements()
{
	bool first = true;

	std::string achievementString = "[";
	for (unsigned int i = 0; i < mAchievmentsShort.size(); i++)
	{
		if (mAchievmentsCleared[i])
		{
			if (!first)
			{
				achievementString += ",";
			}
			first = false;
			achievementString += "\"" +  mAchievmentsShort[i] + "\"";
		}
	}
	achievementString += "]";
	return achievementString;
}
void Achievements::setCompletedAchievements(std::string json)
{
	std::size_t braceIndex = json.find_first_of('[');

	std::string remainder = json.substr(braceIndex+1);

	std::size_t nextIndex = remainder.find_first_not_of("\t \n");
	while (nextIndex != std::string::npos && remainder[nextIndex] != ']')
	{
		std::string nextAchievement = JSON_UTIL::stripQuotes(JSON_UTIL::firstItem(remainder));

		remainder = JSON_UTIL::removeFirstitem(remainder);
		
		AchievementCleared(nextAchievement,true,0);

		nextIndex = remainder.find_first_not_of("\t \n");
		if (nextIndex != std::string::npos && remainder[nextIndex] == ',')
			nextIndex++;
		remainder = remainder.substr(nextIndex);
		nextIndex = remainder.find_first_not_of("\t \n");
	}
}



void
	Achievements::ReadAchievements(Ogre::String filename)
{

	{
		std::string nextAchievement;
		std::ifstream infile;
		infile.open (findFilePath(filename));
		while(!infile.eof()) 
		{
			getline(infile,nextAchievement);
			unsigned int colon = nextAchievement.find_first_of(":");
			if (colon != std::string::npos)
			{
				AddAchievement(nextAchievement.substr(0,colon), nextAchievement.substr(colon+1));
			}
		}
		infile.close();
	}

	ResetActive();

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

void Achievements::ResetAll()
{
	for (unsigned int i = 0; i < mAchievmentsCleared.size(); i++)
	{
		mAchievmentsCleared[i] = false;
	}
	ResetActive();
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
