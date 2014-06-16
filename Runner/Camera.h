namespace Ogre
{
    class Camera;
    class Vector3;
}
class World;
class TrackableObject;


class RunnerCamera
{


public:

	enum FollowType {CLOSE, NORMAL, HIGH};

    RunnerCamera(Ogre::Camera *renderCamera, World *world); 
    void Think(float time);
    // If you have a different cameras, you'll want some acessor methods here.
    //  If your camera always stays still, you could remove this class entirely

	void TrackObject(TrackableObject *objectToTrack);
	void SetFollowType(FollowType typ);
	void Pause();
	void UnPause();
protected:

    Ogre::Camera *mRenderCamera;
    World *mWorld;

	int currentSegment;
	float currentPercent;
	int direction;
	bool mPaused;
	TrackableObject *mCurrentTrackingObject;
	Ogre::Vector3 oldPosition;
	FollowType mType;

	float mDesiredFollowDistance;
	float mDesiredFollowHeight;

	float mCurrentFollowDistance;
	float mCurrentFollowHeight;

};