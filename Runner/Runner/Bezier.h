#pragma once

#include "OgreVector3.h"
#include <vector>



class BezierPath
{
public:
	enum Kind {NORMAL, GAP, BLADES_LEFT, BLADES_RIGHT, BLADES_CENTER, BLADES_DOWN, LOOP, TWIST, BOOST, SHIELD, MAGNET};


	BezierPath(Ogre::Vector3 pt0, Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3);
	void AddPathSegment(Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3,
					    Ogre::Vector3 nrml1, Ogre::Vector3 nrml2, Ogre::Vector3 nrml3, Kind kind = NORMAL);
	void AddPathSegment(Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3, Kind kind = NORMAL);
	Ogre::Vector3 getPoint(int segemntIndex, float percent, bool mirrored = false);
	int NumSegments() { return mNumSegments; }
	void removePathSegment(int startIndex, int endIndex);
	float pathLength(int pathIndex) { return mCurveDistance[pathIndex]; }
	void getPointAndForward(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward, bool mirrored = false);
	void getPointAndRotaionMatrix(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up,  bool mirrored = false);
	Kind kind(int segmentIndex) { return mKind[segmentIndex]; }
	bool getObjectPlaced(int segmentIndex) { return mBladesPlaced[segmentIndex]; }
	void setObjectPlaced(int segmentIndex, bool bladeSet) {  mBladesPlaced[segmentIndex] = bladeSet;  }

private:
	std::vector<Ogre::Vector3> mControlPoints;
	std::vector<Ogre::Vector3> mNormals[2];
	Ogre::Vector3 calculateBezierPoint(float t, Ogre::Vector3 p0, Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3);
	int mNumSegments;
	std::vector<float> mCurveDistance;
	std::vector<Kind> mKind;
	std::vector<bool> mBladesPlaced;
	float calcDist(int index);

};