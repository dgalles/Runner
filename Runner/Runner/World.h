#include "OgreVector3.h"
#include "Bezier.h"
#include <queue>

namespace Ogre {
    class SceneNode;
    class SceneManager;
}

class BezierPath;
class Player;
class ItemQueue;
class HUD;
class RunnerCamera;
class TrackableObject;


class World
{
public:
	
    World(Ogre::SceneManager *sceneManager, HUD *hud);

    // You'll want various methods to access & change your world here
    
    void Think(float time);

	BezierPath *trackPath;

	void AddSegment(Ogre::Vector3 deltap1, Ogre::Vector3 deltap2, Ogre::Vector3 deltap3, BezierPath::Kind type = BezierPath::Kind::NORMAL);

    void reset();

	void AddRandomSegment();
	void AddBarrierSegment(BezierPath::Kind type);

	Ogre::Vector3 getWorldPosition(int segment, float percentage, float relativeX, float relativeY);
	Ogre::Vector3 getTrackUp(int segment, float percentage);

	
	void addCamera(RunnerCamera *c) { mCamera = c; }
	RunnerCamera *getCamera() { return mCamera;}

	void removeWorldSegment(int index);

	
	void RemovePathSegment(int start, int end);
	Ogre::SceneManager *SceneManager() { return mSceneManager; }

	void getWorldPositionAndMatrix(int segment, float percentage, float relativeX, float relativeY, Ogre::Vector3 &worldPostion, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up);

	void clearCoinsBefore(int segment);
	void clearBarriersBefore(int segment);


	int lastCoinAddedSegment() { return mLastCoinAddedSegment;} 
	void addCoins(int segmentToAdd);
	void addCoins();
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

	void AddObjects(int segment);

    ItemQueue *SawPowerup() { return mSawPowerup; }
	ItemQueue *Coins() { return mCoins; }
	HUD *getHUD() { return mHUD; }

	void trackObject(TrackableObject *o);
	void setUnitsPerPathLength(float uppl) { mUnitsPerPathLength = uppl; }


protected:

    void setup();

	void clearBefore(ItemQueue *queue, int segment);

	Ogre::SceneManager *mSceneManager;

	const float width;

	std::vector<std::vector<Ogre::SceneNode *>> mTrackSceneNodes;

	void addTrackNodes(int segmentIndextToAdd, bool startCap = false, bool endCap = false);
	void addPoints(float percent, int segmentIndexToAdd, std::vector<Ogre::Vector3> &points, std::vector<Ogre::Vector3> &normals);



	int mUnitsPerSegment;
	
	Player *mPlayer;

	float mUnitsPerPathLength;

	int mMeshIDIndex;
	ItemQueue *mCoins;
    ItemQueue *mSawPowerup;
	int mLastCoinAddedSegment;
	HUD *mHUD;
	RunnerCamera *mCamera;

	float mObsFreq;
	bool mUseFrontBack;
	int mLastObjSeg;
	int mObsGap;

    bool mDrawTrack;
};
