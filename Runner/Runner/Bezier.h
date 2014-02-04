#include "OgreVector3.h"
#include <vector>



class BezierPath
{
public:
	BezierPath(Ogre::Vector3 pt0, Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3);
	void AddPathSegment(Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3,
					    Ogre::Vector3 nrml1, Ogre::Vector3 nrml2, Ogre::Vector3 nrml3);
	void AddPathSegment(Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3);
	Ogre::Vector3 getPoint(int segemntIndex, float percent);
	int NumSegments() { return numSegments; }
	void removePathSegment(int startIndex, int endIndex);
	float pathLength(int pathIndex) { return curveDistance[pathIndex]; }
	void getPointAndForward(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward);
	void getPointAndRotaionMatrix(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up);

private:
	std::vector<Ogre::Vector3> controlPoints;
	std::vector<Ogre::Vector3> normals;
	Ogre::Vector3 calculateBezierPoint(float t, Ogre::Vector3 p0, Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3);
	int numSegments;
	std::vector<float> curveDistance;

	float calcDist(int index);

};