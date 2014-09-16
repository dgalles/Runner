#include "HUD.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include <OgreTextAreaOverlayElement.h>
#include "OgreFontManager.h"
#include "OgreOverlayElement.h"
#include "OgreIteratorWrappers.h"
#include "Sound.h"

HUD::HUD() : mArmorIndicator()
{

	Ogre::FontManager::getSingleton().getByName("Big")->load();

	mShowArrows = true;
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mArrowOverlay[HUD::left] = om.getByName("HUD/LeftArrow");
	mArrowOverlay[HUD::right] = om.getByName("HUD/RightArrow");
	mArrowOverlay[HUD::center] =om.getByName("HUD/CenterArrow");
	mArrowOverlay[HUD::down] =om.getByName("HUD/DownArrow");
	mArrowOverlay[HUD::up] =om.getByName("HUD/UpArrow");


	mRacingOverlay = om.getByName("HUD/Racing");
	mSpeedUpOverlay = om.getByName("HUD/SpeedUp");
	mSlowDownOverlay = om.getByName("HUD/SlowDown");
	for (int i = HUD::none + 1; i < HUD::last; i++)
	{
		mArrowTimeRemaining[i] = 0;
		mArrowStatus[i] = false;
	}

	mScoreOverlay = Ogre::OverlayManager::getSingleton().getByName("HUD/Score");
	mShowHUDElems = false;
	//mScoreOverlay->show();

	Ogre::String s = mScoreOverlay->getName();

	mCoinsText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Coins");
	mDistanceText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Distance");
	mSpeedText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/Panel/Speed");

	mGhostCoinsText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel/Coins");
	mGhostDistanceText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel/Distance");
	mGhostSpeedText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel/Speed");

	mTimeText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Racing/Panel/Time");
	mRacingMessage =  Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Racing/Panel/Message");

	mRacingDone = Ogre::OverlayManager::getSingleton().getByName("HUD/Racing/Done");

	mRacingDone->hide();

	int armorIndex =  1;
	while (Ogre::OverlayManager::getSingleton().hasOverlayElement("HUD/Armor/" + std::to_string(armorIndex)))
	{
		Ogre::OverlayElement *armor  =Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Armor/" + std::to_string(armorIndex));
		mArmorIndicator.push_back(armor);
		armorIndex++;
	}
	mRacingOverlay->hide();
}


void setRacingFinalMessage(Ogre::String message, bool isGhost = false)
{


}


void HUD::setRacingMesssage(Ogre::String message)
{
	mRacingMessage->setCaption(message);
}

void HUD::showRacingOverlay(bool show)
{
	if (show)
	{
		mRacingOverlay->show();
	}
	else
	{
		mRacingOverlay->hide();
	}

}

void HUD::showGhost(bool show)
{
	mShowGhostElements = show;
	if (mShowGhostElements && mShowHUDElems)
	{
		Ogre::OverlayElement *ghost = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel");
		ghost->show();

	}
	if (!mShowGhostElements)
	{
		Ogre::OverlayElement *ghost = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel");
		ghost->hide();
	}
}

void HUD::showHUDElements(bool show)
{
	mShowHUDElems = show;
	if (show)
	{
		mScoreOverlay->show();
		if(mShowGhostElements)
		{
			Ogre::OverlayElement *ghost = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Score/GhostPanel");
			ghost->show();
		}
	}
	else
	{
		mScoreOverlay->hide();
	}
}

void 
	HUD::stopAllArrows()
{
	for (int i = ((int) Kind::none) + 1; i < Kind::last; i++)
	{
		stopArrow((Kind) i);
	}
}


void HUD::setCoins(int newScore, bool ghost /* = false */, bool consec /* = false */)
{
	std::string score;
	if (consec)
	{
	 score = "Consecutive Coins = ";
	}
	else 
	{
	  score = "Coins = ";
	}
	score.append(std::to_string((long long)newScore));

	if (ghost)
	{
		mGhostCoinsText->setCaption(("Ghost: " + score));
	}
	else
	{
		mCoinsText->setCaption(score);
	}
}



void  HUD::setShowIncreaseSpeed(bool show)
{
	if (show)
	{
		mSpeedUpOverlay->show();
	}
	else
	{
		mSpeedUpOverlay->hide();
	}
}
void  HUD::setShowDecreaseSpeed(bool show)
{
	if (show)
	{
		mSlowDownOverlay->show();
	}
	else
	{
		mSlowDownOverlay->hide();
	}

}


