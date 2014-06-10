#include "HUD.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
 #include <OgreTextAreaOverlayElement.h>
#include "OgreFontManager.h"
#include "OgreOverlayElement.h"
#include "OgreIteratorWrappers.h"

HUD::HUD()
{

	Ogre::FontManager::getSingleton().getByName("Big")->load();

    mShowArrows = true;
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mArrowOverlay[HUD::left] = om.getByName("HUD/LeftArrow");
	mArrowOverlay[HUD::right] = om.getByName("HUD/RightArrow");
	mArrowOverlay[HUD::center] =om.getByName("HUD/CenterArrow");
	mArrowOverlay[HUD::down] =om.getByName("HUD/DownArrow");
	mArrowOverlay[HUD::up] =om.getByName("HUD/UpArrow");

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
}

void HUD::startArrow(HUD::Kind type, float time /*= 5.0*/,  float flashDelay /* = 0.5 */)
{
	if (mArrowFlashTime != 0)
	{
		mArrowFlashTime[(int) type] = flashDelay;
	}
	mArrowFlashDelay[(int) type] = flashDelay;
	mArrowStatus[(int) type] = true;
	mArrowTimeRemaining[(int) type] = time;
    if (mShowArrows && mShowHUDElems)
    {
	    mArrowOverlay[(int) type]->show();
    }
}
