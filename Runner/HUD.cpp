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


	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mArrowOverlay[HUD::left] = om.getByName("HUD/LeftArrow");
	mArrowOverlay[HUD::right] = om.getByName("HUD/RightArrow");
	mArrowOverlay[HUD::center] =om.getByName("HUD/CenterArrow");

	for (int i = 0; i < HUD::last; i++)
	{
		mArrowTimeRemaining[i] = 0;
		mArrowStatus[i] = false;
	}

	mScoreOverlay = Ogre::OverlayManager::getSingleton().getByName("HUD/Score");
	mScoreOverlay->show();

	Ogre::String s = mScoreOverlay->getName();

	mScoreText = Ogre::OverlayManager::getSingleton().getOverlayElement("HUD/ScorePanel/Score");
	

}


void HUD::setScore(int newScore)
{
	std::string score = "Score = ";
	score.append(std::to_string((long long)newScore));
	mScoreText->setCaption(score);
}

void HUD::update(float time)
{
	for (int i = 0; i < (int) HUD::last; i++)
	{
		if (mArrowTimeRemaining[i] > 0)
		{
			if (mArrowTimeRemaining[i] < time)
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
					if (mArrowStatus[i])
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
}

void HUD::startArrow(HUD::Kind type, float time /*= 5.0*/,  float flashDelay /* = 0.5 */)
{
	mArrowFlashTime[(int) type] = flashDelay;
	mArrowFlashDelay[(int) type] = flashDelay;
	mArrowStatus[(int) type] = true;
	mArrowTimeRemaining[(int) type] = time;
	mArrowOverlay[(int) type]->show();
}
