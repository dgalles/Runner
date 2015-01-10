#pragma once
#include "OgreString.h"
 #include "curl/curl.h"


namespace Ogre
{
	class Overlay;
	class OverlayElement;
}
class LoginWrapper
{
public:
	LoginWrapper(Ogre::String server = "");
	void changeUsername(Ogre::String username);
	std::string changePassword(Ogre::String password);
	bool loggedIn();

	void sendProfileData(std::string data);
	std::string getProfileData();


	bool Login();
	void Logout();

	Ogre::String getUsername() { return mCurrentUsername;} 
	~LoginWrapper(void);
	std::string getServer() { return mServerAddress; }

protected:

	std::string extractProfile(std::string data);
	Ogre::String mCurrentUsername;
	Ogre::String mCurrentPassword;


	Ogre::String mServerAddress;
	Ogre::Overlay *mSuccessOverlay;
	Ogre::Overlay *mFailureOverlay;
	bool mLoggedIn;
	 CURL * mCurl;
};

