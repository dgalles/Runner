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
#include "Achievements.h"
#include "Camera.h"

const float Player::SPEED_MULTIPLYER = 20;

 Player::Player(World *world, XInputManager *inputManager, Kinect *k, Achievements *ach) : mWorld(world), mXInputManager(inputManager), mKinect(k), mAchievements(ach)
//Player::Player(World *world, XInputManager *inputManager) : mWorld(world), mInputManager(inputManager)
{
    mKinectSensitivityLR = 1.0f;
    mKinectSensitivityFB = 1.0f;
    mInitialSpeed = 30;
    mAutoCallibrate = true;
	mEnableGamepad = false;
	mEnableKeyboard = false;
	mEnableKinect = true;
	mInvertControls = false;
	mTrackLookahead =100;
    setup();

	int mTotalCoins = 0;
	int mTotalMeters = 0;
	int mLongestRun = 0;
	int mMostCoins = 0;


	mInitialArmor = 3;

	mMaxSpeed = 80;

	mUseFrontBack = true;
	mLeanEqualsDuck = true;

}

 void Player::reset()
 {
     setup();
     mWorld->trackObject(this);

 }

void Player::setup()
{
    mPlayerObject = new RunnerObject(RunnerObject::PLAYER);
	mPlayerObject->loadModel("car.mesh", mWorld->SceneManager());
	mPlayerObject->setScale(Ogre::Vector3(5,6,10));

	mDistSinceMissedCoin = 0;
	mShielded = false;
	mBoosting = false;
	mBoostTime = 0;
	mShieldTime = 0;

	mBoostsHit = 0;
	mSHieldsHit = 0;

	mCurrentSpeed = mInitialSpeed;

	mPaused = true;
	mDistanceWithoutCoins = 0.0f;
	mMaxDistWithoutCoins = 0.0f;

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
	mAutoAccel = 0;
	mManualAccel = 5;
	mArmor = mInitialArmor;
	mWorld->getHUD()->setArmorLevel(mArmor);
}

Ogre::Vector3 
	Player::worldPosition() { return mPlayerObject->getPosition();}


void
    Player::startGame()
{
	mWorld->getHUD()->stopAllArrows();
	mWorld->getHUD()->showHUDElements(true);
	mAchievements->ResetActive();

    if (mAutoCallibrate)
    {
        mKinect->callibrate(4.0f, [this]() { 	mAchievements->DisplayActiveAchievements(3); this->setPaused(false); });
    }
    else
    {
		mAchievements->DisplayActiveAchievements(3);
        mPaused = false;
    }
}

void
    Player::stopArrows(int segment, float percent)
{
    for (int i = 0; i< mWorld->SawPowerup()->size(); i++)
    {
        ItemQueueData d = mWorld->SawPowerup()->atRelativeIndex(i);
        if (d.segmentIndex > segment)
            break;
        if(d.segmentIndex == segment && d.segmentPercent < percent && d.object->type() == RunnerObject::BLADE)
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

		if (mWorld->SawPowerup()->size() > 0)
		{
			for (int i = 0; i < mWorld->SawPowerup()->size(); i++)
			{
				ItemQueueData d = mWorld->SawPowerup()->atRelativeIndex(i);
				if (d.segmentIndex == newSegment && d.object->type() == RunnerObject::BLADE)
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
				d.object->setPosition(Ogre::Vector3(0,0,0));
                mCoinsCollected++;
				mTotalCoins++;
				mMaxDistWithoutCoins = std::max(mMaxDistWithoutCoins, mDistanceWithoutCoins);
				mDistanceWithoutCoins = 0.0f;
                mWorld->getHUD()->setCoins(mCoinsCollected);
			}
			if (d.object->getScale() != Ogre::Vector3::ZERO && d.segmentIndex == newSegment && d.segmentPercent < newPercent)
			{
				mDistSinceMissedCoin = 0;

			}
		}

	}
}

