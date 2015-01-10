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
#include "Sound.h"
#include "Ghost.h"
#include "Menu.h"
const float Player::SPEED_MULTIPLYER = 20;



void Player::resetToDefaults()
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

	mTotalCoins = 0;
	mTotalMeters = 0;
	mLongestRun = 0;
	mMostCoins = 0;
	mLifetimeCoins = 0;

	mShieldDuration = 1;
	mBoostDuration = 1;
	mMagnetDuration = 1;

	mInitialArmor = 1;

	mMaxSpeed = 80;

	mAutoAccel = 0;
	mUseFrontBack = true;
	mLeanEqualsDuck = true;
	mWarningDelta = 1;
	mManualAccel = 5;

	mRaceGoal = 20;
	mRaceType = TOTALCOINS;
	mRacing = false;

}


Player::Player(World *world, XInputManager *inputManager, Kinect *k, Achievements *ach, bool isSecond) : 
	mWorld(world), mXInputManager(inputManager), mKinect(k), mAchievements(ach),mIsSecondPlayer(isSecond)
	//Player::Player(World *world, XInputManager *inputManager) : mWorld(world), mInputManager(inputManager)
{
	mGhost = NULL;
	// TODO:  Place this in a data file!

	mBoostDurationValues = new float[6];
	mBoostDurationValues[0] = 0;
	mBoostDurationValues[1] = 2;
	mBoostDurationValues[2] = 2.3f;
	mBoostDurationValues[3] = 2.7f;
	mBoostDurationValues[4] = 3.0f;
	mBoostDurationValues[5] = 3.3f;

	mShieldDurationValues = new float[6];
	mShieldDurationValues[0] = 0;
	mShieldDurationValues[1] = 3;
	mShieldDurationValues[2] = 4;
	mShieldDurationValues[3] = 5;
	mShieldDurationValues[4] = 6;
	mShieldDurationValues[5] = 7;

	mMagnetDurationValues = new float[6];
	mMagnetDurationValues[0] = 0;
	mMagnetDurationValues[1] = 3;
	mMagnetDurationValues[2] = 4;
	mMagnetDurationValues[3] = 5;
	mMagnetDurationValues[4] = 6;
	mMagnetDurationValues[5] = 7;
	resetToDefaults();
	setup();
}

void Player::reset(Ghost::GhostInfo *ghostInfo /* = NULL */)
{
	setup(ghostInfo);
	mWorld->trackObject(this);

}

void Player::setup(Ghost::GhostInfo *ghostInfo /* = NULL */)
{
	mPlayerObject = new RunnerObject(RunnerObject::PLAYER);
	mPlayerObject->loadModel("car.mesh", mWorld->SceneManager());
	mPlayerObject->setScale(Ogre::Vector3(5,6,10));

	mDistSinceMissedCoin = 0;
	mShielded = false;
	mBoosting = false;
	mBoostTime = 0;
	mShieldTime = 0;

	mMagnetsHit = 0;
	mMagnetTime = 0;
	mMagnetActive = false;

	mTimeSinceLastLog = 0;

	mBoostsHit = 0;
	mShieldsHit = 0;


	if (ghostInfo == NULL)
	{
		mCurrInitialSpeed = mInitialSpeed;
		mCurrInitialArmor = mInitialArmor;
		mCurrManualAccel = mManualAccel;
		mCurrAutoAccel = mAutoAccel;
		mCurrMaxSpeed = mMaxSpeed;
		mCurrMagnetDuration = mMagnetDuration;
		mCurrShieldDuration = mShieldDuration;
		mCurrBoostDuration = mBoostDuration;
		mCurrLeanEqualsDuck = mLeanEqualsDuck;
		mCurrRaceGoal = mRaceGoal;
		mCurrRacing = mRacing;
		mCurrRaceType = mRaceType;
	}
	else
	{
		mCurrInitialSpeed = ghostInfo->mInitialSpeed;
		mCurrInitialArmor = ghostInfo->mInitialArmor;
		mCurrManualAccel = ghostInfo->mManualAceelerateRate;
		mCurrAutoAccel = ghostInfo->mAutoAceelerateRate;
		mCurrMaxSpeed = ghostInfo->mMaxSpeed;
		mCurrMagnetDuration = ghostInfo->mMagnetDuration;
		mCurrShieldDuration = ghostInfo->mShieldDuration;
		mCurrBoostDuration = ghostInfo->mBoostDuration;
		mCurrLeanEqualsDuck = ghostInfo->mLeanEqualsDuck;
		mCurrRaceGoal = ghostInfo->mRaceGoal;
		mCurrRacing = ghostInfo->mRacing;
		mCurrRaceType = (Player::RaceType) ghostInfo->mRaceType;

	}


	mWonRace = false;
	mCurrentSpeed = mCurrInitialSpeed;
	mArmor = mCurrInitialArmor;

	mPaused = true;
	mDistanceWithoutCoins = 0.0f;
	mMaxDistWithoutCoins = 0.0f;

	mLeftCoinsCollected = 0;
	mRightCoinsCollected = 0;
	mMiddleCoinsCollected = 0;
	mConsecutiveCoins = 0;

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
	mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,forward, right, up, mIsSecondPlayer);
	Ogre::Quaternion q(-right,up,forward);

	mPlayerObject->setPosition(pos);
	mPlayerObject->setOrientation(q);

	float mTimeSinceSpeedIncrease = 0;
	mWorld->getHUD()->setArmorLevel(mArmor);
	mWorld->getHUD()->setCoins(0);
	mWorld->getHUD()->setDistance(0);
	mTime = 0;
	if (mRacing)
	{
		Ogre::String message = "Goal = ";
		message.append(std::to_string(mRaceGoal));
		if (mRaceType == RaceType::CONCECUTIVECOINS)
		{
			message.append(" Conscecutive Coins");
		}
		else
		{
			message.append(" Totals Coins");
		}
		mWorld->getHUD()->setRacingMesssage(message);
		mWorld->getHUD()->showRacingOverlay(true);
	}
	else
	{
		mWorld->getHUD()->showRacingOverlay(false);
	}

}

