#pragma once

#include "OgreString.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"

class OBB;

namespace Ogre
{
    class SceneManager;
    class SceneNode;
}
class RunnerObject
{
public:

	enum ObjectType {PLAYER, COIN, BLADE, SPEED, SHEILD, REBUILD_ARMOR};

    RunnerObject(ObjectType type);
    ~RunnerObject(void);

	ObjectType type() {return mType;}

    void loadModel(Ogre::String modelName, Ogre::SceneManager *sm);
    void setPosition(Ogre::Vector3 newPosition);
    void setOrientation(Ogre::Quaternion newOrientation);
    void yaw(Ogre::Degree d);
    void pitch(Ogre::Degree d);
    void roll(Ogre::Degree d);

    void yaw(Ogre::Radian r);
    void pitch(Ogre::Radian r);
    void roll(Ogre::Radian r);
    void setScale(Ogre::Vector3 newScale);

	void translate(Ogre::Vector3 delta);

    bool collides(RunnerObject *other, Ogre::Vector3 &MTD);
    bool collides(const RunnerObject &other, Ogre::Vector3 &MTD);

	Ogre::Vector3 minPointLocalScaled();
	Ogre::Vector3 maxPointLocalScaled();

    Ogre::Vector3 getPosition() { return mPosition; }
    Ogre::Quaternion getOrientation() { return mOrentation; }
    Ogre::Vector3 getScale() { return mScale;}

protected:
	ObjectType mType;
    Ogre::SceneNode *mSceneNode;
    OBB *mCollision;
    Ogre::Vector3 mPosition;
    Ogre::Quaternion mOrentation;
    Ogre::SceneManager *mSceneManager;
    Ogre::Vector3 mScale;
	Ogre::Vector3 mMinPointLocal;
	Ogre::Vector3 mMaxPointLocal;


};

