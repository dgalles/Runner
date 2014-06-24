#include "LoginWrapper.h"
#include "OgreOverlay.h"
#include "OgreOverlayElement.h"
#include "OgreOverlayManager.h"

LoginWrapper::LoginWrapper(void) : mLoggedIn(false)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mSuccessOverlay = om.getByName("Login/Success");
	mFailureOverlay = om.getByName("Login/Failure");

	mFailureOverlay->show();

}


LoginWrapper::~LoginWrapper(void)
{
}



bool LoginWrapper::loggedIn()
{
	return mLoggedIn;

}

void LoginWrapper::changeUsername(Ogre::String username)
{
	mCurrentUsername = username;
	mFailureOverlay->show();
	mSuccessOverlay->hide();
	mLoggedIn = false;

}
void LoginWrapper::changePassword(Ogre::String password)
{
	mCurrentPassword = password;
	//  Actually login here
	//   If login is successfull, then:

	mFailureOverlay->hide();
	mSuccessOverlay->show();
	Ogre::OverlayElement *userNameView = Ogre::OverlayManager::getSingleton().getOverlayElement("Login/Success/Text");

	userNameView->setCaption("Logged in as: " + mCurrentUsername);
	mLoggedIn = true;
}


void  LoginWrapper::sendProfileData(std::string data)
{
	if (!mLoggedIn)
	{
		// Throw here?
		return;
	}
	// Send data down the wire

}
std::string  LoginWrapper::getProfileData()
{
	if (!mLoggedIn)
	{
		// Throw here?
		return "";

	}

	// Get data from website
	return "";
}