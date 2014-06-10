#include "RunnerObject.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "CollisionManager.h"

RunnerObject::RunnerObject(ObjectType type) : mType(type)
{
    mCollision = NULL;
    mSceneNode = NULL;
    mOrentation = Ogre::Quaternion::IDENTITY;
    mPosition = Ogre::Vector3::ZERO;
}


RunnerObject::~RunnerObject(void)
{
    mScale = Ogre::Vector3::UNIT_SCALE;
    if (mSceneNode != NULL)
    {
        	mSceneManager->getRootSceneNode()->removeChild(mSceneNode);

	unsigned short index = 0;
	Ogre::MovableObject *mo = mSceneNode->detachObject(index);
	Ogre::Entity *ent = static_cast<Ogre::Entity *>(mo);

	mSceneManager->destroyEntity(ent);
	mSceneManager->destroySceneNode(mSceneNode);

    }
    delete mCollision;
}


bool RunnerObject::collides(RunnerObject *other, Ogre::Vector3 &MTD)
{
    return mCollision->collides(other->mCollision, MTD);

}
bool RunnerObject::collides(const RunnerObject &other, Ogre::Vector3 &MTD)
{
    return mCollision->collides(other.mCollision, MTD);
}




void RunnerObject::setScale(Ogre::Vector3 newScale)
{
	mScale = newScale;
	mSceneNode->setScale(newScale);
	mCollision->setScale(newScale);
}


	Ogre::Vector3 
		RunnerObject::minPointLocalScaled()
	{
		return mMinPointLocal * mScale;
	}
	Ogre::Vector3 
		RunnerObject::maxPointLocalScaled()
	{
		return mMaxPointLocal * mScale;
	}


void 
	RunnerObject::loadModel(Ogre::String modelName, Ogre::SceneManager *sm)
{
    mSceneManager = sm;
    Ogre::Entity *ent1 =sm->createEntity(modelName);
	mSceneNode =sm->getRootSceneNode()->createChildSceneNode();
	mSceneNode->attachObject(ent1);
    mCollision = new OBB(ent1->getBoundingBox());
	mMaxPointLocal = ent1->getBoundingBox().getMaximum();
	mMinPointLocal =  ent1->getBoundingBox().getMinimum();
}
void RunnerObject::setPosition(Ogre::Vector3 newPosition)
{
    mSceneNode->setPosition(newPosition);
    mCollision->setPosition(newPosition);
    mPosition = newPosition;

}
void RunnerObject::setOrientation(Ogre::Quaternion newOrientation)
{
    mSceneNode->setOrientation(newOrientation);
    mCollision->setOrientation(newOrientation);
    mOrentation = newOrientation;
}
void RunnerObject::yaw(Ogre::Degree d)
{
    mSceneNode->yaw(d);
    mCollision->setOrientation(mSceneNode->getOrientation());
}
void RunnerObject::pitch(Ogre::Degree d)
{
    mSceneNode->pitch(d);
    mCollision->setOrientation(mSceneNode->getOrientation());
}

void RunnerObject::translate(Ogre::Vector3 delta)
{
	mCollision->translate(delta);
	mSceneNode->translate(delta);
	mPosition += delta;
}



void RunnerObject::roll(Ogre::Degree d)
{
    mSceneNode->roll(d);
    mCollision->setOrientation(mSceneNode->getOrientation());


}

void RunnerObject::yaw(Ogre::Radian r)
{
        mSceneNode->yaw(r);
    mCollision->setOrientation(mSceneNode->getOrientation());

}
void RunnerObject::pitch(Ogre::Radian r)
{
        mSceneNode->pitch(r);
    mCollision->setOrientation(mSceneNode->getOrientation());

}
void RunnerObject::roll(Ogre::Radian r)
{
        mSceneNode->roll(r);
    mCollision->setOrientation(mSceneNode->getOrientation());

}