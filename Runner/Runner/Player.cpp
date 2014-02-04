#include "World.h"
#include "Player.h"
#include "Ogre.h"
#include "XInputManager.h"
#include "OgreSceneManager.h"
#include "Bezier.h"
#include "ItemQueue.h"
#include "Kinect.h"
#include "HUD.h"

Player::Player(World *world, XInputManager *inputManager, Kinect *k) : mWorld(world), mInputManager(inputManager), mKinect(k)
{
	Ogre::Entity *ent1 = mWorld->SceneManager()->createEntity("car.mesh");
	mPlayerSceneNode =mWorld->SceneManager()->getRootSceneNode()->createChildSceneNode();
	mPlayerSceneNode->attachObject(ent1);
	mPlayerSceneNode->scale(5,5,10);

	mCurrentSegment = 0;
	mSegmentPercent = 0.3;
	mRelativeX = 0;
	mRelativeY = 5;
	FORWARD_SPEED = 600.0f;
	LATERAL_SPEED = 600.0f;
	mCoinsCollected = 0;
	mAlive = true;

	Ogre::Vector3 pos;
	Ogre::Vector3 forward;
	Ogre::Vector3 right;
	Ogre::Vector3 up;
	mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,forward, right, up);
	Ogre::Quaternion q(-right,up,forward);

	mPlayerSceneNode->setPosition(pos);
	mPlayerSceneNode->setOrientation(q);

}

Ogre::Vector3 
	Player::worldPosition() { return mPlayerSceneNode->getPosition();}



