#include "Bezier.h"



void 
BezierPath::removePathSegment(int startIndex, int endIndex)
{
	mControlPoints.erase(mControlPoints.begin()+ (startIndex * 3), mControlPoints.begin() + (endIndex * 3));
	mCurveDistance.erase(mCurveDistance.begin()+ startIndex, mCurveDistance.begin() + endIndex);
	mNumSegments -= (endIndex - startIndex);
}

BezierPath::BezierPath(Ogre::Vector3 pt0, Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3) : mControlPoints(), mNormals(), mKind()
{
	mControlPoints.push_back(pt0);
	mControlPoints.push_back(pt1);
	mControlPoints.push_back(pt2);
	mControlPoints.push_back(pt3);
	mNormals.push_back(Ogre::Vector3::UNIT_Y);
	mNormals.push_back(Ogre::Vector3::UNIT_Y);
	mNormals.push_back(Ogre::Vector3::UNIT_Y);
	mNormals.push_back(Ogre::Vector3::UNIT_Y);
	mNumSegments = 1;
	float dist = calcDist(0);
	mCurveDistance.push_back(dist);
	mKind.push_back(Kind::NORMAL);
}


void
	BezierPath::getPointAndForward(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward)

{
	point = getPoint(pathIndex, percentage);


		if (percentage + 0.03 > 1)
		{
			if (pathIndex == NumSegments() - 1)
			{
				Ogre::Vector3 nextPoint = getPoint(pathIndex, percentage-0.03f);
				forward = point - nextPoint;
			}
			else
			{
				Ogre::Vector3 nextPoint = getPoint(pathIndex+1, percentage + 0.03f - 1);
				forward = nextPoint - point;
			}
		}
		else
		{
			Ogre::Vector3 nextPoint = getPoint(pathIndex, percentage + 0.03f);
			forward = nextPoint - point;
		}
		forward.normalise();
}


void BezierPath::getPointAndRotaionMatrix(int pathIndex, float percentage, Ogre::Vector3 &point, Ogre::Vector3 &forward, Ogre::Vector3 &right, Ogre::Vector3 &up)
{
	getPointAndForward(pathIndex, percentage, point, forward);
	Ogre::Vector3 relativeUp;

	if (percentage < 0.33f)
	{
		Ogre::Quaternion q = mNormals[pathIndex*3].getRotationTo(mNormals[pathIndex*3+1]);
		q = Ogre::Quaternion::Slerp(percentage*3, Ogre::Quaternion::IDENTITY, q,true);
		relativeUp = mNormals[pathIndex*3];
		relativeUp = (q * mNormals[pathIndex*3]);
	}
	else if (percentage < 0.66)
	{
		Ogre::Quaternion q = mNormals[pathIndex*3+1].getRotationTo(mNormals[pathIndex*3+2]);
		q = Ogre::Quaternion::Slerp((percentage-0.33f)*3, Ogre::Quaternion::IDENTITY, q,true);
		relativeUp = (q * mNormals[pathIndex*3+1]);	
	}
	else
	{
		Ogre::Quaternion q = mNormals[pathIndex*3+2].getRotationTo(mNormals[pathIndex*3+3]);
		q = Ogre::Quaternion::Slerp((percentage -0.66f)*3, Ogre::Quaternion::IDENTITY, q,true);
		relativeUp = (q * mNormals[pathIndex*3+2]);	
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
BezierPath::AddPathSegment(Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3, Kind kind)
{
	AddPathSegment(pt1, pt2, pt3, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Y, kind);

}

void
BezierPath::AddPathSegment(Ogre::Vector3 pt1, Ogre::Vector3 pt2, Ogre::Vector3 pt3,
                           Ogre::Vector3 nrml1, Ogre::Vector3 nrml2, Ogre::Vector3 nrml3, Kind kind)
{
	mControlPoints.push_back(pt1);
	mControlPoints.push_back(pt2);
	mControlPoints.push_back(pt3);
	nrml1.normalise();
	nrml2.normalise();
	nrml3.normalise();
	mNormals.push_back(nrml1);
	mNormals.push_back(nrml2);
	mNormals.push_back(nrml3);
	mNumSegments++;
	float dist = calcDist(mNumSegments-1);
	mCurveDistance.push_back(dist);
	mKind.push_back(kind);
	mBladesPlaced.push_back(false);
}


Ogre::Vector3 
BezierPath::getPoint(int segementIndex, float percent)
{
	if (segementIndex >= mNumSegments)
	{
		// Should probably throw an error here ...
		return Ogre::Vector3::ZERO;
	}
	int nodeIndex = segementIndex * 3;
	return calculateBezierPoint(percent, mControlPoints[nodeIndex], mControlPoints[nodeIndex+1], mControlPoints[nodeIndex+2], mControlPoints[nodeIndex+3]);
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
