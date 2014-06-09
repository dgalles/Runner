#include "World.h"
#include "Player.h"
#include "Ogre.h"
#include "XInputManager.h"
#include "OgreSceneManager.h"
#include "Bezier.h"
#include "ItemQueue.h"
#include "Kinect.h"
#include "HUD.h"
#include "RunnerObject.h"
#include "InputHandler.h"

const float Player::SPEED_MULTIPLYER = 20;

 Player::Player(World *world, XInputManager *inputManager, Kinect *k) : mWorld(world), mXInputManager(inputManager), mKinect(k)
//Player::Player(World *world, XInputManager *inputManager) : mWorld(world), mInputManager(inputManager)
{
    mKinectSensitivityLR = 1.0f;
    mKinectSensitivityFB = 1.0f;
    FORWARD_SPEED = 30;
    mAutoCallibrate = true;
	mEnableGamepad = false;
	mEnableKeyboard = false;
	mEnableKinect = true;
	mInvertControls = false;
    setup();


}

 void Player::reset()
 {
     setup();
     mWorld->trackObject(this);

 }

void Player::setup()
{
    mPlayerObject = new RunnerObject();
    mPlayerObject->loadModel("car.mesh", mWorld->SceneManager());
    mPlayerObject->setScale(Ogre::Vector3(5,6,10));



	mCurrentSegment = 0;
	mSegmentPercent = 0.3f;
	mRelativeX = 0;
	mRelativeY = 5;
	mCoinsCollected = 0;
    mDistance = 0;
	mAlive = true;
	mTargetDeltaY = 0.0f;
	mDeltaY = 0.0f;

	Ogre::Vector3 pos;
	Ogre::Vector3 forward;
	Ogre::Vector3 right;
	Ogre::Vector3 up;
	mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,forward, right, up);
	Ogre::Quaternion q(-right,up,forward);

	mPlayerObject->setPosition(pos);
	mPlayerObject->setOrientation(q);

    float mTimeSinceSpeedIncrease = 0;
    mSpeedIncrease = 0;
}

Ogre::Vector3 
	Player::worldPosition() { return mPlayerObject->getPosition();}


void
    Player::startGame()
{
	mWorld->getHUD()->stopAllArrows();

    if (mAutoCallibrate)
    {
        mKinect->callibrate(4.0f, [this]() { this->setPaused(false); });
    }
    else
    {
        mPaused = false;
    }

}

void
    Player::stopArrows(int segment, float percent)
{
    for (int i = 0; i< mWorld->Saws()->size(); i++)
    {
        ItemQueueData d = mWorld->Saws()->atRelativeIndex(i);
        if (d.segmentIndex > segment)
            break;
        if(d.segmentIndex == segment && d.segmentPercent < percent)
        {
            mWorld->getHUD()->stopArrow(d.xtraData);
        }

    }
}

void
	Player::startArrows(int newSegment)
{
	if (mCurrentSegment != newSegment)
	{
		bool oneWall = false;

		if (mWorld->Saws()->size() > 0)
		{
			for (int i = 0; i < mWorld->Saws()->size(); i++)
			{
				ItemQueueData d = mWorld->Saws()->atRelativeIndex(i);
				if (d.segmentIndex == newSegment)
				{
                    mWorld->getHUD()->startArrow(d.xtraData);
				}
				else if (d.segmentIndex  > newSegment)
				{
					break;
				}


			}

		}

	}
}



void
	Player::coinCollision(int newSegment, float newPercent, float newX)
{
	if (mWorld->Coins()->size() > 0)
	{
		for (int i = 0; i < mWorld->Coins()->size(); i++)
		{
			ItemQueueData d = mWorld->Coins()->atRelativeIndex(i);
			if (d.segmentIndex > newSegment + 1)
			{
				break;
			}

            Ogre::Vector3 MTV;
            if (d.object->collides(mPlayerObject, MTV))
			{
                d.object->setScale(Ogre::Vector3::ZERO);
				d.object->translate(Ogre::Vector3(100,100,100));
                mCoinsCollected++;
                mWorld->getHUD()->setCoins(mCoinsCollected);
			}
		}

	}
}

bool 
	Player::detectCollision(int newSegment, float newPercent, float newX)
{

	bool collide = false;
	if (mWorld->Saws()->size() > 0)
	{
		for (int i = 0; i < mWorld->Saws()->size(); i++)
		{
			ItemQueueData d = mWorld->Saws()->atRelativeIndex(i);
			if (d.segmentIndex > newSegment)
			{
				break;
			}
            Ogre::Vector3 MTD;
            if (d.object->collides(mPlayerObject, MTD))
            {
                collide = true;
                break;
            }
		}
	}
	return collide;
}

