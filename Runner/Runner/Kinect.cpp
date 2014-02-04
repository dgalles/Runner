#include "Kinect.h"
#include "OgreVector2.h"
#include "OgreMath.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"



Kinect::Kinect(void): mListeners()
{
	mToso1Overlay = Ogre::OverlayManager::getSingleton().getByName("Kinect/Torso1");
	mToso2Overlay = Ogre::OverlayManager::getSingleton().getByName("Kinect/Torso2");
	mCallibrationText = Ogre::OverlayManager::getSingleton().getOverlayElement("Kinect/Calibrate2");


	mCallibrationOverlay = Ogre::OverlayManager::getSingleton().getByName("Kinect/CallibrationOverlay");

	mTimeSinceLastUpdate = 5.0; // Start out uncallibrated ...

	mToso1Overlay->show();
	mToso2Overlay->show();

	mToso1Overlay->setScroll(0.85, 0.8);
	mToso2Overlay->setScroll(0.65, 0.8);
}


Kinect::~Kinect(void)
{
}

HRESULT
Kinect::initSensor()
{
	HRESULT  hr;

	if ( !m_pNuiSensor  || true)
	{

		HRESULT hr = NuiCreateSensorByIndex(0, &m_pNuiSensor);

		if ( FAILED(hr) )
		{
			return hr;
		}

		m_instanceId = m_pNuiSensor->NuiDeviceConnectionId();
	}

	//DWORD nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON |  NUI_INITIALIZE_FLAG_USES_COLOR;
	DWORD nuiFlags =  NUI_INITIALIZE_FLAG_USES_SKELETON;
	hr = m_pNuiSensor->NuiInitialize( nuiFlags );


	if ( FAILED( hr ) )
	{
		return hr;
	}

	if ( HasSkeletalEngine( m_pNuiSensor ) )
	{
		m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

		hr = m_pNuiSensor->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT );
		if( FAILED( hr ) )
		{
			return hr;
		}
	}

	m_hEvNuiProcessStop = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hThNuiProcess = CreateThread( NULL, 0, Nui_ProcessThread, this, 0, NULL );
	return hr;

}

void 
Kinect::addListener(KinectMessageReceiver *listener)
{
	mListeners.push_back(listener);
}

void 
Kinect::callibrate(float delay)
{
	if (delay == 0)
	{
		recenterNext = true;
	}
	else
	{
		for (std::vector<KinectMessageReceiver *>::iterator it = mListeners.begin(); it != mListeners.end(); it++)
		{
			(*it)->callibrationStarted();
		}
		std::string message = "Callibration in ";

		long long printDelay = (long long) (delay + 0.99f);

		message.append(std::to_string(printDelay));

		message.append(" seconds");

		mCallibrationText->setCaption(message);
		mCalibrationClock = delay;
		mCallibrating = true;
		mCallibrationOverlay->show();

	}
}



void
Kinect::update(float time)
{
	mTimeSinceLastUpdate += time;
	if (mCallibrating)
	{
		if (mTimeSinceLastUpdate > 1.0)
		{
			mCallibrationText->setCaption("Kinect cannot see you. \nPlease adjust position");
		}
		else
		{

			mCalibrationClock -= time;
			if (mCalibrationClock <= 0)
			{
				for (std::vector<KinectMessageReceiver *>::iterator it = mListeners.begin(); it != mListeners.end(); it++)
				{
					(*it)->callibrationCompleted();
				}
				recenterNext = true;
				mCallibrating = false;	
				mCallibrationOverlay->hide();
			}
			else
			{

				std::string message = "Callibration in ";

				long long printDelay = (long long) (mCalibrationClock + 0.99f);

				message.append(std::to_string(printDelay));

				message.append(" seconds");

				mCallibrationText->setCaption(message);
			}

		}
	}
}


