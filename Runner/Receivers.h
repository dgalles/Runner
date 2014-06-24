#ifndef __RECVRS_H
#define __RECVRS_H

#include "OgreString.h"


struct Session
{
	int Score;
	char timeStamp[sizeof "2011-10-08T07:07:09Z"];
	int level;
};


struct PlyrData
{
  Ogre::String id;
  
  int totalScore;
  int currency;
  int sessionsPlayed;
  Session LastSession;

  PlyrData()
  {
    id = "";
    totalScore = 0;
    currency = 0;
    sessionsPlayed = 0;
  }

  PlyrData(Ogre::String ID, 
           int tScore = 0, 
           int cur = 0, 
           int seshPlayed = 0)
  {
    id = ID;
    totalScore = tScore;
    currency = cur;
    sessionsPlayed = seshPlayed;
  }

  PlyrData(PlyrData *data)
  {
    id = data->id;
    totalScore = data->totalScore;
    currency = data->currency;
    sessionsPlayed = data->sessionsPlayed;
  }
};

class PlyrDataMsgr
{
public:
  virtual void ReceivePlayerID(int playerid) { }
  virtual void CreateDBProfile(int playerid) { }
  virtual void ReceivePlyrData(PlyrData *data) { }

  float LogInterval(void) { return mPlyrLogInterval; }

protected:
  float mPlyrLogInterval;
};

struct SkelData
{
  float lrAngle;
  float fbAngle;
  float lrAngleTrue;

  SkelData()
  {
    lrAngle = 0.0f;
    fbAngle = 0.0f;
	lrAngleTrue = 0.0f;
  }

  SkelData(float lr, float fb, float lrt)
  {
    lrAngle = lr;
    fbAngle = fb;
	lrAngleTrue = lrt;
  }
  SkelData(SkelData *sd)
  {
    lrAngle = sd->lrAngle;
    fbAngle = sd->fbAngle;
	lrAngleTrue = sd->lrAngleTrue;
  }
};

class KinectSkelMsgr
{
public:
  virtual void ReceiveSkelData(SkelData *data) { }

protected:
  float mSkelLogInterval;
};

/*-----------------------------------------------------------*/
class DBMsgr
{
public:
  virtual bool IsConnected(void) { return mConnected; }
  virtual int Connect(void) { return 1; }
  virtual void Disconnect(void) { }

protected:
  int mPlayerId;
  bool mConnected;
  char *mHost;
  char *mPort;
};

/*-----------------------------------------------------------*/
class SessionListener
{
public:
 virtual void StartSession() { }
  virtual void EndSession() { }

protected:
  bool mSessionStarted;
  bool mSessionEnded;
};
#endif // !__RECVRS_H