bool 
	Player::detectCollision(int newSegment, float newPercent, float newX)
{

	bool collide = false;
	if (mWorld->SawPowerup()->size() > 0)
	{
		for (int i = 0; i < mWorld->SawPowerup()->size(); i++)
		{
			ItemQueueData d = mWorld->SawPowerup()->atRelativeIndex(i);
			if (d.segmentIndex > newSegment)
			{
				break;
			}
            Ogre::Vector3 MTD;
            if (d.object->collides(mPlayerObject, MTD))
            {
				if (d.object->type() == RunnerObject::BLADE && !mShielded)
				{
					// TODO: Add armor here
					mAchievements->AchievementCleared("Buzzed");
					kill();
					break;
				}
				else if (d.object->type() == RunnerObject::SPEED)
				{
					d.object->setScale(Ogre::Vector3::ZERO);
					d.object->setPosition(Ogre::Vector3(0,0,0));

					mBoostsHit++;
					if (mBoostsHit == 1)
					{
						mAchievements->AchievementCleared("Booster");
					}
					if (mBoostsHit == 2)
					{
						mAchievements->AchievementCleared("Need for Speed");
					}
					if (mBoostsHit == 3)
					{
						mAchievements->AchievementCleared("Blazin'");
					}

					mPlayerObject->setAlpha(0.4f);
				//	mPlayerObject->setMaterial("Kinect/Blue");

					mBoosting = true;
					mShielded = true;
					mShieldTime = 5;
					mBoostTime = 3;
					mWorld->getCamera()->SetFollowType(RunnerCamera::CLOSE);
				}
				else if (d.object->type() == RunnerObject::SHEILD)
				{
					mShieldsHit++;
						if (mShieldsHit == 1)
					{
						mAchievements->AchievementCleared("Shielded");
					}
					if (mShieldsHit == 2)
					{
						mAchievements->AchievementCleared("Covered");
					}
					if (mShieldsHit == 3)
					{
						mAchievements->AchievementCleared("Invunerable");
					}
										mPlayerObject->setAlpha(0.4f);

					mShielded = true;
					mShieldTime = std::min(mShieldTime, 5.0f);;


				}



				}
			}
		}
	}
	return collide;
}



void Player::setLevel(int level)
{


//    mCurrentSpeed = 300 + level*200;
    // Also do rate change?

}


