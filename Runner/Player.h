#include "OgreVector3.h"
class World;
class XInputManager;
class Ogre::SceneNode;
class Kinect;




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
	Player(World *world, XInputManager *inputManager, Kinect *k);

	void Think(float time);

	float reltiveX() { return mRelativeX;} 
	float reltiveY() { return mRelativeY;} 

	virtual Ogre::Vector3 worldPosition();
	void setPaused(bool p) { mPaused = p; }

protected:

	void kill();
	void startArrows(int newSegment);
	void moveExplosion(float time);
	bool detectCollision(int newSegment, float newPercent, float newX);
	void coinCollision(int newSegment, float newPercent, float newX);
	World *mWorld;
	Kinect *mKinect;
	XInputManager *mInputManager;
	float mRelativeX;
	float mRelativeY;
	bool mPaused;

	float mVelocity;
	Ogre::SceneNode * mPlayerSceneNode;

	Ogre::SceneNode *debris[4];

	float FORWARD_SPEED;
	float LATERAL_SPEED;
	int mCoinsCollected;
	bool mAlive;
	float mExplodeTimer;

		Ogre::Vector3 mExplosionforward;
	Ogre::Vector3 mExplosionright;
	Ogre::Vector3 mExplosionup;
};