#include "OgreVector3.h"
class World;
class XInputManager;
class Ogre::SceneNode;
class Kinect;
class RunnerObject;
class Achievements;



class TrackableObject
{
public:

	virtual Ogre::Vector3 worldPosition() { return Ogre::Vector3::ZERO; }
	virtual int currentSegment() { return mCurrentSegment; }
	virtual float segmentPercent() {  return mSegmentPercent; }


protected:
	float mSegmentPercent;
	int mCurrentSegment;

};


class Player : public TrackableObject
{

public:
	Player(World *world, XInputManager *inputManager, Kinect *k, Achievements *ach);

	void Think(float time);

	float reltiveX() { return mRelativeX;} 
	float reltiveY() { return mRelativeY;} 

	virtual Ogre::Vector3 worldPosition();
	void setPaused(bool p) { mPaused = p; }

    void setSpeed(int speed) {FORWARD_SPEED = speed;}
	int getSpeed() { return FORWARD_SPEED; }
    void setSpeedIncrease(int delta) { mSpeedIncrease = delta; }
	int getSpeedIncrease() {return mSpeedIncrease;} 
    void setKinectSentitivityLR(float sensitivity) {mKinectSensitivityLR = sensitivity; }
    void setKinectSentitivityFB(float sensitivity) {mKinectSensitivityFB = sensitivity; }
    void setLevel(int level);

    void startGame();
    void setAutoCallibrate(bool autoCal) { mAutoCallibrate = autoCal; }
    bool getAutoCallibrate() { return mAutoCallibrate; }

	bool getEnableKinect() { return mEnableKinect; }
	void setEnableKinect(bool enable) { mEnableKinect = enable; }

	bool getEnableKeyboard() { return mEnableKeyboard; }
	void setEnableKeyboard(bool enable) { mEnableKeyboard = enable; }

	bool getEnableGamepad() { return mEnableGamepad; }
	void setEnableGamepad(bool enable) { mEnableGamepad = enable; }


	void setUseFrontBack(bool ufb) {mUseFrontBack = ufb; }
	bool getUseFrontBack() { return  mUseFrontBack; }

	void setInvertControls(bool ufb) {mInvertControls = ufb; }
	bool getInvertControls() { return  mInvertControls; }



    void reset();

protected:
	void updateAchievements();

	void updateAnglesFromConrols(Ogre::Degree &angle, Ogre::Degree &angle2);

	void kill();
    void setup();
	void startArrows(int newSegment);
	void stopArrows(int segment, float percent);
	void moveExplosion(float time);
	bool detectCollision(int newSegment, float newPercent, float newX);
	void coinCollision(int newSegment, float newPercent, float newX);
	World *mWorld;
 	Kinect *mKinect;
	XInputManager *mXInputManager;
	float mRelativeX;
	float mRelativeY;
	bool mPaused;
    bool mAutoCallibrate;

	float mVelocity;
    RunnerObject *mPlayerObject;

	// Ogre::SceneNode * mPlayerSceneNode;

	Ogre::SceneNode *debris[4];

	int FORWARD_SPEED;
	int LATERAL_SPEED;
	int mCoinsCollected;
	bool mAlive;
	float mExplodeTimer;
    float mTimeSinceSpeedIncrease;
    int mSpeedIncrease;

	Ogre::Vector3 mExplosionforward;
	Ogre::Vector3 mExplosionright;
	Ogre::Vector3 mExplosionup;
    float mKinectSensitivityLR;
    float mKinectSensitivityFB;

    double mDistance;

	bool mEnableKinect;
	bool mEnableKeyboard;
	bool mEnableGamepad;

	float mTargetDeltaY;
	float mDeltaY;

		bool mUseFrontBack;
		bool mInvertControls;

	Achievements *mAchievements;
    static const float SPEED_MULTIPLYER;


	int mTotalCoins;
	float mTotalMeters;
	int mLongestRun;
	int mMostCoins;
	float mDistanceWithoutCoins;
	float mMaxDistWithoutCoins;
	float mDistSinceMissedCoin;
	bool mShielded;
	bool mBoosting;
	float mBoostTime;
	float mShieldTime;
	int mBoostsHit;
};