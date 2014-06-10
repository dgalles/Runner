#pragma once
#include "OgreString.h"


namespace Ogre
{
	class Overlay;
	class OverlayElement;
}

class Achievements
{
public:
	Achievements(void);

	void Think(float time);

	void AchievementCleared(int index, float timeToDisplay = 5);
	void AchievementCleared(Ogre::String achievement, float timeToDisplay = 5);
	void ReadAchievements(Ogre::String filename);

	void AddAchievement(Ogre::String ShortName, Ogre::String LongName);
	void ResetActive();
	void clearUI();

	void DisplayActiveAchievements(float time = 3);
	~Achievements(void);


protected:

	void setupCompletedUI(Ogre::String message, float timeToDisplay);

	static const int ACTIVE_ACHIEVEMENTS  = 3;

	std::vector<Ogre::String> mAchievmentsShort;
	std::vector<Ogre::String> mAchievmentsLong;
	std::vector<bool> mAchievmentsCleared;
	std::vector<bool> mAchievmentsActive;
	std::map<Ogre::String, int> mShortNameToIndex;


	Ogre::Overlay *mCompletedAchievement[3];
	Ogre::Overlay *mActiveAchievements;

	float mShowActiveTimeRemaining;
	float mShowCompletedTimeRemaining[3];
	std::vector<Ogre::String> AchievmentOrder;

	Ogre::OverlayElement *mCompletedText[ACTIVE_ACHIEVEMENTS];
	Ogre::OverlayElement *mActiveText[ACTIVE_ACHIEVEMENTS];

	int mCompletedActive;

};

