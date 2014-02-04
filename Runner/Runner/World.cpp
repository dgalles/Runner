#include "World.h"
#include "Ogre.h"
#include "OgreMath.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "ItemQueue.h"
#include "Bezier.h"
#include "Player.h"
#include "Camera.h"
#include <stdlib.h>



//Ogre::SceneManager *
//World::SM()
//{
//	return mSceneManager;
//}





World::World(Ogre::SceneManager *sceneManager, HUD *hud)   : mSceneManager(sceneManager), mTrackSceneNodes(), width(20.0f), mHUD(hud)
{
	// Global illumination for now.  Adding individual light sources will make you scene look more realistic
	mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));

	mLastCoinAddedSegment = -1;
	mMeshIDIndex = 0;
	mNumSegments = 3;
	mUnitsPerSegment = 10;
	mUnitsPerPathLength = 0.1f;
	mCoins = new ItemQueue(500);
	mWalls = new ItemQueue(100);

	trackPath = new BezierPath(Ogre::Vector3(0,0,0),
		Ogre::Vector3(50,0,0),
		Ogre::Vector3(100,0,0),
		Ogre::Vector3(150,0,0));

	int size = trackPath->NumSegments();

	for (int i = 0; i < size; i++)
	{
		addTrackNodes(i);
	}

//	AddTwisty();

	for (int i = 0; i < 10; i++)
	{
		AddRandomSegment();
		addCoins();
	}

	// Now that we have a scene node, we can move, scale, and rotate it any way we like
	//   (take a look at the header file for Ogre::SceneNode -- a good IDE is your friend, here
}

void 
	World::clearBarriersBefore(int segment)
{
	clearBefore(mWalls, segment);
}

void 
	World::clearBefore(ItemQueue *queue, int segment)
{
	if (queue->size() == 0)
		return;

	ItemQueueData d = queue->atRelativeIndex(0);
	while (d.segmentIndex < segment && queue->size() > 0)
	{
		Ogre::SceneNode *n = d.sceneNode;
		if (n != 0)
		{
			mSceneManager->getRootSceneNode()->removeChild(n);

			unsigned short index = 0;
			Ogre::MovableObject *mo = n->detachObject(index);
			Ogre::Entity *ent = static_cast<Ogre::Entity *>(mo);

			mSceneManager->destroyEntity(ent);
			mSceneManager->destroySceneNode(n);
		}

		queue->dequeue();
		d = queue->atRelativeIndex(0);
	}
}


void
	World::trackObject(TrackableObject *o)
{
	mCamera->TrackObject(o);

}

void 
	World::clearCoinsBefore(int segment)
{
	clearBefore(mCoins, segment);

}

void 
	World::getWorldPositionAndMatrix(int segment, float percentage, float relativeX, float relativeY, Ogre::Vector3 &worldPosition, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up)
{
	trackPath->getPointAndRotaionMatrix(segment, percentage, worldPosition, forward, right, up);
	worldPosition += right*relativeX;
	worldPosition += up*relativeY;
}



void 
World::removeWorldSegment(int index)
{
	


}


void
	World::RemovePathSegment(int start, int end)
{
	//for (std::vector<std::vector<Ogre::SceneNode*>>::iterator it = mSides.begin() + start; it != mSides.begin() + end; it++)
	//{
	//	for (std::vector<Ogre::SceneNode*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
	//	{
	//		Ogre::SceneNode *n = (*it2);
	//		mSceneManager->getRootSceneNode()->removeChild(n);
	//		mRecycledSides.push_back(n);


	//		//unsigned short index = 0;
	//		//Ogre::MovableObject *mo = n->detachObject(index);

	//		//Ogre::Entity *ent = static_cast<Ogre::Entity *>(mo);
	//		//
	//		//mSceneManager->destroyEntity(ent);
	//		//mSceneManager->destroySceneNode(*it2);
	//	}
	//}
	//for (std::vector<std::vector<Ogre::SceneNode*>>::iterator it = mBottoms.begin() + start; it != mBottoms.begin() + end; it++)
	//{
	//	for (std::vector<Ogre::SceneNode*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
	//	{
	//		Ogre::SceneNode *n = (*it2);
	//		mSceneManager->getRootSceneNode()->removeChild(n);
	//		mRecycledBottom.push_back(n);

	//		//unsigned short index = 0;
	//		//Ogre::MovableObject *mo = n->detachObject(index);

	//		//Ogre::Entity *ent = static_cast<Ogre::Entity *>(mo);
	//		//
	//		//mSceneManager->destroyEntity(ent);
	//		//mSceneManager->destroySceneNode(*it2);
	//	}
	//}
	//mSides.erase(mSides.begin()+start, mSides.begin()+end);
	//mBottoms.erase(mBottoms.begin()+start, mBottoms.begin()+end);
	//mLastCoinAddedSegment =- (start - end + 1);
	trackPath->removePathSegment(start, end);
}