Ogre::Vector3 
	Player::worldPosition() { return mPlayerObject->getPosition();}



void Player::setGhostInfo(Ghost::GhostInfo *ghostInfo)
{
	ghostInfo->mInitialSpeed = 	mCurrInitialSpeed;
	ghostInfo->mInitialArmor = mCurrInitialArmor;
	ghostInfo->mManualAceelerateRate = mCurrManualAccel;
	ghostInfo->mAutoAceelerateRate = mCurrAutoAccel;
	ghostInfo->mMaxSpeed = mCurrMaxSpeed;
	ghostInfo->mMagnetDuration = mCurrMagnetDuration;
	ghostInfo->mShieldDuration = mCurrShieldDuration;
	ghostInfo->mBoostDuration = mCurrBoostDuration;
	ghostInfo->mLeanEqualsDuck = mCurrLeanEqualsDuck;
}


void
	Player::startGame()
{
	mWaitingOnKey = false;
	mWorld->getHUD()->stopAllArrows();
	mWorld->getHUD()->showHUDElements(true);
	mWorld->getHUD()->showRaceOver(false);

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
				if (d.segmentIndex <= newSegment + mWarningDelta  && d.segmentIndex >= newSegment &&  d.object->type() == RunnerObject::BLADE)
				{
					mWorld->getHUD()->startArrow(d.xtraData);
				}
				else if (d.segmentIndex  > newSegment + mWarningDelta * 2)
				{
					break;
				}


			}

		}

	}
}



