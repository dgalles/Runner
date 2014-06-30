#include "OgreOverlay.h"
#include "OgreOverlayElement.h"
#include "OgreOverlayManager.h"
#include "LoginWrapper.h"
#include "JsonUtils.h"


LoginWrapper::LoginWrapper(void) : mLoggedIn(false)
{
	Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
	mSuccessOverlay = om.getByName("Login/Success");
	mFailureOverlay = om.getByName("Login/Failure");

	mFailureOverlay->show();

}


LoginWrapper::~LoginWrapper(void)
{
	Logout();
}


bool LoginWrapper::loggedIn()
{
	return mLoggedIn;

}

void LoginWrapper::changeUsername(Ogre::String username)
{
	if (mCurrentUsername != username)
	{
		mCurrentUsername = username;
		Logout();
	}

}


int debug_callback(CURL *handle,
                    curl_infotype type,
                    char *data,
                    size_t size,
                    void *userptr)
{

	int x = 0;
	x = x + 1;
	return 0;

}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	return nmemb * size;
}


bool loginSuccess;

size_t check_password(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string contents(ptr);

	std::string::size_type t = contents.find("Log In Required");

	if (t != std::string::npos)
	{
		loginSuccess = false;
	}
	else
	{
		loginSuccess = true;
	}

	return nmemb * size;

}


void LoginWrapper::Logout()
{

	if (mLoggedIn && mCurl != NULL)
	{
		curl_easy_cleanup(mCurl);
		mCurl = NULL;
	}
	mFailureOverlay->show();
	mSuccessOverlay->hide();
	mLoggedIn = false;


}


bool LoginWrapper::Login()
{
	if (mLoggedIn && mCurl != NULL)
	{
		curl_easy_cleanup(mCurl);
		mCurl = NULL;
	}


	struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    
    curl_global_init(CURL_GLOBAL_ALL);

    curl_formadd(&formpost, &lastptr,
                 CURLFORM_COPYNAME, "username",
				 CURLFORM_COPYCONTENTS, mCurrentUsername.c_str(),
				 CURLFORM_END);

	curl_formadd(&formpost, &lastptr,
		CURLFORM_COPYNAME, "password",
		CURLFORM_COPYCONTENTS, mCurrentPassword.c_str(),
		CURLFORM_END);

	mCurl = curl_easy_init();
	curl_easy_setopt(mCurl, CURLOPT_VERBOSE, 1);

	curl_easy_setopt(mCurl, CURLOPT_DEBUGFUNCTION,
		debug_callback);

	curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, check_password);

	if (mCurl) 
	{
		curl_easy_setopt(mCurl, CURLOPT_URL, "https://creamstout.cs.usfca.edu/login");
		curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(mCurl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(mCurl, CURLOPT_COOKIEFILE, "");
		curl_easy_perform(mCurl);
		curl_easy_reset(mCurl);
		curl_formfree(formpost);

	}

	if (loginSuccess)
	{
		mFailureOverlay->hide();
		mSuccessOverlay->show();

		Ogre::OverlayElement *userNameView = Ogre::OverlayManager::getSingleton().getOverlayElement("Login/Success/Text");
		mLoggedIn = true;
		userNameView->setCaption("Logged in as: " + mCurrentUsername);
	}
	else
	{
		mFailureOverlay->show();
		mSuccessOverlay->hide();
		mLoggedIn = false;
	}

	return mLoggedIn;

}


std::string data;


size_t get_profile_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	data = std::string(ptr);
	return nmemb * size;
}


size_t send_profile_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string retval(ptr);
	return nmemb * size;
}



std::string LoginWrapper::extractProfile(std::string data)
{
	int code = -1;
	std::string header = JSON_UTIL::getFromArray(data, 0);

	std::string message = JSON_UTIL::stripQuotes(JSON_UTIL::getFromDictionary(header, "\"message\""));
	std::string strCode = JSON_UTIL::getFromDictionary(header, "\"code\"");
	if (strCode.length() > 0)
	{
		code = atoi(strCode.c_str());
	}
	if (message == "ok" || code == 400)
	{
		std::string profile = JSON_UTIL::getFromArray(data,1);
		if (profile.length() == 0)
		{
			return "{}";
		}
		else
		{
			return profile;
		}
	}
	else
	{
		// Should probably throw error here ...
		return "{}";
	}
}

std::string LoginWrapper::getProfileData()
{
	if (mLoggedIn && mCurl != NULL)
	{
		data = std::string("");
		curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, get_profile_callback);
		curl_easy_setopt(mCurl, CURLOPT_URL, "https://creamstout.cs.usfca.edu/profile");
		curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(mCurl, CURLOPT_POST, 1);
		curl_easy_perform(mCurl);
		curl_easy_reset(mCurl);
		if (data.length() > 0)
		{
			return extractProfile(data);
		}
	}
	return "";
}


std::string LoginWrapper::changePassword(Ogre::String password)
{
	mCurrentPassword = password;

	if (Login())
	{
		return getProfileData();
	}
	else
	{
		return "";
	}



}


void  LoginWrapper::sendProfileData(std::string data)
{
	if (!mLoggedIn)
	{
		// Throw here?
		return;
	}

	struct curl_slist *headers = NULL;

	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, headers); 
	curl_easy_setopt(mCurl, CURLOPT_URL,"https://creamstout.cs.usfca.edu/profile");  
	curl_easy_setopt(mCurl, CURLOPT_CUSTOMREQUEST, "PUT"); 
	curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, get_profile_callback);

	curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, data.c_str()); 

	CURLcode res = curl_easy_perform(mCurl);

	curl_slist_free_all(headers);
}