void World::addCoins()
{
	addCoins(mLastCoinAddedSegment + 1);
}

void
	World::addCoins(int segmentToAdd)
{
	const int COINS_PER_SEGMENT = 4;


	float r =  rand() / (float) RAND_MAX;
	r = r * 3;
	r = (int) r;
	r = r -1;
	float relX =r * width;

	for (int i = 0; i < COINS_PER_SEGMENT; i++)
	{

		if (mCoins->size() < mCoins->maxSize())
		{

			Ogre::Entity *ent1 =SceneManager()->createEntity("coin.mesh");
			Ogre::SceneNode *coinNode = SceneManager()->getRootSceneNode()->createChildSceneNode();
			coinNode->attachObject(ent1);
			coinNode->scale(5,5,5);

			Ogre::Vector3 center;
			Ogre::Vector3 forward;
			Ogre::Vector3 pos;
			Ogre::Vector3 right;
			Ogre::Vector3 up;
			getWorldPositionAndMatrix(segmentToAdd,i* 1.0 / (float) COINS_PER_SEGMENT, relX, 10, pos,forward, right, up);
			Ogre::Quaternion q(-right,up,forward);

			coinNode->setPosition(pos);
			coinNode->setOrientation(q);
			coinNode->roll(Ogre::Degree(90));
			mCoins->enqueue(segmentToAdd,i* 1.0 / (float) COINS_PER_SEGMENT,relX, 10, coinNode);
		}
	}
	mLastCoinAddedSegment = std::max(segmentToAdd, mLastCoinAddedSegment);


}

void 
	World::addPoints(float percent, int segmentIndextToAdd, std::vector<Ogre::Vector3> &points, std::vector<Ogre::Vector3> &normals, Barrier barrier)

{
	Ogre::Vector3 centerPoint;
	Ogre::Vector3 right;
	Ogre::Vector3 up;
	Ogre::Vector3 forward;
	trackPath->getPointAndRotaionMatrix(segmentIndextToAdd,percent,centerPoint,forward,right, up);

	float deltaLeft = 30;
	float deltaRight  = 30;
	float deltaHeight = 10;
	if (barrier == BARRIER_LEFT)
	{
		deltaLeft = -10;
		deltaHeight = 15;
	}
	if (barrier == BARRIER_RIGHT)
	{
		deltaRight  = -10;
		deltaHeight = 15;
	}

	if (barrier == BARRIER_EDGES)
	{
		deltaRight  = 10;
		deltaLeft  = 10;
		deltaHeight = 15;
	}


	points.push_back(centerPoint - right*35 + up * deltaHeight);
	normals.push_back(up);
	points.push_back(centerPoint - right*deltaLeft + up * deltaHeight);
	normals.push_back(up);
	points.push_back(centerPoint - right*deltaLeft);
	normals.push_back(up);
	points.push_back(centerPoint + right*deltaRight);
	normals.push_back(up);
	points.push_back(centerPoint + right*deltaRight + up * deltaHeight);
	normals.push_back(up);
	points.push_back(centerPoint + right*35 + up * deltaHeight);
	normals.push_back(up);
	points.push_back(centerPoint + right*35 - up * 20);
	normals.push_back(-up);
	points.push_back(centerPoint - right*35 - up * 20);
	normals.push_back(-up);
}

