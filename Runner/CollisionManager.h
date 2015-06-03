#pragma once
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreAxisAlignedBox.h"

class OBB
{
public:
    OBB(Ogre::AxisAlignedBox b);
    OBB(Ogre::AxisAlignedBox b, Ogre::Vector3 position, Ogre::Quaternion orientation);
    ~OBB(void);

    void setPosition(Ogre::Vector3 newPosition);
    void translate(Ogre::Vector3 deltaPosition);
    void setOrientation(Ogre::Quaternion orentation);
    bool collides(OBB &other, Ogre::Vector3 &MTD);
    bool collides(OBB *other, Ogre::Vector3 &MTD);
    void setScale(Ogre::Vector3 scale);

protected:
    mutable Ogre::AxisAlignedBox mAABB;
    Ogre::Vector3 mPosition;
    Ogre::Quaternion mOrientation;
    Ogre::Vector3 mScale;
    void resetAABBandPoints();
    std::vector<Ogre::Vector3> mPointsLocal;
    std::vector<Ogre::Vector3> mPointsGlobal;
    std::vector<Ogre::Vector3> mNormals;
    void setPoints(Ogre::AxisAlignedBox b);
    void setNormals();
	bool mGlobalPointsDirty;

};