void
	Player::coinCollision(int newSegment, float newPercent, float newX, float time)
{
	if (mWorld->Coins()->size() > 0)
	{
		if (mMagnetActive)
		{
			float MAGNET_DISTANCE_SQUARED = 800 * 800;
			Ogre::Vector3 playerPos = mPlayerObject->getPosition();
			for (int i = 0; i < mWorld->Coins()->size(); i++)
			{
				ItemQueueData d = mWorld->Coins()->atRelativeIndex(i);
				Ogre::Vector3 coinPos = d.object->getPosition();
				if (d.segmentIndex > newSegment + 8)
				{
					break;
				}
				if (d.object->getScale().x > 0)
				{
					float squaredDist = coinPos.squaredDistance(playerPos) ;
					if (squaredDist < MAGNET_DISTANCE_SQUARED)
					{
						float dist = time * mCurrentSpeed * 20;
						if (mBoosting)
						{
							dist *= 5;
						}
						if (dist * dist > squaredDist)
						{
							d.object->setPosition(playerPos);
						}
						else
						{
							Ogre::Vector3 delta = (playerPos - coinPos);
							delta.normalise();
							d.object->setPosition(coinPos + delta * dist);
						}
					}
				}

			}




		}

		bool missedCoin = false;
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
				if (d.relativeX < 0)
				{
					mLeftCoinsCollected++;
				}
				else if (d.relativeX > 0)
				{
					mRightCoinsCollected++;
				}
				else
				{
					mMiddleCoinsCollected++;
				}
				d.object->setScale(Ogre::Vector3::ZERO);
				d.object->setPosition(Ogre::Vector3(0,0,0));
				mCoinsCollected++;
				mConsecutiveCoins++;
				mTotalCoins++;
				mLifetimeCoins++;
				SoundBank::getInstance()->play("coin");
				mMaxDistWithoutCoins = std::max(mMaxDistWithoutCoins, mDistanceWithoutCoins);
				mDistanceWithoutCoins = 0.0f;
				if (mRacing && mRaceType == RaceType::CONCECUTIVECOINS)
				{
					mWorld->getHUD()->setCoins(mConsecutiveCoins,false,true);
					if (mConsecutiveCoins >= mRaceGoal)
					{
						mWonRace = true;
					}
				}
				else if(mRacing && mRaceType == RaceType::TOTALCOINS)
				{
					mWorld->getHUD()->setCoins(mCoinsCollected,false,false);
					if (mCoinsCollected >= mRaceGoal)
					{
						mWonRace = true;
					}

				}
				else
				{
					mWorld->getHUD()->setCoins(mCoinsCollected);
				}
			}
			if (d.segmentIndex == newSegment && d.segmentPercent < newPercent )
			{
				missedCoin = (d.object->getScale() != Ogre::Vector3::ZERO);	
			}
		}
		if (missedCoin)
		{
			mDistSinceMissedCoin = 0;
			mConsecutiveCoins = 0;
			if (mRacing && mRaceType == RaceType::CONCECUTIVECOINS)
			{
				mWorld->getHUD()->setCoins(mConsecutiveCoins,false,true);
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
					mShieldTime = mBoostDurationValues[mBoostDuration] + 2;
					mBoostTime = mBoostDurationValues[mBoostDuration];
					mWorld->getCamera()->SetFollowType(RunnerCamera::CLOSE);
				}
				else if (d.object->type() == RunnerObject::SHEILD)
				{
					d.object->setScale(Ogre::Vector3::ZERO);
					d.object->setPosition(Ogre::Vector3(0,0,0));
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
					mShieldTime = std::max(mShieldTime, mShieldDurationValues[mShieldDuration]);;
				}
				else if (d.object->type() == RunnerObject::MAGNET)
				{
					d.object->setScale(Ogre::Vector3::ZERO);
					d.object->setPosition(Ogre::Vector3(0,0,0));
					if (!mMagnetActive)
					{
						mMagnetActive = true;
						Ogre::SceneManager * sm= mWorld->SceneManager();
						Ogre::Entity *e = sm->createEntity("Magnet.mesh");

						mMagnetNode = mPlayerObject->getSceneNode()->createChildSceneNode();
						mMagnetNode->attachObject(e);

						mMagnetNode->setPosition(0,3,0);
					}
					mMagnetTime = mMagnetDurationValues[mMagnetDuration];

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
		if (InputHandler::getInstance()->KeyPressedThisFrame(OIS::KC_Q))
		{

			mPlayerObject->setMaterial("simpleOrange");
		}

		if (InputHandler::getInstance()->KeyPressedThisFrame(OIS::KC_R))
		{
			mWorld->getCamera()->setReview(!mWorld->getCamera()->getReview());
		} 


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


void Player::SendData(float time)
{
	mTimeSinceLastLog += time;
	if(mTimeSinceLastLog >= 1.0)
	{

		float speed = mCurrentSpeed;
		if (mBoosting)
		{
			speed *= 5;
		}
		for (std::vector<PlyrDataMsgr *>::iterator it = mLogger.begin(); it != mLogger.end(); it++)
		{
			(*it)->ReceivePlyrData(new PlyrData("", mLeftCoinsCollected,mRightCoinsCollected,mMiddleCoinsCollected,
				mWorld->getCoinsMissedLeft(), mWorld->getCoinsMissedRight(), mWorld->getCoinsMissedMiddle(), speed));
		} 
		mTimeSinceLastLog = 0.0;
	}

}

void 
	Player::Think(float time)
{

	if (mPaused)
	{
		return;
	}

	// Send data to our logger
	SendData(time);


	if (mAlive)
	{
		// Deal with timed powerups
		//  (Boost, Shield, Magnet)

		if (mBoostTime  > 0 && mBoostTime <= time)
		{
			mBoostTime = 0;
			mBoosting = false;
			mWorld->getCamera()->SetFollowType(RunnerCamera::NORMAL);
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
		if (mMagnetTime  > 0 && mMagnetTime <= time)
		{
			mMagnetTime = 0;
			mMagnetActive = false;
			mPlayerObject->getSceneNode()->removeChild(mMagnetNode);
		}
		else
		{
			mMagnetTime -= time;
		}



		// Get new angles from the controler(s)

		Ogre::Degree angle = Ogre::Degree(0);
		Ogre::Degree angle2 = Ogre::Degree(0);
		updateAnglesFromControls(angle, angle2);



		int newSegment = mCurrentSegment;
		float newPercent = mSegmentPercent;

		mTimeSinceSpeedIncrease  += time;
		if (mTimeSinceSpeedIncrease > 1 && mCurrLeanEqualsDuck)
		{
			mTimeSinceSpeedIncrease = 0;
			mCurrentSpeed += mCurrAutoAccel;
			mCurrentSpeed = std::min(mCurrentSpeed, mCurrMaxSpeed);
		}


		if (mCurrLeanEqualsDuck)
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
				mCurrentSpeed += mCurrManualAccel * time;
				mCurrentSpeed = std::min(mCurrentSpeed, mCurrMaxSpeed);

				mWorld->getHUD()->setShowIncreaseSpeed(true);
				mWorld->getHUD()->setShowDecreaseSpeed(false);
			}
			else if (angle2 > Ogre::Degree(10))
			{
				mCurrentSpeed -= mCurrManualAccel * time;
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
		{
			int speed = (int) mCurrentSpeed;
			if (mBoosting)
			{
				speed *= 5;
			}
			mWorld->getHUD()->setSpeed(speed);
		}

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
			mWorld->AddObjects(newSegment + 5, mIsSecondPlayer);
			newPercent = 0.0f;
			bool flagUp = false;
			for (int i = 0; i < mWarningDelta + 2; i++)
			{
				if (mWorld->trackPath->kind(newSegment + 1 + i) == BezierPath::Kind::GAP)
				{
					flagUp = true;
					break;
				}
			}
			if (flagUp)
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
		if (newSegment > mWorld->lastCoinAddedSegment(mIsSecondPlayer) - 5)
		{
			mWorld->addCoins(mIsSecondPlayer);
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

		mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,forward, right, up, mIsSecondPlayer);
		Ogre::Quaternion q(-right,up,forward);

		mPlayerObject->setOrientation(q);
		mPlayerObject->setPosition(pos);

		if (angle2 > Ogre::Degree(0) && mCurrLeanEqualsDuck)
		{
			mPlayerObject->pitch(Ogre::Radian(-angle2));
			mPlayerObject->setPosition(pos  + up *( - Ogre::Math::Sin(angle2) * mPlayerObject->minPointLocalScaled().z* 0.8f) + mDeltaY);
		}

		mPlayerObject->roll(Ogre::Radian(angle));

		mTime += time;

		mWorld->getHUD()->setTime(mTime);
		if (mGhost != NULL)
		{
			int speed = (int) mCurrentSpeed;
			if (mBoosting)
			{
				speed *= 5;
			}
			mGhost->record(mTime, mCurrentSegment, 	mSegmentPercent, mRelativeX, mRelativeY, angle, angle2, mCoinsCollected, (int) mDistance / 200, (int) mCurrentSpeed);			
		}

		// Collision with coins

		mDistanceWithoutCoins += distance;
		coinCollision(newSegment, newPercent, newX, time);

		mDistSinceMissedCoin+= distance;


		// Collision with walls & powerups (could kill player)

		bool collide = detectCollision(newSegment, newPercent, newX);

		if (mRacing && mWonRace)
		{
			finishRace();
		}

	}
	handleTimers(time);


}

void
	Player::handleTimers(float time)
{
	if (!mAlive &&  (mExplodeTimer > 0))
	{
		moveExplosion(time);
	}
    if (mWaitingOnKey)
	{
		if (InputHandler::getInstance()->IsKeyDown(OIS::KC_SPACE))
		{
			MenuManager::getInstance()->getMenu("gameOver")->enable();
			mWorld->getHUD()->showRaceOver(false);
		}
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
	if (mExplodeTimer <= 0 && mExplodeTimer > -100)
	{
		mExplodeTimer = -200;
		MenuManager::getInstance()->getMenu("gameOver")->enable();
	}
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


void Player::finishRace()
{
	mAlive = false;
	mLongestRun = std::max(mLongestRun, (int) mDistance);
	mMostCoins = std::max(mMostCoins, mCoinsCollected);
	mWorld->getHUD()->stopAllArrows();
	mWorld->getHUD()->setShowDecreaseSpeed(false);
	mWorld->getHUD()->setShowIncreaseSpeed(false);
	char buf[50];
	sprintf(buf, "Time = %.2f", mTime);
	mWorld->endGame();
	mWorld->getHUD()->setFinalRaceTime(Ogre::String(buf), false);
	mWorld->getHUD()->showRaceOver(true);
	if (mGhost != NULL)
	{
		mGhost->playerDead((int) mDistance / 200, mCoinsCollected);
		mGhost->stopRecording(true);
	}
	mWaitingOnKey = true;
//	MenuManager::getInstance()->getMenu("gameOver")->enable();

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

		SoundBank::getInstance()->play("crash");

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

		mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,mExplosionforward, mExplosionright, mExplosionup, mIsSecondPlayer);
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
		if (mGhost != NULL)
		{
			mGhost->playerDead((int) mDistance / 200, mCoinsCollected);
			mGhost->stopRecording();
		}
		mWorld->endGame();

	}
	else
	{
		mShielded = true;
		mShieldTime = 2;
		mWorld->getCamera()->Shake();
	}
}