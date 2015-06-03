#pragma once
#define KINECT_AVAILABLE
#include <windows.h>
#include <ole2.h>

#ifdef KINECT_AVAILABLE

#include "NuiApi.h"
#pragma comment(lib, "Kinect10.lib")
#endif
#include "OgreVector2.h"
#include "Receivers.h"


const float CALIBRATION_TIME = 4.0f;

namespace Ogre
{
	class Overlay;
	class OverlayElement;
}




class Kinect : public SessionListener
{
public:
	Kinect(void);
	~Kinect(void);
	HRESULT initSensor();
	void update(float);

	void callibrate(float delay = 4.0f, std::function<void(void)> callback = NULL);
	void shutdown();

	Ogre::Degree leftRightAngle() { return mLeftRightAngle; }
	Ogre::Degree frontBackAngle() { return mFrontBackAngle; }

    bool callibrating() { return mCallibrating; }
    void cancelCallibration();
	void addSkelListener(KinectSkelMsgr *listener);
	virtual void StartSession();
	virtual void EndSession();

protected:

	void updateKinectSkeleton( );
	std::vector<KinectSkelMsgr *> mSkelListeners;

#ifdef KINECT_AVAILABLE
	// Current kinect
	INuiSensor *            m_pNuiSensor;
	BSTR                    m_instanceId;
	HANDLE        m_hNextSkeletonEvent;
	DWORD         m_SkeletonIds[NUI_SKELETON_COUNT];
	DWORD         m_TrackedSkeletonIds[NUI_SKELETON_MAX_TRACKED_COUNT];
	HANDLE        m_hThNuiProcess;
	HANDLE        m_hEvNuiProcessStop;
	Ogre::Vector3 mSkelPositions[NUI_SKELETON_POSITION_COUNT];

	static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);
	DWORD WINAPI            Nui_ProcessThread();
#endif

	bool   recenterNext;
	bool   updateDelay; 
	float  mTimeSinceLastLog;
	Ogre::Vector2 baseVectorDelta;


	Ogre::Degree mLeftRightAngle;
	Ogre::Degree mFrontBackAngle;
	Ogre::Degree mLeftRightTrue;

	float mTimeSinceLastUpdate;
	float mCalibrationClock;

	Ogre::Overlay *mToso1Overlay;
	Ogre::Overlay *mToso2Overlay;
	Ogre::Overlay *mCallibrationOverlay;
	bool mCallibrating;
	Ogre::OverlayElement *mCallibrationText;
    std::function<void()>  mCallibrationFinishedCallback;


};