void
Kinect::updateKinectSkeleton()
{

	NUI_SKELETON_FRAME SkeletonFrame = {0};

	bool bFoundSkeleton = false;

	if ( SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame( 0, &SkeletonFrame )) )
	{
		for ( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ||
				(SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY)) 
			{
				bFoundSkeleton = true;
			}
		}
	}

	// no skeletons!
	if( !bFoundSkeleton )
	{
		return;
	}
	mTimeSinceLastUpdate = 0;


	// smooth out the skeleton data
	HRESULT hr = m_pNuiSensor->NuiTransformSmooth(&SkeletonFrame,NULL);
	if ( FAILED(hr) )
	{
		return;
	}

	int x = 0;

	bool bSkeletonIdsChanged = false;
	for ( int i = 0 ; i < NUI_SKELETON_COUNT; i++ )
	{
		if ( m_SkeletonIds[i] != SkeletonFrame.SkeletonData[i].dwTrackingID )
		{
			m_SkeletonIds[i] = SkeletonFrame.SkeletonData[i].dwTrackingID;
			bSkeletonIdsChanged = true;
		}

		// Show skeleton only if it is tracked, and the center-shoulder joint is at least inferred.
		if ( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			SkeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			// Here's our skeleton, let's update it.  Multiple tracked skeletons could be a problem,
			// fix that later



			NUI_SKELETON_DATA * pSkel =  &SkeletonFrame.SkeletonData[i];

			// TODO:  Check for     pSkel->eSkeletonPositionTrackingState[ JOINT ];
			Vector4 shoulderPos = pSkel->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER];
			Vector4 headPos = pSkel->SkeletonPositions[NUI_SKELETON_POSITION_HEAD];
			Vector4 leftShoulder =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT];
			Vector4 rightShoulder =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
			Vector4 leftElbow =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT];
			Vector4 rightElbow =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT];
			Vector4 leftWrist =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_WRIST_LEFT];
			Vector4 rightWrist =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT];
			Vector4 leftHand =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
			Vector4 rightHand =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];


			int x = 3;
			Ogre::Vector2 leftVector(leftShoulder.x - rightShoulder.x, leftShoulder.z - rightShoulder.z);
			leftVector.normalise();
			Ogre::Vector2 FrontVector(leftVector.y, -leftVector.x);

			Ogre::Vector2 BaseVector((leftElbow.x + rightElbow.x + leftShoulder.x + rightShoulder.x) / 4,
				(leftElbow.z + rightElbow.z + leftShoulder.z + rightShoulder.z) / 4);
			if (recenterNext)
			{
				recenterNext = false;
				baseVectorDelta = Ogre::Vector2(headPos.x, headPos.z) - BaseVector;
			}

			// Note:  headPos is a 3D point, baseVector is a 2D point, hence z/y confusion
			double xDisplacement = (headPos.x - BaseVector.x - baseVectorDelta.x) * leftVector.x + (headPos.z - BaseVector.y-baseVectorDelta.y) * leftVector.y;

			Ogre::Radian leftRightAngle1 = Ogre::Math::ATan2(-xDisplacement, headPos.y - shoulderPos.y + 0.5);
			mLeftRightAngle = leftRightAngle1 * 4;

			double ZDisplacement = (headPos.x - BaseVector.x-baseVectorDelta.x) * FrontVector.x + (headPos.z - BaseVector.y-baseVectorDelta.y) * FrontVector.y;

			Ogre::Radian frontBackAngle1 = Ogre::Math::ATan2(ZDisplacement, headPos.y - shoulderPos.y + 0.5);
			mFrontBackAngle = frontBackAngle1 * 4;


		}
		else if ( true && SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY )
		{
			// Position only, ignore for now.
		}
	}
	//mToso1Overlay->setRotate(Ogre::Radian(-mLeftRightAngle));

	mToso1Overlay->setRotate(Ogre::Radian(-mLeftRightAngle));
	mToso1Overlay->setScroll(0.85, 0.8);

	mToso2Overlay->setRotate(Ogre::Radian(-mFrontBackAngle));
	mToso2Overlay->setScroll(0.65, 0.8);
}


DWORD WINAPI Kinect::Nui_ProcessThread(LPVOID pParam)
{
	Kinect *pthis = (Kinect *) pParam;
	return pthis->Nui_ProcessThread();
}


void
Kinect::shutdown()
{
	    // Stop the Nui processing thread
    if ( NULL != m_hEvNuiProcessStop )
    {
        // Signal the thread
        SetEvent(m_hEvNuiProcessStop);

        // Wait for thread to stop
        if ( NULL != m_hThNuiProcess )
        {
            WaitForSingleObject( m_hThNuiProcess, INFINITE );
            CloseHandle( m_hThNuiProcess );
        }
        CloseHandle( m_hEvNuiProcessStop );
    }

    if ( m_pNuiSensor )
    {
        m_pNuiSensor->NuiShutdown( );
    }
    if ( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextSkeletonEvent );
        m_hNextSkeletonEvent = NULL;
    }

    if ( m_pNuiSensor )
    {
        m_pNuiSensor->Release();
        m_pNuiSensor = NULL;
    }


}

//-------------------------------------------------------------------
// Nui_ProcessThread
//
// Thread to handle Kinect processing
//-------------------------------------------------------------------
DWORD WINAPI Kinect::Nui_ProcessThread()
{
	const int numEvents = 2;
	HANDLE hEvents[numEvents] = { m_hEvNuiProcessStop, m_hNextSkeletonEvent };
	int    nEventIdx;


	// Main thread loop
	bool continueProcessing = true;
	while ( continueProcessing )
	{
		// Wait for any of the events to be signalled
		nEventIdx = WaitForMultipleObjects( numEvents, hEvents, FALSE, 100 );

		// Process signal events
		switch ( nEventIdx )
		{
		case WAIT_TIMEOUT:
			continue;

			// If the stop event, stop looping and exit
		case WAIT_OBJECT_0:
			continueProcessing = false;
			continue;

		case WAIT_OBJECT_0 + 1:
			updateKinectSkeleton( );
			break;
		}

	}

	return 0;
}