void Player::setLevel(int level)
{


    FORWARD_SPEED = 300 + level*200;
    // Also do rate change?

}


void Player::updateAnglesFromConrols(Ogre::Degree &angle, Ogre::Degree &angle2)
{
	if (mEnableKinect)
	{
		angle =  mKinect->leftRightAngle() * mKinectSensitivityLR;
		angle2 = mKinect->frontBackAngle() * 0.8 *  mKinectSensitivityFB;
	}

	if (mEnableGamepad)
	{
		short stickLeftRight = mXInputManager->state[0].Gamepad.sThumbLX;
		short stickForwardBack = mXInputManager->state[0].Gamepad.sThumbLY;

		angle = (Ogre::Real) stickLeftRight / 1000;
		angle2 =  - (Ogre::Real)  stickForwardBack / 1000;
	}

	if (mEnableKeyboard)
		{
			if (InputHandler::getInstance()->IsKeyDown(OIS::KC_LEFT))
			{
				angle = -Ogre::Degree(30);
			} 
			else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_RIGHT))
			{
				angle = Ogre::Degree(30);

			}
			if (InputHandler::getInstance()->IsKeyDown(OIS::KC_UP))
			{
				angle2 = Ogre::Degree(-30);
			}
			else if (InputHandler::getInstance()->IsKeyDown(OIS::KC_DOWN))
			{
				angle2 = Ogre::Degree(30);
			}
			else
			{
				angle2 = Ogre::Degree(0);
			}
		}
		if (!mUseFrontBack)
		{
			angle2 = Ogre::Degree(0);
		}
		if (mInvertControls)
		{

			angle2 = -angle2;
		}

}


void 
	Player::Think(float time)
{

	if (mPaused)
	{
		return;
	}

	Ogre::Degree angle = Ogre::Degree(0);

	Ogre::Degree angle2 = Ogre::Degree(0);

	updateAnglesFromConrols(angle, angle2);



	int newSegment = mCurrentSegment;
	float newPercent = mSegmentPercent;

	mTimeSinceSpeedIncrease  += time;
	if (mTimeSinceSpeedIncrease > 1)
    {
        mTimeSinceSpeedIncrease = 0;
        FORWARD_SPEED += mSpeedIncrease;
    }

	LATERAL_SPEED = FORWARD_SPEED;

	if (mAlive)
	{


		float distance = time * FORWARD_SPEED * SPEED_MULTIPLYER;
		mDistance += distance;
        mWorld->getHUD()->setDistance((int) mDistance / 200);

		while (distance < 0 && -distance > mWorld->trackPath->pathLength(newSegment)*newPercent)
		{
			distance +=  mWorld->trackPath->pathLength(newSegment)*newPercent;
			newSegment--;
			newPercent = 1.0f;
		}


		while (distance > mWorld->trackPath->pathLength(newSegment) * (1 - newPercent))
		{
			distance -= mWorld->trackPath->pathLength(newSegment) * (1 - newPercent);
			newSegment++;
			mWorld->AddBlades(newSegment + 5);
			newPercent = 0.0f;
			if ((mWorld->trackPath->kind(newSegment + 2) == BezierPath::Kind::GAP) ||
				(mWorld->trackPath->kind(newSegment + 1) == BezierPath::Kind::GAP)  ||
				(mWorld->trackPath->kind(newSegment + 3) == BezierPath::Kind::GAP))
			{
				mWorld->getHUD()->startArrow(HUD::Kind::up);
			}
			if (mWorld->trackPath->kind(newSegment - 1) == BezierPath::Kind::GAP)
			{
				mWorld->getHUD()->stopArrow(HUD::Kind::up);
				if (mTargetDeltaY < 0)
				{
					kill();
					return;
				}
			}
			if (mWorld->trackPath->kind(newSegment) == BezierPath::Kind::GAP)
			{
				if (angle2.valueDegrees() >  5)
				{
					mTargetDeltaY = 0;
					mDeltaY = 0;
				}
				else
				{
					mTargetDeltaY = -10;
					mDeltaY = 0;
				}
			}

		}
		newPercent += distance / mWorld->trackPath->pathLength(newSegment);

		if (mTargetDeltaY == 0)
		{
			mDeltaY = 0;
		}
		else
		{
			mDeltaY =- time * 10;
			mDeltaY = std::max(mTargetDeltaY, mDeltaY);
		}

		if (newSegment > mWorld->trackPath->NumSegments() - 100)
		{
			mWorld->AddRandomSegment();
		}
		if (newSegment > mWorld->lastCoinAddedSegment() - 5)
		{
			mWorld->addCoins();
		}


		if (angle2 < Ogre::Degree(0))
		{
			float diff = (angle2.valueDegrees() / 10) + 6;
			diff = std::max(diff, 0.5f);
			mPlayerObject->setScale(Ogre::Vector3(5,diff,10));
		}
		else
		{
			mPlayerObject->setScale(Ogre::Vector3(5,6,10));
		}

		
		float distanceX = time * LATERAL_SPEED * SPEED_MULTIPLYER;
		float newX;


		if (angle < -Ogre::Degree(20))
		{
			newX = std::max<float>(mRelativeX - distanceX, -25);
		}
		else if (angle > Ogre::Degree(20))
		{
			newX = std::min<float>(mRelativeX + distanceX, 25);
		}
		else if (std::abs(mRelativeX) < distanceX)
		{
			newX = 0;
		}
		else if (mRelativeX > 0)
		{
			newX = mRelativeX - distanceX;
		}
		else
		{
			newX = mRelativeX + distanceX;
        }

        mWorld->clearCoinsBefore(mCurrentSegment-1);
        mWorld->clearBarriersBefore(mCurrentSegment-1);


		/// Arrow detection

		stopArrows(newSegment, newPercent);

		startArrows(newSegment);

		Ogre::Vector3 pos;
		Ogre::Vector3 forward;
		Ogre::Vector3 right;
		Ogre::Vector3 up;

		mSegmentPercent = newPercent;
		mCurrentSegment = newSegment;
		mRelativeX = newX;


		mRelativeY = - mPlayerObject->minPointLocalScaled().y + mDeltaY;

		mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,forward, right, up);
		Ogre::Quaternion q(-right,up,forward);

		mPlayerObject->setOrientation(q);
		mPlayerObject->setPosition(pos);

		if (angle2 > Ogre::Degree(0))
		{
			mPlayerObject->pitch(Ogre::Radian(-angle2));
			mPlayerObject->setPosition(pos  + up *( - Ogre::Math::Sin(angle2) * mPlayerObject->minPointLocalScaled().z* 0.8f) + mDeltaY);
		}


		mPlayerObject->roll(Ogre::Radian(angle));



		// Collision with walls

		bool collide = detectCollision(newSegment, newPercent, newX);

		// Collision with coins

		coinCollision(newSegment, newPercent, newX);


		if (collide)
		{
			kill();
		}
	}
	else if (mExplodeTimer > 0)
	{
		moveExplosion(time);
	}

}

