#include "OgreVector3.h"
#include "Receivers.h"
class World;
class XInputManager;
class Ogre::SceneNode;
class Kinect;
class RunnerObject;
class Achievements;
class Ghost;



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
	Player(World *world, XInputManager *inputManager, Kinect *k, Achievements *ach, bool isSecond = false);

		enum RaceType {TOTALCOINS = 0, CONCECUTIVECOINS = 1};


	void resetToDefaults();

	void Think(float time);

	float reltiveX() { return mRelativeX;} 
	float reltiveY() { return mRelativeY;} 

	virtual Ogre::Vector3 worldPosition();
	void setPaused(bool p) { mPaused = p; }

	void setInitialSpeed(int speed) {mInitialSpeed = (float) speed;}
	int getInitialSpeed() { return (int) mInitialSpeed; }
	void setKinectSentitivityLR(float sensitivity) {mKinectSensitivityLR = sensitivity; }
	void setKinectSentitivityFB(float sensitivity) {mKinectSensitivityFB = sensitivity; }
	void setLevel(int level);

	void setLeanEqualsDuck(bool val);
	bool getLeanEqualsDuck() {return mLeanEqualsDuck; }
	void setGhost(Ghost *g) { mGhost = g; }

	int getTotalCoins() { return mTotalCoins; }
	void setTotalCoins(int totalCoins) { mTotalCoins = totalCoins; }

	int getLifetimeCoins() { return mLifetimeCoins; }
	void setLifetimeCoins(int livetimeCoins) { mLifetimeCoins = livetimeCoins; }


	void startGame();
	void setAutoCallibrate(bool autoCal) { mAutoCallibrate = autoCal; }
	bool getAutoCallibrate() { return mAutoCallibrate; }

	bool getEnableKinect() { return mEnableKinect; }
	void setEnableKinect(bool enable) { mEnableKinect = enable; }

	bool getEnableKeyboard() { return mEnableKeyboard; }
	void setEnableKeyboard(bool enable) { mEnableKeyboard = enable; }

	bool getEnableGamepad() { return mEnableGamepad; }
	void setEnableGamepad(bool enable) { mEnableGamepad = enable; }

	int getMaxSpeed() {return (int) mMaxSpeed; }
	void setMaxSpeed(int newSpeed) { mMaxSpeed = (float) newSpeed;}

	void setUseFrontBack(bool ufb) {mUseFrontBack = ufb; }
	bool getUseFrontBack() { return  mUseFrontBack; }

	void setInvertControls(bool ufb) {mInvertControls = ufb; }
	bool getInvertControls() { return  mInvertControls; }

	void setAutoAceelerateRate(int rate) { mAutoAccel = rate;} 
	int getAutoAccelerateRate() { return mAutoAccel; }

	void setManualAceelerateRate(int rate) { mManualAccel = rate;} 
	int getManualAccelerateRate() { return mManualAccel; }

	void setInitialArmor(int initialArmor) { mInitialArmor = initialArmor; }
	int getInitialArmor() { return mInitialArmor; }

	void setTrackLookahed(int look) { mTrackLookahead = look; }
	int getTrackLookahead() { return mTrackLookahead; }

	float getTotalMeters() { return mTotalMeters; }
	void  setTotalMeters(float meters) {  mTotalMeters = meters; }

	void setArrowDistance(int newDelta) { mWarningDelta = newDelta; }
	int getArrowDistance() { return mWarningDelta; }

	void addPlayerListener(PlyrDataMsgr *listener) { mLogger.push_back(listener); }

	void reset(Ghost::GhostInfo *ghostInfo = NULL);

	void setRacing(bool racing) { mRacing = racing; }
	bool getRacing() { return mRacing; } 

	int getBoostDuration() { return mBoostDuration; }
	void setBoostDuration(int n) { mBoostDuration = n; }

		void setRaceType(RaceType raceType) {mRaceType = raceType; }
	RaceType getRaceType() { return mRaceType; }

	void setRaceGoal(int goal) { mRaceGoal = goal; }
	int getRaceGoal() { return mRaceGoal; }



	int getShieldDuration() { return mShieldDuration; }
	void setShieldDuration(int n) { mShieldDuration = n; }

	int getMagnetDuration() { return mMagnetDuration; }
	void setMagnetDuration(int n) { mMagnetDuration = n; }

	void setGhostInfo(Ghost::GhostInfo *ghostInfo);

protected:
	void updateAchievements();
	void SendData(float time);
	void updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2);

	void kill();
	void finishRace();
	void setup(Ghost::GhostInfo *ghostInfo = NULL);
	void startArrows(int newSegment);
	void stopArrows(int segment, float percent);
	void moveExplosion(float time);
	bool detectCollision(int newSegment, float newPercent, float newX);
	void coinCollision(int newSegment, float newPercent, float newX, float time);
	void handleTimers(float time);
	World *mWorld;
	Kinect *mKinect;
	XInputManager *mXInputManager;
	float mRelativeX;
	float mRelativeY;
	bool mPaused;
	bool mAutoCallibrate;
	bool mIsSecondPlayer;
	bool mWonRace;

	// HACK!! TODO:  REMOVE/REFACTOR
	bool mWaitingOnKey;


	Ogre::SceneNode *mMagnetNode;

	int mConsecutiveCoins;
	Ghost *mGhost;

	float mVelocity;
	RunnerObject *mPlayerObject;

	// Ogre::SceneNode * mPlayerSceneNode;

	Ogre::SceneNode *debris[4];

	int mTrackLookahead;

	float mTimeSinceLastLog;
	std::vector<PlyrDataMsgr *> mLogger;

	float mCurrentSpeed;
	float mInitialSpeed;
	float mCurrInitialSpeed;

	int mCoinsCollected;
	bool mAlive;
	float mExplodeTimer;
	float mTimeSinceSpeedIncrease;

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

	int mArmor;
	int mInitialArmor;
	int mCurrInitialArmor;

	int mAutoAccel;
	int mCurrAutoAccel;
	int mManualAccel;
	int mCurrManualAccel;

	Achievements *mAchievements;
	static const float SPEED_MULTIPLYER;

	float mTime;

	float *mBoostDurationValues;
	float *mShieldDurationValues;
	float *mMagnetDurationValues;

	int mWarningDelta;
	int mLifetimeCoins;
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
	int mShieldsHit;
	int mMagnetsHit;
	float mMagnetTime;
	float mMagnetActive;
	bool mLeanEqualsDuck;
	bool mCurrLeanEqualsDuck;
	float mMaxSpeed;
	float mCurrMaxSpeed;

	int mLeftCoinsCollected;
	int mRightCoinsCollected;
	int mMiddleCoinsCollected;

	int mShieldDuration;
	int mBoostDuration;
	int mMagnetDuration;

	int mCurrShieldDuration;
	int mCurrBoostDuration;
	int mCurrMagnetDuration;


		bool mRacing;
	bool mCurrRacing;

	RaceType mCurrRaceType;
	int mCurrRaceGoal;

	RaceType mRaceType;
	int mRaceGoal;

};