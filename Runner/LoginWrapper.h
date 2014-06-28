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
	LoginWrapper(void);
	void changeUsername(Ogre::String username);
	std::string changePassword(Ogre::String password);
	bool loggedIn();

	void sendProfileData(std::string data);
	std::string getProfileData();


	bool Login();
	void Logout();

	Ogre::String getUsername() { return mCurrentUsername;} 
	~LoginWrapper(void);

protected:

	std::string extractProfile(std::string data);
	Ogre::String mCurrentUsername;
	Ogre::String mCurrentPassword;


	Ogre::Overlay *mSuccessOverlay;
	Ogre::Overlay *mFailureOverlay;
	bool mLoggedIn;
	 CURL * mCurl;
};

