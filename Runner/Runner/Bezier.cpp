#include "Bezier.h"



void 
BezierPath::removePathSegment(int startIndex, int endIndex)
{
	controlPoints.erase(controlPoints.begin()+ (startIndex * 3), controlPoints.begin() + (endIndex * 3));
	curveDistance.erase(curveDistance.begin()+ startIndex, curveDistance.begin() + endIndex);
	numSegments -= (endIndex - startIndex);
}

BezierPath::BezierPath(Ogre::Vector3 pt0, Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3) : controlPoints(), normals()
{
	controlPoints.push_back(pt0);
	controlPoints.push_back(pt1);
	controlPoints.push_back(pt2);
	controlPoints.push_back(pt3);
	normals.push_back(Ogre::Vector3::UNIT_Y);
	normals.push_back(Ogre::Vector3::UNIT_Y);
	normals.push_back(Ogre::Vector3::UNIT_Y);
	normals.push_back(Ogre::Vector3::UNIT_Y);
	numSegments = 1;
	float dist = calcDist(0);
	curveDistance.push_back(dist);
}


void
	BezierPath::getPointAndForward(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward)

{
	point = getPoint(pathIndex, percentage);


		if (percentage + 0.03 > 1)
		{
			if (pathIndex == NumSegments() - 1)
			{
				Ogre::Vector3 nextPoint = getPoint(pathIndex, percentage-0.03);
				forward = point - nextPoint;
			}
			else
			{
				Ogre::Vector3 nextPoint = getPoint(pathIndex+1, percentage + 0.03 - 1);
				forward = nextPoint - point;
			}
		}
		else
		{
			Ogre::Vector3 nextPoint = getPoint(pathIndex, percentage + 0.03);
			forward = nextPoint - point;
		}
		forward.normalise();
}


void BezierPath::getPointAndRotaionMatrix(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up)
{
	getPointAndForward(pathIndex, percentage, point, forward);
	Ogre::Vector3 relativeUp;

	if (percentage < 0.33)
	{
		Ogre::Quaternion q = normals[pathIndex*3].getRotationTo(normals[pathIndex*3+1]);
		q = Ogre::Quaternion::Slerp(percentage*3, Ogre::Quaternion::IDENTITY, q,true);
		relativeUp = normals[pathIndex*3];
		relativeUp = (q * normals[pathIndex*3]);
	}
	else if (percentage < 0.66)
	{
		Ogre::Quaternion q = normals[pathIndex*3+1].getRotationTo(normals[pathIndex*3+2]);
		q = Ogre::Quaternion::Slerp((percentage-0.33)*3, Ogre::Quaternion::IDENTITY, q,true);
		relativeUp = (q * normals[pathIndex*3+1]);	
	}
	else
	{
		Ogre::Quaternion q = normals[pathIndex*3+2].getRotationTo(normals[pathIndex*3+3]);
		q = Ogre::Quaternion::Slerp((percentage -0.66)*3, Ogre::Quaternion::IDENTITY, q,true);
		relativeUp = (q * normals[pathIndex*3+2]);	
	}

	right= forward.crossProduct(relativeUp);
	right.normalise();

	if (forward == Ogre::Vector3::ZERO)
	{
		forward = Ogre::Vector3(0,0,1);
	}
	up = right.crossProduct(forward);
	up.normalise();
}

// TODO:  Do the recursive version of this to calculate length a whole lot better
float 
BezierPath::calcDist(int segmentIndex)
{
	float length = 0;
	Ogre::Vector3 currentPoint = getPoint(segmentIndex, 0);
	for (float percent = 0.5; percent <= 1.0; percent += 0.5)
	{
		Ogre::Vector3 nextPoint = getPoint(segmentIndex, percent);
		length += (currentPoint - nextPoint).length();
		currentPoint = nextPoint;
	}

	return length;
}


void
BezierPath::AddPathSegment(Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3)
{
	AddPathSegment(pt1, pt2, pt3, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Y);

}

void
BezierPath::AddPathSegment(Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3,
                           Ogre::Vector3 nrml1, Ogre::Vector3 nrml2, Ogre::Vector3 nrml3)
{
	controlPoints.push_back(pt1);
	controlPoints.push_back(pt2);
	controlPoints.push_back(pt3);
	nrml1.normalise();
	nrml2.normalise();
	nrml3.normalise();
	normals.push_back(nrml1);
	normals.push_back(nrml2);
	normals.push_back(nrml3);
	numSegments++;
	float dist = calcDist(numSegments-1);
	curveDistance.push_back(dist);
}


Ogre::Vector3 
BezierPath::getPoint(int segementIndex, float percent)
{
	if (segementIndex >= numSegments)
	{
		// Should probably throw an error here ...
		return Ogre::Vector3::ZERO;
	}
	int nodeIndex = segementIndex * 3;
	return calculateBezierPoint(percent, controlPoints[nodeIndex], controlPoints[nodeIndex+1], controlPoints[nodeIndex+2], controlPoints[nodeIndex+3]);
}



Ogre::Vector3 
BezierPath::calculateBezierPoint(float t, Ogre::Vector3 p0, Ogre::Vector3 p1, Ogre::Vector3 p2, Ogre::Vector3 p3)
{

	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	Ogre::Vector3 p = uuu * p0;
	p += 3 * uu * t * p1;
	p += 3 * u * tt * p2;
	p += ttt * p3;

	return p;

}
