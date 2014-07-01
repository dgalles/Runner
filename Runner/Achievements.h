#pragma once
#include "OgreString.h"


namespace Ogre
{
	class Overlay;
	class OverlayElement;
	class TextAreaOverlayElement;
	class OverlayContainer;

}

class Achievements
{
public:
	// Achievements(void);
	Achievements(Ogre::String achievmentFilename = "");

	void Think(float time);

	// Given the index for an achievement, clear the achievment (if it has not already
	// been cleared, AND is currently active) and then displayu a cleared message for timeToDsiplay seconds
	// (if timeToDisplay is zero, then do no display, even if active)  If force is true, then 
	void AchievementCleared(int index, bool force = false, float timeToDisplay = 5);

	// Given the short name for an achievement, clear the achievment (if it has not already
	// been cleared, AND is currently active) and then displayu a cleared message for timeToDsiplay seconds
	// (if timeToDisplay is zero, then do no display, even if active)  If force is true, then 
	void AchievementCleared(Ogre::String shortName, bool force = false, float timeToDisplay = 5);

	// Read all achievments from a file
	void ReadAchievements(Ogre::String filename);

	// Add a new achievement. 
	void AddAchievement(Ogre::String ShortName, Ogre::String LongName);

	//  Called at the start of a game, this method resets the active achievements
	//  The top three unachieved acheiemvnets become active
	void ResetActive();

	// Removes all acheievemnt UI elements
	void clearUI();

	// Display all acheievemnts.  Should be called (with show = true)  only when the game is paused, since this method
	// causes the screen to be covered in achievments and also hijacks the keyboard for scrolling
	void ShowAllAchievements(bool show);

	// Display active acheievemnts.  Should be called at the start of the game (or any time you want to
	// show the currntly active achievements.
	void DisplayActiveAchievements(float time = 3);

	// Are we currently showing achievements?  A bit of a hack to get menus &etc working properly
	bool ShowingAllAchievements() { return mViewAllAchievements; }

	std::string getCompletedAchievements();
	void setCompletedAchievements(std::string json);

	void ResetAll();

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

	bool mViewAllAchievements;
	int mCompletedActive;

	std::vector< Ogre::TextAreaOverlayElement*> mAchievemntTextAreas;
	std::vector< Ogre::TextAreaOverlayElement*> mTitles;
	 Ogre::OverlayContainer *mAchievementPanel;

	 float mMaxScroll;
	Ogre::Overlay *mAllOverlay;
};

