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
#include "RunnerObject.h"
#include "Runner.h"


//Ogre::SceneManager *
//World::SM()
//{
//	return mSceneManager;
//}



void World::resetToDefaults()
{
	mDrawTrack = true;
	mObsFreq = 0.15f;
	mUseFrontBack = true;
	mObsGap = 3;
	mSimpleMaterials = false;
}

World::World(Ogre::SceneManager *sceneManager, HUD *hud, Runner *base, bool useMirror)   : mSceneManager(sceneManager), mTrackSceneNodes(), 
	width(20.0f), mHUD(hud), mBase(base), mUseMirror(useMirror)
{
	mMeshIDIndex = 0;
	resetToDefaults();
	setup();
}

void World::setup()
{
	// Global illumination for now.  Adding individual light sources will make you scene look more realistic
	mSceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));
	mTrackSceneNodes.clear();

	mLastObjSeg = 0;


	mLastCoinAddedSegment[0] = 0;
	mLastCoinAddedSegment[1] = 0;
	mUnitsPerSegment = 10;
	mUnitsPerPathLength = 0.1f;
	mCoins = new ItemQueue<ItemQueueData>(500);
	mSawPowerup = new ItemQueue<ItemQueueData>(100);

	trackPath = new BezierPath(Ogre::Vector3(0,0,0),
		Ogre::Vector3(50,0,0),
		Ogre::Vector3(100,0,0),
		Ogre::Vector3(150,0,0));

	int size = trackPath->NumSegments();

	for (int i = 0; i < size; i++)
	{
		addTrackNodes(i);
	}
	for (int i = 0; i < 3; i++)
	{
		AddNormalSegment();
	}
	for (int i = 0; i < 5; i++)
	{
	
			AddRandomSegment();

		addCoins(false);
		if (mUseMirror)
		{
		addCoins(true);

		}
	}
	AddObjects(3, false);
	AddObjects(4, false);
	AddObjects(5, false);

	if (mUseMirror)
	{
		AddObjects(3, true);
		AddObjects(4, true);
		AddObjects(5, true);

	}
	//AddJump();
	//AddJump();
	//AddJump();
	//AddJump();

}


void World::reset()
{
	delete mSawPowerup;
	delete mCoins;
		mSceneManager->clearScene();
		delete trackPath;
	mSceneManager->setSkyBox(true, "Skybox/Cloudy");

	setup();
}


void 
	World::clearBarriersBefore(int segment)
{
	clearBefore(mSawPowerup, segment);
}

