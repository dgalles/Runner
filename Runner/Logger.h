#ifndef __LOGGER_H
#define __LOGGER_H
//
//#include "Kinect.h"
//#include "KinectSkelMsgr.h"

#include "Receivers.h"
#include "LoginWrapper.h"

#include <ctime>
#include <stdlib.h>
#include <queue>
#include <mutex>
#include <thread>

#define DEFAULT_BUFSIZE 256

struct ServerCom;

class LoginWrapper;

class Logger : public PlyrDataMsgr, 
               public KinectSkelMsgr, 
               public DBMsgr, 
               public SessionListener
{
public:
  Logger(LoginWrapper *login);
  ~Logger();

  //bool TrackFullSkel(void) { return mTrackFullSkel; }
  virtual void ReceiveSkelData(SkelData *data);

  /* DBMsgr inherited functions */
  virtual bool IsConnected(void) { return mConnected; }
  virtual int Connect(void);
  virtual void Disconnect(void);

  /* PlyrDataMsgr inherited functions */
  virtual void ReceivePlyrData(PlyrData *data);


  virtual void StartSession();
  virtual void EndSession();

protected:
  ServerCom *mSock;
  ServerCom *mScoreSock;
  ServerCom *mLRSock;
  ServerCom *mFBSock;
  char mPlyrBuf[DEFAULT_BUFSIZE];
  char mKinBuf1[DEFAULT_BUFSIZE];
  char mKinBuf2[DEFAULT_BUFSIZE];
  LoginWrapper *mLogin;

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

  Ogre::String mCurrentID;
  
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
