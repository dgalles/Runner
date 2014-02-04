#pragma once

#include <windows.h>
#include <ole2.h>

#include "NuiApi.h"
#include "OgreVector2.h"

const float CALIBRATION_TIME = 4.0f;

namespace Ogre
{
	class Overlay;
	class OverlayElement;
}


class KinectMessageReceiver
{
public:
	virtual void callibrationStarted() { }
	virtual void callibrationCompleted() { }
};


class Kinect
{
public:
	Kinect(void);
	~Kinect(void);
	HRESULT initSensor();
	void update(float);

	void callibrate(float delay = 4.0f);
	void addListener(KinectMessageReceiver *listener);
	void shutdown();

	Ogre::Degree leftRightAngle() { return mLeftRightAngle; }
	Ogre::Degree frontBackAngle() { return mFrontBackAngle; }

protected:
	void updateKinectSkeleton( );


	// Current kinect
	INuiSensor *            m_pNuiSensor;
	BSTR                    m_instanceId;
	HANDLE        m_hNextSkeletonEvent;
	DWORD         m_SkeletonIds[NUI_SKELETON_COUNT];
	DWORD         m_TrackedSkeletonIds[NUI_SKELETON_MAX_TRACKED_COUNT];
	HANDLE        m_hThNuiProcess;
	HANDLE        m_hEvNuiProcessStop;

	static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);
	DWORD WINAPI            Nui_ProcessThread();

	bool   recenterNext;
	bool   updateDelay; 

	Ogre::Vector2 baseVectorDelta;


	Ogre::Degree mLeftRightAngle;
	Ogre::Degree mFrontBackAngle;

	float mTimeSinceLastUpdate;
	float mCalibrationClock;

	Ogre::Overlay *mToso1Overlay;
	Ogre::Overlay *mToso2Overlay;
	Ogre::Overlay *mCallibrationOverlay;
	bool mCallibrating;
	Ogre::OverlayElement *mCallibrationText;
	std::vector<KinectMessageReceiver*> mListeners;

};