void 
	World::clearBefore(ItemQueue<ItemQueueData> *queue, int segment)
{
	if (queue->size() == 0)
		return;

	ItemQueueData d = queue->atRelativeIndex(0);
	while (d.segmentIndex < segment && queue->size() > 0)
	{
		RunnerObject *runnerObj = d.object;

		delete runnerObj;
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
	World::getWorldPositionAndMatrix(int segment, float percentage, float relativeX, float relativeY, Ogre::Vector3 &worldPosition, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up, bool isSecondPlayer)
{
	trackPath->getPointAndRotaionMatrix(segment, percentage, worldPosition, forward, right, up, isSecondPlayer);
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



void World::addCoins(bool player)
{
	addCoins(mLastCoinAddedSegment[(int) player] + 1, player);
}

void
	World::addCoins(int segmentToAdd, bool player)
{
	const int COINS_PER_SEGMENT = 4;


	float r =  rand() / (float) RAND_MAX;
	r = r * 3;
	r = (float) ((int) r);
	r = r -1;
	float relX =r * width;

	for (int i = 0; i < COINS_PER_SEGMENT; i++)
	{

		if (mCoins->size() < mCoins->maxSize())
		{

			RunnerObject *coin = new RunnerObject(RunnerObject::COIN);
			coin->loadModel("coin.mesh", SceneManager());
			coin->setScale(Ogre::Vector3(5,5,5));

			Ogre::Vector3 center;
			Ogre::Vector3 forward;
			Ogre::Vector3 pos;
			Ogre::Vector3 right;
			Ogre::Vector3 up;
			getWorldPositionAndMatrix(segmentToAdd,i* 1.0f / (float) COINS_PER_SEGMENT, relX, 10, pos,forward, right, up, player);
			Ogre::Quaternion q(-right,up,forward);

			coin->setPosition(pos);
			coin->setOrientation(q);
			coin->roll(Ogre::Degree(90));
			mCoins->enqueue(ItemQueueData(segmentToAdd,i* 1.0f / (float) COINS_PER_SEGMENT,relX, 10, coin));
		}
	}
	mLastCoinAddedSegment[(int) player] = std::max(segmentToAdd, mLastCoinAddedSegment[(int) player]);


}

void 
	World::addPoints(float percent, int segmentIndextToAdd, std::vector<Ogre::Vector3> &points, std::vector<Ogre::Vector3> &normals, bool mirror)

{
	Ogre::Vector3 centerPoint;
	Ogre::Vector3 right;
	Ogre::Vector3 up;
	Ogre::Vector3 forward;
	trackPath->getPointAndRotaionMatrix(segmentIndextToAdd,percent,centerPoint,forward,right, up, mirror);

	float deltaLeft = 30;
	float deltaRight  = 30;
	float deltaHeight = 10;


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
	World::addTrackNodes(int segmentIndextToAdd, bool startCap, bool endcap)
{

	addTrackNodesOne(segmentIndextToAdd, false, startCap, endcap);
	if (mUseMirror)
	{
		addTrackNodesOne(segmentIndextToAdd, true, startCap, endcap);

	}

}


void
	World::addTrackNodesOne(int segmentIndextToAdd, bool mirror, bool startCap, bool endcap)
{
	if (!mDrawTrack)
		return;

	std::vector<Ogre::Vector3> points;
	std::vector<Ogre::Vector3> normals;


	// Add a little before this segment, so we don't see any gaps ..
	if (segmentIndextToAdd > 0)
	{
		addPoints(0.95f, segmentIndextToAdd-1, points, normals, mirror);
	}
	float deltaPercent = 20 / trackPath->pathLength(segmentIndextToAdd);

	for (int j = 0; j < trackPath->pathLength(segmentIndextToAdd) * mUnitsPerPathLength; j++)
	{
		float percent = ((float) j) / (trackPath->pathLength(segmentIndextToAdd) * mUnitsPerPathLength);

		addPoints(percent, segmentIndextToAdd, points, normals, mirror);
	}

	addPoints(1, segmentIndextToAdd, points, normals, mirror);




	Ogre::ManualObject *man = mSceneManager->createManualObject();
	man->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_TRIANGLE_LIST);
	for (unsigned int i =0; i < points.size(); i = i +1)
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

	if (startCap)
	{
		man->triangle(0,2,1);
		man->triangle(7,2,0);
		man->triangle(7,6,2);
		man->triangle(6,3,2);
		man->triangle(6,5,3);
		man->triangle(5,4,3);

	}

	for (unsigned int i = 0; i < points.size()/8 - 1; i = i + 1)
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

	trackPath->AddPathSegment(deltap1 + point, point + 2 * deltap1, point + 3 * deltap1, q*n1, q*(q*n1), q*q*q*n1,  BezierPath::Kind::TWIST);


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
		trackPath->AddPathSegment(p1, p2, p3, normals[i*3], normals[i*3+1], normals[i*3+2], BezierPath::Kind::LOOP);
		addTrackNodes(trackPath->NumSegments() - 1);
		previous = p3;
	}
}

void
	World::AddJump()
{
	Ogre::Vector3 point;
	Ogre::Vector3 direction;
	Ogre::Vector3 right;
	Ogre::Vector3 up;


	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);

	Ogre::Vector3 p1 = point + direction*100;
	Ogre::Vector3 p2 = p1 + direction*150;

	Ogre::Vector3 p3 = p2 + direction * 100;
	p3.y = p2.y;

	trackPath->AddPathSegment(p1, p2, p3);
	addTrackNodes(trackPath->NumSegments() - 1);

	Ogre::Vector3 flatDirection;
	Ogre::Vector3 flatUp;
	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,flatDirection,right,flatUp);


	p1 = point +  flatDirection*50;
	p2 = point + flatDirection*50 + flatUp * 50;
	p3 = point + flatDirection * 60 + flatUp * 100;

	trackPath->AddPathSegment(p1, p2, p3);
	addTrackNodes(trackPath->NumSegments() - 1);

	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);

	p1 = point +  direction *100;
	p2 = p1 + flatDirection * 100;
	p3 = point + flatDirection * 300;

	trackPath->AddPathSegment(p1, p2, p3, BezierPath::Kind::GAP);
	//	addTrackNodes(trackPath->NumSegments() - 1);

	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);

	p1 = point + direction * 30;
	p2 = p1 + (direction + flatDirection) * 30;
	p3 = p2 + flatDirection * 30;

	trackPath->AddPathSegment(p1, p2, p3);
	addTrackNodes(trackPath->NumSegments() - 1, true);


}