void 
Player::moveExplosion(float time)
{
	mExplodeTimer -= time;
	debris[0]->translate(-mExplosionforward * time * 100);
	debris[0]->roll(Ogre::Degree(time * 600));
	debris[1]->translate(mExplosionright * time * 100);
	debris[1]->pitch(Ogre::Degree(time * 500));
	debris[2]->translate(-mExplosionright * time * 100);
		debris[2]->roll(Ogre::Degree(time * 300));
	debris[2]->pitch(Ogre::Degree(time * 200));

	debris[3]->translate(mExplosionup * time * 100);
	debris[3]->yaw(Ogre::Degree(time * 700));

}


void 
	Player::kill()
{
	mAlive = false;
	mExplodeTimer = 3.0;

	Ogre::Entity *debrisEntity[4];

	debrisEntity[0] = mWorld->SceneManager()->createEntity("car1.mesh");
	debrisEntity[1] = mWorld->SceneManager()->createEntity("car2.mesh");
	debrisEntity[2] = mWorld->SceneManager()->createEntity("car3.mesh");
	debrisEntity[3] = mWorld->SceneManager()->createEntity("car4.mesh");

	for (int i = 0; i < 4; i++)
	{
		debris[i] = mWorld->SceneManager()->getRootSceneNode()->createChildSceneNode();
		debris[i]->attachObject(debrisEntity[i]);
		debris[i]->scale(mPlayerObject->getScale());
	}

    delete mPlayerObject;


	Ogre::Vector3 pos;

	mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,mExplosionforward, mExplosionright, mExplosionup);
	Ogre::Quaternion q(-mExplosionright,mExplosionup,mExplosionforward);


	for (int i = 0; i < 4; i++)
	{
		debris[i]->setPosition(pos);
		debris[i]->setOrientation(q);
	}

	 mWorld->trackObject(NULL);
	 mWorld->getHUD()->stopAllArrows();
}