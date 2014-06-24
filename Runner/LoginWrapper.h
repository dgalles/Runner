#pragma once
#include "OgreString.h"

namespace Ogre
{
	class Overlay;
	class OverlayElement;
}

class LoginWrapper
{
public:
	LoginWrapper(void);
	void changeUsername(Ogre::String username);
	void changePassword(Ogre::String password);
	bool loggedIn();

	void sendProfileData(std::string data);
	std::string getProfileData();

	Ogre::String getUsername() { return mCurrentUsername;} 
	~LoginWrapper(void);

private:
	Ogre::String mCurrentUsername;
	Ogre::String mCurrentPassword;


	Ogre::Overlay *mSuccessOverlay;
	Ogre::Overlay *mFailureOverlay;
	bool mLoggedIn;

};