void Player::updateAnglesFromControls(Ogre::Degree &angle, Ogre::Degree &angle2)
{
	if (mEnableKinect)
	{
		angle =  mKinect->leftRightAngle() * mKinectSensitivityLR;
		angle2 = mKinect->frontBackAngle() * 0.8f *  mKinectSensitivityFB;
	}

	if (mEnableGamepad)
	{
		//short stickLeftRight = mXInputManager->state[0].Gamepad.sThumbLX;
		//short stickForwardBack = mXInputManager->state[0].Gamepad.sThumbLY;

		//angle = (Ogre::Real) stickLeftRight / 1000;
		//angle2 =  - (Ogre::Real)  stickForwardBack / 1000;
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


void Player::setLeanEqualsDuck(bool val)
{
	mLeanEqualsDuck = val;

}


void 
	Player::Think(float time)
{

	if (mPaused)
	{
		return;
	}



	if (mAlive)
	{

			if (mBoostTime  > 0 && mBoostTime <= time)
	{
		mBoostTime = 0;
		mBoosting = false;
		mWorld->getCamera()->SetFollowType(RunnerCamera::NORMAL);

		// mPlayerObject->restoreOriginalMaterial();

	}
	else
	{
		mBoostTime -= time;
	}
	if (mShieldTime > 0 && mShieldTime <= time)
	{
		mShieldTime = 0;
		mShielded = false;
		mPlayerObject->setAlpha(1);
	}
	else 
	{
		mShieldTime -= time;
	}


	Ogre::Degree angle = Ogre::Degree(0);

	Ogre::Degree angle2 = Ogre::Degree(0);

	updateAnglesFromControls(angle, angle2);



	int newSegment = mCurrentSegment;
	float newPercent = mSegmentPercent;

	mTimeSinceSpeedIncrease  += time;
	if (mTimeSinceSpeedIncrease > 1 && mLeanEqualsDuck)
	{
		mTimeSinceSpeedIncrease = 0;
		mCurrentSpeed += mAutoAccel;
		mCurrentSpeed = std::min(mCurrentSpeed, mMaxSpeed);
	}


	if (mLeanEqualsDuck)
	{
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
	}
	else
	{
		if (angle2 < Ogre::Degree(-10))
		{
			mCurrentSpeed += mManualAccel * time;
			mCurrentSpeed = std::min(mCurrentSpeed, mMaxSpeed);

			mWorld->getHUD()->setShowIncreaseSpeed(true);
			mWorld->getHUD()->setShowDecreaseSpeed(false);
		}
		else if (angle2 > Ogre::Degree(10))
		{
			mCurrentSpeed -= mManualAccel * time;
			mCurrentSpeed = std::max(mCurrentSpeed, 5.0f);
			mWorld->getHUD()->setShowIncreaseSpeed(false);
			mWorld->getHUD()->setShowDecreaseSpeed(true);
		}
		else
		{
			mWorld->getHUD()->setShowIncreaseSpeed(false);
			mWorld->getHUD()->setShowDecreaseSpeed(false);
		}

	}

	float lateralSpeed;
	if (mBoosting)
	{
		lateralSpeed = mCurrentSpeed *3;
	}
	else 
	{
		lateralSpeed = mCurrentSpeed / 2;
	}


		float distance = time * mCurrentSpeed * SPEED_MULTIPLYER;
		if (mBoosting)
		{
			distance *= 5;
		}
		mDistance += distance;
		mTotalMeters += distance;
        mWorld->getHUD()->setDistance((int) mDistance / 200);
		mWorld->getHUD()->setSpeed((int) mCurrentSpeed);
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
			mWorld->AddObjects(newSegment + 5);
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
				if (mTargetDeltaY < 0 &&  !mShielded)
				{
					kill();
					return;
				}
			}
			if ((mWorld->trackPath->kind(newSegment -1) == BezierPath::Kind::LOOP) && (mWorld->trackPath->kind(newSegment) != BezierPath::Kind::LOOP))
			{
				mAchievements->AchievementCleared("Looper");
			}
			if ((mWorld->trackPath->kind(newSegment -1) == BezierPath::Kind::TWIST) && (mWorld->trackPath->kind(newSegment) != BezierPath::Kind::TWIST))
			{
				mAchievements->AchievementCleared("Snap");
			}
			if (mWorld->trackPath->kind(newSegment) == BezierPath::Kind::GAP)
			{
				if (angle2.valueDegrees() >  5 || mShielded)
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

		if (newSegment > mWorld->trackPath->NumSegments() - mTrackLookahead)
		{
			mWorld->AddRandomSegment();
		}
		if (newSegment > mWorld->lastCoinAddedSegment() - 5)
		{
			mWorld->addCoins();
		}


		
		float distanceX = time * lateralSpeed * SPEED_MULTIPLYER;
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

		updateAchievements();

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

		if (angle2 > Ogre::Degree(0) && mLeanEqualsDuck)
		{
			mPlayerObject->pitch(Ogre::Radian(-angle2));
			mPlayerObject->setPosition(pos  + up *( - Ogre::Math::Sin(angle2) * mPlayerObject->minPointLocalScaled().z* 0.8f) + mDeltaY);
		}


		mPlayerObject->roll(Ogre::Radian(angle));

		// Collision with coins

		mDistanceWithoutCoins += distance;
		coinCollision(newSegment, newPercent, newX);

		mDistSinceMissedCoin+= distance;


		// Collision with walls & powerups (could kill player)

		bool collide = detectCollision(newSegment, newPercent, newX);


	}
	else if (mExplodeTimer > 0)
	{
		moveExplosion(time);
	}

}

void
Player::updateAchievements()
{
	if (mCoinsCollected >= 100)
	{
		mAchievements->AchievementCleared("Pennies From Heaven");
	}
	if (mCoinsCollected >= 200)
	{
		mAchievements->AchievementCleared("Making Money");
	}
	if (mCoinsCollected >= 500)
	{
		mAchievements->AchievementCleared("Getting Bank");
	}

	if (mDistance / 200 > 100)
	{
		mAchievements->AchievementCleared("Getting Started");
	}
		if (mDistance / 200 > 500)
	{
		mAchievements->AchievementCleared("Middle Distance");
	}

			if (mDistance / 200 > 1000)
	{
		mAchievements->AchievementCleared("Long Haul");
	}

	if (mTotalMeters / 200 >= 5000)
	{
		mAchievements->AchievementCleared("Marathon I");
	}
		if (mTotalMeters / 200 >= 10000)
	{
		mAchievements->AchievementCleared("Marathon II");
	}
	if (mTotalMeters / 200 >= 50000)
	{
		mAchievements->AchievementCleared("Marathon III");
	}
	if (mTotalCoins >=5000)
	{
		mAchievements->AchievementCleared("Building Bling");
	}
	if (mDistSinceMissedCoin / 200 > 20)
	{

		mAchievements->AchievementCleared("Greedy I");
	}
	if (mDistSinceMissedCoin / 200 > 50)
	{

		mAchievements->AchievementCleared("Greedy II");
	}	if (mDistSinceMissedCoin / 200 > 100)
	{

		mAchievements->AchievementCleared("Greedy III");
	}
	if (mDistanceWithoutCoins / 200 > 10)
	{
		mAchievements->AchievementCleared("Penniless I");
	}
	if (mDistanceWithoutCoins / 200 > 50)
	{
		mAchievements->AchievementCleared("Penniless II");
	}

	if (mDistanceWithoutCoins / 200 > 100)
	{
		mAchievements->AchievementCleared("Penniless III");
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
	mArmor--;
	mWorld->getHUD()->setArmorLevel(mArmor);

	if (mArmor <= 0)
	{
	mAlive = false;
	mExplodeTimer = 3.0;

	
	mLongestRun = std::max(mLongestRun, (int) mDistance);
	mMostCoins = std::max(mMostCoins, mCoinsCollected);

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
	 mWorld->getHUD()->setShowDecreaseSpeed(false);
	 mWorld->getHUD()->setShowIncreaseSpeed(false);
	}
	else
	{
		mShielded = true;
		mShieldTime = 2;
		mWorld->getCamera()->Shake();
	}
}