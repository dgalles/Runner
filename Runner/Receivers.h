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
  
  int leftCoinsCollected;
  int rightCoinsCollected;
  int middleCoinsCollected;
  int leftCoinsMissed;
  int middleCoinsMissed;
  int rightCoinsMissed;
  float speed;

  //PlyrData()
  //{
  //  id = "";
  // leftCoinsCollected = 0;
  // rightCoinsCollected = 0;
  // middleCoinsCollected = 0;
  // leftCoinsMissed = 0;
  // middleCoinsMissed = 0;
  // rightCoinsMissed = 0;
  // speed = 0;
  //}

  PlyrData(Ogre::String ID = "", 
           int leftCollect = 0, 
           int rightCollect = 0, 
           int middleCollect = 0,
		   int leftMissed = 0,
		   int rightMissed = 0,
		   int middleMissed = 0,
		   float spd = 0) :
  id(ID), leftCoinsCollected(leftCollect), rightCoinsCollected(rightCollect),middleCoinsCollected(middleCollect),
	  leftCoinsMissed(leftMissed), rightCoinsMissed(rightMissed), middleCoinsMissed(middleMissed), speed(spd)
  {

  }

  PlyrData(PlyrData *data)
  {
	  id = data->id;
	  leftCoinsCollected  = data->leftCoinsCollected;
	  rightCoinsCollected = data->rightCoinsCollected;
	  middleCoinsCollected = data->middleCoinsCollected;
	  leftCoinsMissed = data ->leftCoinsMissed;
	  middleCoinsMissed = data->middleCoinsMissed;
	  rightCoinsMissed = data->rightCoinsMissed;
	  speed = data->speed;
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

