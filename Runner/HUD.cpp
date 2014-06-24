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

	mCoinsText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/ScorePanel/Coins");
	mDistanceText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/ScorePanel/Distance");
	mSpeedText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/ScorePanel/Speed");

	int armorIndex =  1;
	while (Ogre::OverlayManager::getSingleton().hasOverlayElement("HUD/Armor/" + std::to_string(armorIndex)))
	{
		Ogre::OverlayElement *armor  =Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/Armor/" + std::to_string(armorIndex));
		mArmorIndicator.push_back(armor);
		armorIndex++;
	}


}


void HUD::showHUDElements(bool show)
{
	mShowHUDElems = show;
	if (show)
	{
		mScoreOverlay->show();
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


void HUD::setCoins(int newScore)
{
	std::string score = "Coins = ";
	score.append(std::to_string((long long)newScore));
	mCoinsText->setCaption(score);
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


void HUD::setSpeed(int speed)
{
	std::string score = "Speed = ";
	score.append(std::to_string((long long)speed));
    score.append(" m / s");
	mSpeedText->setCaption(score);


}


void HUD::setArmorLevel(int level)
{
	for (unsigned int i = 0; i < mArmorIndicator.size(); i++)
	{
		if (level > i)
		{
			mArmorIndicator[i]->show();
		}
		else
		{
			mArmorIndicator[i]->hide();
		}

	}

}


void HUD::setDistance(int newScore)
{
	std::string score = "Distance = ";
	score.append(std::to_string((long long)newScore));
    score.append(" meters");
	mDistanceText->setCaption(score);
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

void HUD::stopArrow(Kind type)
{
    mArrowStatus[(int) type] = false;
    mArrowTimeRemaining[(int) type] = 0;
    mArrowOverlay[(int) type]->hide();
	mArrowFlashTime[(int) type] = 0;

	if (type == Kind::left)
	{
		SoundBank::getInstance()->fadeOut("BuzzsawRight1", 1000);

	}
	else if (type == Kind::right)
	{
		SoundBank::getInstance()->fadeOut("BuzzsawLeft1", 1000);

	}
	else if (type == Kind::down)
	{
		SoundBank::getInstance()->fadeOut("BuzzsawCenterHigh1", 1000);

	}
	else if (type == Kind::center)
	{
		SoundBank::getInstance()->fadeOut("BuzzsawCenter1", 1000);
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
		mArrowStatus[(int) type] = true;
		if (mShowArrows && mShowHUDElems)
		{
			mArrowOverlay[(int) type]->show();
		}
		if (type == HUD::Kind::left)
		{
			SoundBank::getInstance()->fadeIn("BuzzsawRight1", 100, true);
		}
		else if (type == HUD::Kind::right)
		{
			SoundBank::getInstance()->fadeIn("BuzzsawLeft1", 100, true);

		}
		else if (type == HUD::Kind::center)
		{
			SoundBank::getInstance()->fadeIn("BuzzsawCenter1", 100, true);
		}
		else if(type == HUD::Kind::down)
		{
			SoundBank::getInstance()->fadeIn("BuzzsawCenterHigh1", 100, true);

		}
		else if(type == HUD::Kind::up)
		{

		}
	}

}