void
	World::addTrackNodes(int segmentIndextToAdd, Barrier barrier, float barrierPercent)
{

	std::vector<Ogre::Vector3> points;
	std::vector<Ogre::Vector3> normals;

	if (segmentIndextToAdd > 0)
	{
		addPoints(0.95, segmentIndextToAdd-1, points, normals);
	}
	float deltaPercent = 20 / trackPath->pathLength(segmentIndextToAdd);

	for (int j = 0; j < trackPath->pathLength(segmentIndextToAdd) * mUnitsPerPathLength; j++)
	{
		float percent = ((float) j) / (trackPath->pathLength(segmentIndextToAdd) * mUnitsPerPathLength);

		if (percent >= barrierPercent && percent <= barrierPercent + deltaPercent)
		{
			addPoints(percent, segmentIndextToAdd, points, normals, barrier);
		}
		else
		{
			addPoints(percent, segmentIndextToAdd, points, normals, BARRIER_NONE);
		}
	}

	addPoints(1, segmentIndextToAdd, points, normals);




	Ogre::ManualObject *man = mSceneManager->createManualObject();
	man->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_TRIANGLE_LIST);
	for (int i =0; i < points.size(); i = i +1)
	{
		Ogre::Vector3 pos = points[i];
		man->position(pos);
		if (i % 8 == 2 || i % 8 == 3)
		{
			man->colour(0,0,1);
		}
		else if (i % 8 == 6 || i % 8 == 7)
		{
			man->colour(0,1,0);
		}
		else 
		{
			man->colour(1,0,0);
		}
		man->normal(normals[i]);
	}

	for (int i = 0; i < points.size()/8 - 1; i = i + 1)
	{
		man->triangle(i*8, i*8+1, i*8+8);
		man->triangle(i*8+1, i*8+9, i*8 + 8);
		man->triangle(i*8+1, i*8+10, i*8+9);
		man->triangle(i*8+2, i*8+10, i*8+1);
		man->triangle(i*8+2, i*8+11, i*8+10);
		man->triangle(i*8+3, i*8+11, i*8+2);
		man->triangle(i*8+3, i*8+4, i*8+11);
		man->triangle(i*8+11, i*8+4, i*8+12);
		man->triangle(i*8+4, i*8+13, i*8+12);
		man->triangle(i*8+4, i*8+5, i*8+13);
		man->triangle(i*8+5, i*8+6, i*8+14);
		man->triangle(i*8+5, i*8+14, i*8+13);
		man->triangle(i*8+15, i*8+14, i*8+7);
		man->triangle(i*8+14, i*8+6, i*8+7);
		man->triangle(i*8+7, i*8+0, i*8+8);
		man->triangle(i*8+7, i*8+8, i*8+15);
	}
	man->end();

	std::string meshStr = std::string("Mesh").append(std::to_string((long double) mMeshIDIndex));
	man->convertToMesh(meshStr.c_str());
	mMeshIDIndex++;

	Ogre::Entity *entity = mSceneManager->createEntity(meshStr.c_str());
	Ogre::SceneNode *node = mSceneManager->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);


}



void
	World::AddTwisty()
{
	Ogre::Vector3 point;
	Ogre::Vector3 direction;
	Ogre::Vector3 right;
	Ogre::Vector3 up;

	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);


	Ogre::Vector3 deltap1 = direction*400;

	Ogre::Vector3 n1 = up;
	Ogre::Quaternion q(Ogre::Degree(90),direction);

	//q = Ogre::Quaternion::IDENTITY;
	
	trackPath->AddPathSegment(deltap1 + point, point + 2 * deltap1, point + 3 * deltap1, q*n1, q*(q*n1), q*q*q*n1);


	addTrackNodes(trackPath->NumSegments() - 1);
}


void
	World::AddLoop()
{
	Ogre::Vector3 point;
	Ogre::Vector3 direction;
	Ogre::Vector3 right;
	Ogre::Vector3 up;


	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);


	Ogre::Quaternion q = direction.getRotationTo(direction + up);

	std::vector<Ogre::Vector3> directions(9);
	std::vector<Ogre::Vector3> normals(9);
	directions[0] = direction;
	normals[0] = up;  
	
	
	for (int i = 0; i < 8; i++)
	{
		directions[i+1] = q * directions[i];
		normals[i+1] = q * normals[i];
	}

	Ogre::Vector3 previous = point;

	for (int i = 0; i < 3; i++)
	{
		Ogre::Vector3 p1 =previous + directions[i*3] * 100 + right*10;
		Ogre::Vector3 p2 =p1 + directions[i*3 + 1] * 100 + right*10;
		Ogre::Vector3 p3 =p2 + directions[i*3 + 2] * 100 + right*10;
		trackPath->AddPathSegment(p1, p2, p3, normals[i*3], normals[i*3+1], normals[i*3+2]);
		addTrackNodes(trackPath->NumSegments() - 1);
		previous = p3;
	}
}