void HUD::setSpeed(int speed, bool ghost)
{
	std::string score = "Speed = ";
	score.append(std::to_string((long long)speed));
	score.append(" m / s");
	if (ghost)
	{
		mGhostSpeedText->setCaption(("Ghost: " + score));
	}
	else
	{
		mSpeedText->setCaption(score);
	}
}


void HUD::setArmorLevel(int level)
{
	for (unsigned int i = 0; i < mArmorIndicator.size(); i++)
	{
		if (level > (int) i)
		{
			mArmorIndicator[i]->show();
		}
		else
		{
			mArmorIndicator[i]->hide();
		}

	}

}



void  HUD::setTime(float newTime)
{
	char buf[100];

	sprintf(buf, "%.2f", newTime);

		std::string time = "Time = ";
		time.append(buf);
		mTimeText->setCaption(time);



}


void HUD::setDistance(int newScore, bool ghost)
{
	std::string score = "Distance = ";
	score.append(std::to_string((long long)newScore));
	score.append(" meters");
	if (ghost)
	{
		mGhostDistanceText->setCaption("Ghost: " + score);
	}
	else
	{
		mDistanceText->setCaption(score);
	}
}
void HUD::update(float time)
{
	for (int i = HUD::none + 1; i < (int) HUD::last; i++)
	{
		if (mArrowTimeRemaining[i] <= time)
		{
			mArrowTimeRemaining[i] = 0;
			mArrowStatus[i] = false;
			mArrowOverlay[i]->hide();
		}
		else
		{
			mArrowTimeRemaining[i] -= time;
			if (mArrowFlashTime[i] <= time)
			{
				mArrowFlashTime[i] += mArrowFlashDelay[i] - time;
				mArrowStatus[i] = !mArrowStatus[i];
				if (mArrowStatus[i] && mShowArrows && mShowHUDElems)
				{
					mArrowOverlay[i]->show();
				}
				else
				{
					mArrowOverlay[i]->hide();
				}
			}
			else
			{
				mArrowFlashTime[i] -= time;
			}
		}
	}


}

void HUD::setFinalRaceTime(Ogre::String message, bool isGhost /* = false */)
{
	if (isGhost)
	{
		Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Racing/Done/Panel/Ghost/Time")->setCaption(message);
	}
	else
	{
		Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Racing/Done/Panel/Player/Time")->setCaption(message);
	}




}


void HUD::showRaceOver(bool show)
{
	if (show)
	{
		mRacingDone->show();
	}
	else
	{
		mRacingDone->hide();
	}
}


void HUD::stopArrow(Kind type)
{
	mArrowStatus[(int) type] = false;
	mArrowTimeRemaining[(int) type] = 0;
	mArrowOverlay[(int) type]->hide();
	mArrowFlashTime[(int) type] = 0;

	SoundBank *sb = SoundBank::getInstance();

	if (type == Kind::left)
	{
		sb->fadeOut("left", 1000);

	}
	else if (type == Kind::right)
	{
		sb->fadeOut("right", 1000);

	}
	else if (type == Kind::down)
	{
		sb->fadeOut("down", 1000);

	}
	else if (type == Kind::center)
	{
		sb->fadeOut("center", 1000);
	}
	else if (type == Kind::up)
	{
		sb->fadeOut("up", 1000);
	}
	//	Sound::stopPlaying();
}

void HUD::startArrow(HUD::Kind type, float time /*= 5.0*/,  float flashDelay /* = 0.5 */)
{
	if (mArrowFlashTime != 0)
	{
		mArrowFlashTime[(int) type] = flashDelay;
	}
	mArrowFlashDelay[(int) type] = flashDelay;
	mArrowTimeRemaining[(int) type] = time;
	if (!mArrowStatus[(int) type])
	{
		SoundBank *sb = SoundBank::getInstance();

		mArrowStatus[(int) type] = true;
		if (mShowArrows && mShowHUDElems)
		{
			mArrowOverlay[(int) type]->show();
		}
		if (type == HUD::Kind::left)
		{
			sb->fadeIn("left", 100, true);
		}
		else if (type == HUD::Kind::right)
		{
			sb->fadeIn("right", 100, true);

		}
		else if (type == HUD::Kind::center)
		{
			sb->fadeIn("center", 100, true);
		}
		else if(type == HUD::Kind::down)
		{
			sb->fadeIn("down", 100, true);

		}
		else if(type == HUD::Kind::up)
		{
			sb->fadeIn("up", 100, true);

		}
	}

}