void
	Player::startArrows(int newSegment)
{
	if (mCurrentSegment != newSegment)
	{
		bool oneWall = false;
		bool walls[HUD::last];
		for (int i = 0; i < HUD::last; i++)
		{
			walls[i] = false;
		}
		if (mWorld->Walls()->size() > 0)
		{
			for (int i = 0; i < mWorld->Walls()->size(); i++)
			{
				ItemQueueData d = mWorld->Walls()->atRelativeIndex(i);
				if (d.segmentIndex == newSegment)
				{
					oneWall = true;
					if ((d.relativeX) == 0)
					{
						walls[(int)HUD::center] = true;
					}
					else if (d.relativeX < 0)
					{
						walls[(int)HUD::left] = true;
					}
					else // d.relativeX > 0
					{
						walls[(int)HUD::right] = true;
					}

				}
				else if (d.segmentIndex  > newSegment)
				{
					break;
				}


			}

			if (oneWall)
			{
				for (int i = 0; i < HUD::last; i++)
				{
					if (!walls[i]) 
					{
						float time = mWorld->trackPath->pathLength(newSegment) / FORWARD_SPEED;
						mWorld->getHUD()-> startArrow(((HUD::Kind) i), time);
					}

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
			if (d.segmentIndex > newSegment || (d.segmentIndex == newSegment && d.segmentPercent > newPercent))
			{
				break;
			}

			if (d.segmentIndex == newSegment && std::abs(d.segmentPercent- newPercent) < 0.1)
			{

				if (std::abs((d.relativeX - newX)) < 10)
				{
					d.sceneNode->scale(0,0,0);
					d.relativeX = 500;
					mWorld->Coins()->setRelativeIndex(i, d);
					mCoinsCollected++;
					mWorld->getHUD()->setScore(mCoinsCollected);
				}
			}
		}

	}
}

bool 
	Player::detectCollision(int newSegment, float newPercent, float newX)
{

	bool collide = false;
	if (mWorld->Walls()->size() > 0)
	{
		for (int i = 0; i < mWorld->Walls()->size(); i++)
		{
			ItemQueueData d = mWorld->Walls()->atRelativeIndex(i);
			if (d.segmentIndex > newSegment || (d.segmentIndex == newSegment && d.segmentPercent > newPercent))
			{
				break;
			}

			if (d.segmentIndex == mCurrentSegment && d.segmentPercent >= mSegmentPercent)
			{
				if (d.relativeX == 0 && std::abs(newX) < 20)
				{
					collide = true;
				}
				if (d.relativeX == 1 && newX > 15)
				{
					collide = true;
				}

				if (d.relativeX == -1 &&  newX < -15)
				{
					collide = true;
				}
			}
		}
	}
	return collide;
}

void 
	Player::Think(float time)
{

	if (mPaused)
	{
		return;
	}
	LATERAL_SPEED = FORWARD_SPEED;
	int newSegment = mCurrentSegment;
	float newPercent = mSegmentPercent;

	if (mAlive)
	{


		float distance = time * FORWARD_SPEED;

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
			newPercent = 0.0f;
		}
		newPercent += distance / mWorld->trackPath->pathLength(newSegment);


		if (newSegment > mWorld->trackPath->NumSegments() - 500)
		{
			mWorld->AddRandomSegment();
		}
		if (newSegment > mWorld->lastCoinAddedSegment() - 10)
		{
			mWorld->addCoins();
		}

		short stickRight = mInputManager->state[0].Gamepad.sThumbRY;

		if (stickRight < -10000)
		{
			FORWARD_SPEED -= 5;

		}
		else if (stickRight > 10000)
		{
			FORWARD_SPEED += 5;
		}

		//	LATERAL_SPEED = FORWARD_SPEED / 3 * 4;

		Ogre::Degree angle = mKinect->leftRightAngle();


		short stickLeft = mInputManager->state[0].Gamepad.sThumbLX;
		float distanceX = time * LATERAL_SPEED;
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




		for (int i = mCurrentSegment -1; i < newSegment - 1; i++)
		{
			mWorld->clearCoinsBefore(mCurrentSegment-1);
			mWorld->clearBarriersBefore(mCurrentSegment-1);
			//mWorld->removeWorldSegments();
			// TODO:  Also clear track here!
		}


		Ogre::Vector3 pos;
		Ogre::Vector3 forward;
		Ogre::Vector3 right;
		Ogre::Vector3 up;

		/// Arrow detection

		startArrows(newSegment);


		// Collision with walls


		bool collide = detectCollision(newSegment, newPercent, newX);

		// Collision with coins


		coinCollision(newSegment, newPercent, newX);



		if (!collide)
		{
			mSegmentPercent = newPercent;
			mCurrentSegment = newSegment;
			mRelativeX = newX;


			mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,forward, right, up);
			Ogre::Quaternion q(-right,up,forward);

			mPlayerSceneNode->setPosition(pos);
			mPlayerSceneNode->setOrientation(q);

			mPlayerSceneNode->roll(Ogre::Radian(angle));

		}
		else
		{
			mAlive = false;
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
	debris[1]->translate(mExplosionright * time * 100);
	debris[2]->translate(-mExplosionright * time * 100);
	debris[3]->translate(mExplosionup * time * 100);

}


void 
	Player::kill()
{
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
		debris[i]->scale(5,5,10);
	}

	mWorld->SceneManager()->getRootSceneNode()->removeChild(mPlayerSceneNode);

	unsigned short index = 0;
	Ogre::MovableObject *mo = mPlayerSceneNode->detachObject(index);
	Ogre::Entity *ent = static_cast<Ogre::Entity *>(mo);

	mWorld->SceneManager()->destroyEntity(ent);
	mWorld->SceneManager()->destroySceneNode(mPlayerSceneNode);


	Ogre::Vector3 pos;

	mWorld->getWorldPositionAndMatrix(mCurrentSegment, mSegmentPercent, mRelativeX, mRelativeY, pos,mExplosionforward, mExplosionright, mExplosionup);
	Ogre::Quaternion q(-mExplosionright,mExplosionup,mExplosionforward);


	for (int i = 0; i < 4; i++)
	{
		debris[i]->setPosition(pos);
		debris[i]->setOrientation(q);
	}

	mWorld->trackObject(NULL);

}