void
	World::AddSegment(Ogre::Vector3 deltap1, Ogre::Vector3 deltap2, Ogre::Vector3 deltap3, Barrier b, float barrierPercent)
{
	Ogre::Vector3 lastPoint = trackPath->getPoint(trackPath->NumSegments() - 1, 1);



	trackPath->AddPathSegment(deltap1 + lastPoint,
		deltap2  + lastPoint,
		deltap3 + lastPoint);



	bool barrier =  (rand() / (float) RAND_MAX > 0.3);
	addTrackNodes(trackPath->NumSegments() - 1,b, barrierPercent);
}


void World::AddBarrierSegment()
{
	Ogre::Vector3 point;
	Ogre::Vector3 direction;
	Ogre::Vector3 right;
	Ogre::Vector3 up;
	float barrierPercent = 0.9;

	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);

	direction = direction * 400;
	Ogre::Vector3 direction2 = direction*2;
	Ogre::Vector3 direction3 = direction*3;


	Barrier b = (Barrier) ((int) (((rand() - 1) / ((float) RAND_MAX))* 3)+1);


	AddSegment(direction,direction2, direction3, b, barrierPercent);
	int segment = trackPath->NumSegments() - 1;
	if (b == BARRIER_CENTER)
	{
		mWalls->enqueue(segment, barrierPercent, 0, 0, 0);	
	}
	else if (b == BARRIER_LEFT)
	{
		mWalls->enqueue(segment, barrierPercent, -1, 0, 0);	
		mWalls->enqueue(segment, barrierPercent, 0, 0, 0);	
	}
	else if (b == BARRIER_RIGHT)
	{
		mWalls->enqueue(segment, barrierPercent, 0, 0, 0);	
		mWalls->enqueue(segment, barrierPercent, 1, 0, 0);	

	}
	else if (b == BARRIER_EDGES)
	{
		mWalls->enqueue(segment, barrierPercent, -1, 0, 0);	
		mWalls->enqueue(segment, barrierPercent, 1, 0, 0);	
	}
}


void 
	World::AddRandomSegment()
{

	float r = (rand() / (float) RAND_MAX);

	if (r > 0.95)
	{
		AddTwisty();
	}
	else if (r > 0.90)
	{
		AddLoop();
	}
	else if (r > 0.35)
	{
		AddBarrierSegment();
	}
	else
	{
		Ogre::Vector3 point;
		Ogre::Vector3 direction;
		Ogre::Vector3 right;
		Ogre::Vector3 up;


		trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);


		Ogre::Vector3 direction2 = direction + (((rand() / (float) RAND_MAX) * 2  - 1) * right)
			+  (((rand() / (float) RAND_MAX) * 0.6  - 0.3) * up) ;

		direction2.normalise();

		Ogre::Vector3 direction3 = direction2 + (((rand() / (float) RAND_MAX)* 1  - 0.5) * right)
			+  (((rand() / (float) RAND_MAX) * 0.2  - 0.1) * up) ;

		direction = direction * 400;
		direction2 = direction + direction2 * 400;
		direction3 = direction2 + direction3 * 400;

		AddSegment(direction,direction2, direction3, BARRIER_NONE);
	}

}

void 
	World::Think(float time)
{
	// This is a pretty silly think method, but it gives you some ideas about how
	//  to proceed.  The single object will jiggle around randomly, and rotate

	//Vector3 velocity(Ogre::Math::RangeRandom(-30,30), Ogre::Math::RangeRandom(-30,30), Ogre::Math::RangeRandom(-30,30));
	//Vector3 currentPos = mExampleSceneNode->getPosition();
	// Vector3 newPos = currentPos + velocity * time;
	// mExampleSceneNode->setPosition(newPos);

	float radiansPerSecond = 1.0;

	for (int i = 0; i < mCoins->size(); i++)
	{
		ItemQueueData d = mCoins->atRelativeIndex(i);
		d.sceneNode->pitch(Ogre::Radian(radiansPerSecond * time));

	}
	//  mExampleSceneNode->yaw(Radian(time * radiansPerSecond));

	//for (std::vector<std::vector<Ogre::SceneNode*>>::iterator it = mTrackSceneNodes.begin(); it != mTrackSceneNodes.end(); it++)
	//{
	//	for (std::vector<Ogre::SceneNode*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
	//		(*it2)->yaw(Radian(time * radiansPerSecond));

	//}



}


