#include "OgreVector3.h"
#include <queue>

namespace Ogre {
    class SceneNode;
    class SceneManager;
}

class BezierPath;
class Player;
class ItemQueue;
class HUD;
class PongCamera;
class TrackableObject;


class World
{
public:
	enum Barrier {BARRIER_NONE, BARRIER_LEFT,BARRIER_RIGHT,BARRIER_EDGES,BARRIER_CENTER};
	
    World(Ogre::SceneManager *sceneManager, HUD *hud);

    // You'll want various methods to access & change your world here
    
    void Think(float time);

	BezierPath *trackPath;

	void AddSegment(Ogre::Vector3 deltap1, Ogre::Vector3 deltap2, Ogre::Vector3 deltap3, Barrier b, float barrierPercent = 0.95f);


	void AddRandomSegment();
	void AddBarrierSegment();

	Ogre::Vector3 getWorldPosition(int segment, float percentage, float relativeX, float relativeY);
	Ogre::Vector3 getTrackUp(int segment, float percentage);

	
	void addCamera(PongCamera *c) { mCamera = c; }

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
	ItemQueue *Walls() { return mWalls; }
	ItemQueue *Coins() { return mCoins; }
	HUD *getHUD() { return mHUD; }

	void trackObject(TrackableObject *o);


protected:

	void clearBefore(ItemQueue *queue, int segment);

	Ogre::SceneManager *mSceneManager;

	const float width;

	std::vector<std::vector<Ogre::SceneNode *>> mTrackSceneNodes;

	void addTrackNodes(int segmentIndextToAdd, Barrier b = BARRIER_NONE, float barrierPercent = 0.9);
	void addPoints(float percent, int segmentIndexToAdd, std::vector<Ogre::Vector3> &points, std::vector<Ogre::Vector3> &normals, Barrier b = BARRIER_NONE);


	int mNumSegments;
	int mUnitsPerSegment;
	
	Player *mPlayer;

	float mUnitsPerPathLength;

	int mMeshIDIndex;
	ItemQueue *mCoins;
	ItemQueue *mWalls;
	int mLastCoinAddedSegment;
	HUD *mHUD;
	PongCamera *mCamera;
};
