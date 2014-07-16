#include "OgreVector3.h"
#include "Bezier.h"
#include <queue>
#include "ItemQueue.h"

namespace Ogre {
	class SceneNode;
	class SceneManager;
}

class BezierPath;
class Player;
class HUD;
class RunnerCamera;
class TrackableObject;
class Runner;


class World
{
public:

	World(Ogre::SceneManager *sceneManager, HUD *hud, Runner *base, bool useMirror = false);

	// You'll want various methods to access & change your world here

	void Think(float time);

	BezierPath *trackPath;

	void AddSegment(Ogre::Vector3 deltap1, Ogre::Vector3 deltap2, Ogre::Vector3 deltap3, BezierPath::Kind type = BezierPath::Kind::NORMAL);

	void reset(int seed = -1, bool fromGhost = false);
	void resetToDefaults();

	void AddRandomSegment();
	void AddBarrierSegment(BezierPath::Kind type);

	Ogre::Vector3 getWorldPosition(int segment, float percentage, float relativeX, float relativeY);
	Ogre::Vector3 getTrackUp(int segment, float percentage);


	void addCamera(RunnerCamera *c) { mCamera = c; }
	RunnerCamera *getCamera() { return mCamera;}

	void removeWorldSegment(int index);

	void endGame();

	void RemovePathSegment(int start, int end);
	Ogre::SceneManager *SceneManager() { return mSceneManager; }

	void getWorldPositionAndMatrix(int segment, float percentage, float relativeX, float relativeY, Ogre::Vector3 &worldPostion, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up, bool isSecondPlayer);

	void clearCoinsBefore(int segment);
	void clearBarriersBefore(int segment);


	int lastCoinAddedSegment(bool player) { return mLastCoinAddedSegment[(int) player];} 
	void addCoins(int segmentToAdd, bool player);
	void addCoins(bool player);
	void AddTwisty();
	void AddLoop(); 
	void AddJump(); 
	void AddNormalSegment();
	void setUseFrontBack(bool ufb) {mUseFrontBack = ufb; }
	bool getUseFrontBack() { return  mUseFrontBack; }

	void setObstacleFrequency(float freq) {mObsFreq = freq; }
	float getObstacleFrequency() { return  mObsFreq; }


	void setObstacleSeparation(int gap) {mObsGap = gap; }
	int getObstacleSeparation() { return  mObsGap; }

	void AddObjects(int segment, bool player);

	ItemQueue<ItemQueueData> *SawPowerup() { return mSawPowerup; }
	ItemQueue<ItemQueueData> *Coins() { return mCoins; }
	HUD *getHUD() { return mHUD; }

	void trackObject(TrackableObject *o);
	void setUnitsPerPathLength(float uppl) { mUnitsPerPathLength = uppl; }

	bool getUsingSimpleMaterials() { return mSimpleMaterials; }
	void setUsingSimpleMaterials(bool simple) {  mSimpleMaterials = simple; }

	int getCoinsMissedLeft() { return mLeftCoinsMissed;}
	int getCoinsMissedRight() { return mRightCoinsMissed; }
	int getCoinsMissedMiddle() { return mMiddleCoinsMissed; }


	int getShieldFreq() { return mShieldFreq; }
	void setShieldFreq(int n) { mShieldFreq = n; }

	int getBoostFreq() { return mBoostFreq; }
	void setBoostFreq(int n) { mBoostFreq = n; }

	int getMagnetFrequency() { return mMagnetFreq; }
	void setMagnetFrequency(int n) { mMagnetFreq = n; }

	int getSeed() { return mInitialSeed;}


protected:



	int worldRand();

	int mLeftCoinsMissed;
	int mRightCoinsMissed;
	int mMiddleCoinsMissed;

	void setup(int seed = -1, bool fromGhost = false);

	void clearBefore(ItemQueue<ItemQueueData> *queue, int segment, bool isCoins);

	Ogre::SceneManager *mSceneManager;

	const float width;

	std::vector<std::vector<Ogre::SceneNode *>> mTrackSceneNodes;

	void addTrackNodes(int segmentIndextToAdd, bool startCap = false, bool endCap = false);
	void addTrackNodesOne(int segmentIndextToAdd, bool mirrored, bool startCap = false, bool endCap = false);
	void addPoints(float percent, int segmentIndexToAdd, std::vector<Ogre::Vector3> &points, std::vector<Ogre::Vector3> &normals, bool mirror);


	bool mUseMirror;

	int mUnitsPerSegment;

	Player *mPlayer;

	float mUnitsPerPathLength;

	int mMeshIDIndex;
	ItemQueue<ItemQueueData> *mCoins;
	ItemQueue<ItemQueueData> *mSawPowerup;
	int mLastCoinAddedSegment[2];
	HUD *mHUD;
	RunnerCamera *mCamera;

	float mObsFreq;
	bool mUseFrontBack;
	int mLastObjSeg;
	int mObsGap;

	bool mSimpleMaterials;
	int mInitialSeed;
	Runner *mBase;

	int mShieldFreq;
	int mBoostFreq;
	int mMagnetFreq;

	int mSeed;

	bool mGhosting;
	bool mDrawTrack;

	std::vector<int> mRandom;
	int randomIndex;
};