void
	World::AddSegment(Ogre::Vector3 deltap1, Ogre::Vector3 deltap2, Ogre::Vector3 deltap3, BezierPath::Kind segmentType)
{
	Ogre::Vector3 lastPoint = trackPath->getPoint(trackPath->NumSegments() - 1, 1);


	int len  = trackPath->NumSegments();

	trackPath->AddPathSegment(deltap1 + lastPoint,
		deltap2  + lastPoint,
		deltap3 + lastPoint,
		segmentType);



	bool barrier =  (rand() / (float) RAND_MAX > 0.3);
	addTrackNodes(trackPath->NumSegments() - 1);
}


void World::AddBarrierSegment(BezierPath::Kind type)
{
	Ogre::Vector3 point;
	Ogre::Vector3 direction;
	Ogre::Vector3 right;
	Ogre::Vector3 up;

	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);

	direction = direction * 400;
	Ogre::Vector3 direction2 = direction*2;
	Ogre::Vector3 direction3 = direction*3;



	// b = BARRIER_TOP;
	AddSegment(direction, direction2, direction3, type);

}

void World::AddObjects(int segment, bool player)
{

	if (((trackPath->kind(segment) != BezierPath::Kind::BLADES) && trackPath->kind(segment) != BezierPath::Kind::BOOST
		  && trackPath->kind(segment) != BezierPath::Kind::SHIELD && trackPath->kind(segment) != BezierPath::Kind::MAGNET ) || 
		  trackPath->getObjectPlaced(segment))
	{
		return;
	}

	trackPath->setObjectPlaced(segment, true);

	if (trackPath->kind(segment) == BezierPath::Kind::BLADES)
	{

		float barrierPercent = 0.9f;

		float relX[6];
		float relY[6];
		int numBlades = 0;

		float b = (float) rand() /(float) RAND_MAX;
		if (!mUseFrontBack)
		{
			b = b * 0.749f;
		}

		HUD::Kind type;
		if (b < 0.25)
		{
			type = HUD::Kind::right;
			relX[0] = -width; 
			relY[0] = 5;

			relX[1] = -width;
			relY[1] = 11;

			relX[2] = -width;
			relY[2] = 17;

			relX[3] = 0;
			relY[3] = 5;

			relX[4] = 0;
			relY[4] = 11;

			relX[5] = 0;
			relY[5] = 17;
			numBlades = 6;

		}
		else if (b < 0.5)
		{
			type = HUD::Kind::left;

			relX[0] = width; 
			relY[0] = 5;

			relX[1] = width;
			relY[1] = 11;

			relX[2] = width;
			relY[2] = 17;

			relX[3] = 0;
			relY[3] = 5;

			relX[4] = 0;
			relY[4] = 11;

			relX[5] = 0;
			relY[5] = 17;
			numBlades = 6;

		}
		else if (b < 0.75)
		{
			type = HUD::Kind::center;

			relX[0] = width; 
			relY[0] = 5;

			relX[1] = width;
			relY[1] = 11;

			relX[2] = width;
			relY[2] = 17;

			relX[3] = -width;
			relY[3] = 5;

			relX[4] = -width;
			relY[4] = 11;

			relX[5] = -width;
			relY[5] = 17;

			numBlades = 6;
		}
		else //  if (b < 1.0)
		{
			type = HUD::Kind::down;

			relX[0] = width;
			relY[0] = 14;

			relX[1] = width;
			relY[1] = 21;

			relX[2] = 0;
			relY[2] = 14;

			relX[3] = 0;
			relY[3] = 21;

			relX[4] = -width;
			relY[4] = 14;

			relX[5] = -width;
			relY[5] = 21;

			numBlades = 6;

		}

		for (int i = 0; i < numBlades; i++)
		{
			RunnerObject *saw = new RunnerObject(RunnerObject::BLADE);
			saw->loadModel("sawblade.mesh", SceneManager());
			if (mSimpleMaterials)
				saw->setMaterial("simpleOrange");
			if (type == HUD::Kind::down)
			{
				saw->setScale(Ogre::Vector3(8,5,8));
			}
			else
			{
				saw->setScale(Ogre::Vector3(5,5,5));
			}

			Ogre::Vector3 pos;
			Ogre::Vector3 forward;
			Ogre::Vector3 right;
			Ogre::Vector3 up;


			getWorldPositionAndMatrix(segment, barrierPercent, relX[i], relY[i], pos,forward, right, up, player);
			Ogre::Quaternion q(-right,up,forward);

			saw->setPosition(pos); ///5
			saw->setOrientation(q);
			ItemQueueData d(segment,barrierPercent,relX[i], relY[i], saw);
			d.xtraData = type;
			mSawPowerup->enqueue(d);
		}
	}
	else if (trackPath->kind(segment) == BezierPath::Kind::BOOST || trackPath->kind(segment) == BezierPath::Kind::SHIELD ||
		      trackPath->kind(segment) == BezierPath::Kind::MAGNET)
	{

		float barrierPercent = 0.9f;

		float relX;
		float relY = 7;

		float b = (float) rand() /(float) RAND_MAX;
		if (b < 0.33)
		{
			relX = -width;
		}
		else if (b < 0.66)
		{
			relX = 0;
		}
		else 
		{
			relX = width;
		}


		Ogre::Vector3 pos;
		Ogre::Vector3 forward;
		Ogre::Vector3 right;
		Ogre::Vector3 up;

		getWorldPositionAndMatrix(segment, barrierPercent, relX, relY, pos,forward, right, up, player);
		Ogre::Quaternion q(-right,up,forward);

		RunnerObject *obj;
		if  (trackPath->kind(segment) == BezierPath::Kind::BOOST)
		{
			obj = new RunnerObject(RunnerObject::SPEED);
			obj->loadModel("Arrow.mesh", SceneManager());
			obj->setScale(Ogre::Vector3(6,6,6));
			obj->setPosition(pos); ///5
			obj->setOrientation(q);
			obj->pitch(Ogre::Degree(50));

		}
		else if (trackPath->kind(segment) == BezierPath::Kind::SHIELD)
		{
			obj = new RunnerObject(RunnerObject::SHEILD);
			obj->loadModel("Shield.mesh", SceneManager());
			obj->setScale(Ogre::Vector3(6,6,6));
			obj->setPosition(pos); ///5
			obj->setOrientation(q);
			obj->pitch(Ogre::Degree(90));

		}
		else if (trackPath->kind(segment) == BezierPath::Kind::MAGNET)
		{
			obj = new RunnerObject(RunnerObject::MAGNET);
			obj->loadModel("Magnet.mesh", SceneManager());
			obj->setScale(Ogre::Vector3(5,5,5));
			obj->setPosition(pos + up * 5); ///5
			obj->setOrientation(q);
			obj->pitch(Ogre::Degree(90));
			obj->yaw(Ogre::Degree(180));

		}
		else
		{
			// Should pribably throw here ...
		}



		ItemQueueData d(segment,barrierPercent,relX, relY, obj);
		// d.xtraData = type;
		mSawPowerup->enqueue(d);

	}
	else
	{
		int x = 3;
		x++;


	}
}



