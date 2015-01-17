#ifndef __LOGGER_H
#define __LOGGER_H
//
//#include "Kinect.h"
//#include "KinectSkelMsgr.h"

#include "Receivers.h"

#include <ctime>
#include <stdlib.h>
#include <queue>
#include <mutex>
#include <thread>


#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define DEFAULT_BUFSIZE 256

struct ServerCom;


typedef struct 
{
    SOCKET socket;
    SSL *sslHandle;
    SSL_CTX *sslContext;
} connection;


class Logger : public PlyrDataMsgr, 
               public KinectSkelMsgr, 
               public DBMsgr, 
               public SessionListener
{
public:
  Logger();
  ~Logger();


  	void changeUsername(Ogre::String username);
	std::string changePassword(Ogre::String password);
	bool loggedIn() {  return mLoggedIn;  }

	void sendProfileData(std::string data);
	std::string getProfileData();


	bool Login();
	void Logout();

	Ogre::String getUsername() { return mCurrentUsername;} 



  //bool TrackFullSkel(void) { return mTrackFullSkel; }
  virtual void ReceiveSkelData(SkelData *data);

  /* DBMsgr inherited functions */
  virtual bool IsConnected(void) { return mConnected; }

  /* PlyrDataMsgr inherited functions */
  virtual void ReceivePlyrData(PlyrData *data);


  virtual void StartSession();
  virtual void EndSession();

protected:

  int LogAndSend(const char* attribName, int value);
  int LogAndSend(const char* attribName, float value);


  connection *mSecureConnection;

  	Ogre::String mCurrentUsername;
	Ogre::String mCurrentPassword;

	bool mLoggedIn;

  char mPlyrBuf[DEFAULT_BUFSIZE];
  char mKinBuf1[DEFAULT_BUFSIZE];
  char mKinBuf2[DEFAULT_BUFSIZE];

private:
  time_t mTimeStampBeg;
  time_t mTimeStampEnd;
  int mSeconds;
  int mMinutes;
  int mSeconds2;
  int mMinutes2;
  char mBegBuf[sizeof "2011-10-08T07:07:09Z"];
  char mEndBuf[sizeof "2011-10-08T07:07:09Z"];
  std::queue<SkelData *> mSkelData;
  std::queue<PlyrData *> mPlyrData;
  std::mutex mSkelLock;
  std::mutex mPlyrLock;
  std::mutex mDbLock;
  std::thread mDaemon;

  
  void WriteToLog(char *buf, size_t buflen);
  void daemonFunc();
  void daemonSendPlyrData(PlyrData *pdata);
  void daemonSendSkelData(SkelData *pdata);

  int DB_Connect(char *host, char *port, ServerCom *com);
  int DB_Disconnect(ServerCom *com);
  int DB_Send(char *buf, size_t buflen, ServerCom *com);
  int DB_CreateEntry();
};



#endif // !__LOGGER_H
