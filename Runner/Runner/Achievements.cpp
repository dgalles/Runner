#include "Achievements.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
 #include <OgreTextAreaOverlayElement.h>
#include "OgreFontManager.h"
#include "OgreOverlayElement.h"

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


}

void  Achievements::setupCompletedUI(Ogre::String message, float timeToDisplay)
{
	mShowCompletedTimeRemaining[mCompletedActive] = timeToDisplay;
	mCompletedText[mCompletedActive]->setCaption(message);
	mCompletedAchievement[mCompletedActive]->show();
	mCompletedActive++;
}

void Achievements::AchievementCleared(int index, float timeToDisplay /* = 5 */)
{
	if (mAchievmentsActive[index])
	{
		mAchievmentsCleared[index] = true;
		mAchievmentsActive[index] = false;
		setupCompletedUI(mAchievmentsShort[index] + ": " + mAchievmentsLong[index], timeToDisplay);
	}
}

void 
	Achievements::AchievementCleared(Ogre::String achievement, float timeToDisplay /*  = 5 */)
{
	std::map<Ogre::String, int>::iterator iter = mShortNameToIndex.find(achievement);
	if (iter != mShortNameToIndex.end())
	{
		AchievementCleared(iter->second, timeToDisplay);
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
	AddAchievement("Pennies From Hevean", "Collect 100 Coints in one run");
	AddAchievement("Making Money", "Collect 200 Coints in one run");
	AddAchievement("Looper", "Complete a Loop-de-loop");
	AddAchievement("Greedy I", "Get all coints in a 20 meter segment");
	AddAchievement("Buzzed", "Hit a Sawblade");
	AddAchievement("Penniless I", "Run 10 Meters without picking up a coin");
	AddAchievement("Marathon I", "Run 5000 Meters Lifetime");
	AddAchievement("Snap", "Complete a Snap Turn");
	AddAchievement("Greedy II", "Get all coints in a 50 meter segment");
	AddAchievement("Penniless II", "Run 50 Meters without picking up a coin");
	AddAchievement("Middle Distance", "Run 500 Meters in one run");
	AddAchievement("Greedy III", "Get all coints in a 100 meter segment");
	AddAchievement("Long Haul", "Run 1000 Meters in one run");
	AddAchievement("Getting Bank", "Collect 500 Coints in one run");
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
	for (int i = 0; i < mAchievmentsLong.size(); i++)
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
	for (int i = 0; i < mAchievmentsLong.size(); i++)
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