void
	World::AddNormalSegment()
{		
	Ogre::Vector3 point;
	Ogre::Vector3 direction;
	Ogre::Vector3 right;
	Ogre::Vector3 up;


	trackPath->getPointAndRotaionMatrix(trackPath->NumSegments()-1, 1,point,direction,right,up);


	Ogre::Vector3 direction2 = direction + (((rand() / (float) RAND_MAX) * 2  - 1) * right)
		+  (((rand() / (float) RAND_MAX) * 0.6f  - 0.3f) * up) ;

	direction2.normalise();

	Ogre::Vector3 direction3 = direction2 + (((rand() / (float) RAND_MAX)* 1  - 0.5f) * right)
		+  (((rand() / (float) RAND_MAX) * 0.2f  - 0.1f) * up) ;

	direction = direction * 400;
	direction2 = direction + direction2 * 400;
	direction3 = direction2 + direction3 * 400;

	AddSegment(direction,direction2, direction3);
}
void 
	World::AddRandomSegment()
{

	float r = (rand() / (float) RAND_MAX);

	if ((r < mObsFreq) && mLastObjSeg >= mObsGap)
	{
		mLastObjSeg = 0;
		r = (rand() / (float) RAND_MAX);
		if (r < 0.25 && mUseFrontBack)
		{
			AddJump();
		}
		else
		{
			AddBarrierSegment(BezierPath::Kind::BLADES);
		}
	}
	else
	{
		mLastObjSeg++;
		r = (rand() / (float) RAND_MAX);
		if (r > 0.95)
		{
			float r2 = (rand() / float (RAND_MAX));
			if (r2 > 0.5f)
			{
				AddBarrierSegment(BezierPath::Kind::SHIELD);
			}
			else if (r2 >= 0.0f)
			{
				AddBarrierSegment(BezierPath::Kind::BOOST);
			}
			else
			{
				AddBarrierSegment(BezierPath::Kind::MAGNET);
			}
		}
		else if (r > 0.88)
		{
			AddTwisty();
		}
		else if (r > 0.84)
		{
			AddLoop();
		}
		else
		{
			AddNormalSegment();
		}
	}

}


void World::endGame()
{
	mBase->endGame();
	// HACK!!! Change!!



}

void 
	World::Think(float time)
{

	float radiansPerSecond = 1.0f;
	float sawRadiansPerSecond = 7.0f;

	for (int i = 0; i < mCoins->size(); i++)
	{
		ItemQueueData d = mCoins->atRelativeIndex(i);
		d.object->pitch(Ogre::Radian(radiansPerSecond * time));

	}
	int direction = 1;
	for (int i = 0; i < mSawPowerup->size(); i++)
	{
		ItemQueueData d = mSawPowerup->atRelativeIndex(i);
		if (d.object->type() == RunnerObject::BLADE)
		{
			d.object->yaw(Ogre::Radian(sawRadiansPerSecond * time * direction));
			direction = direction * -1;
		}
		else if (d.object->type() == RunnerObject::SPEED)
		{
			d.object->yaw(Ogre::Radian(radiansPerSecond * time));
		}
		else if (d.object->type() == RunnerObject::SHEILD)
		{
			d.object->roll(Ogre::Radian(radiansPerSecond * -1 * time));
		}
	}
}


