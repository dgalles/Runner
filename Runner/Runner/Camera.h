namespace Ogre
{
    class Camera;
    class Vector3;
}
class World;
class TrackableObject;


class PongCamera
{


public:
    PongCamera(Ogre::Camera *renderCamera, World *world); 
    void Think(float time);
    // If you have a different cameras, you'll want some acessor methods here.
    //  If your camera always stays still, you could remove this class entirely

	void TrackObject(TrackableObject *objectToTrack);
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
	float mFollowDistance;
	Ogre::Vector3 oldPosition;
};