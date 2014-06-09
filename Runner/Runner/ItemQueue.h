namespace Ogre {
    class SceneNode;
    class SceneManager;
}
class RunnerObject;

#include "HUD.h"


class ItemQueueData
{
public:

	ItemQueueData()
		: segmentIndex(0), segmentPercent(0.0f), relativeX(0.0f), relativeY(0.0f), object(0), xtraData(HUD::none) {}

	ItemQueueData(int segIndex, float segPercent, float relX, float relY, RunnerObject *ob)
		: segmentIndex(segIndex), segmentPercent(segPercent), relativeX(relX), relativeY(relY), object(ob), xtraData(HUD::none) {}

	ItemQueueData(int segIndex, float segPercent, float relX, float relY)
		: segmentIndex(segIndex), segmentPercent(segPercent), relativeX(relX), relativeY(relY), object(0), xtraData(HUD::none) {}

	RunnerObject *object;
	int segmentIndex;
	float segmentPercent;
	float relativeX;
	float relativeY;
    HUD::Kind xtraData;
    
};



class ItemQueue
{
public:
	ItemQueue(int MaxSize);
    ~ItemQueue();

	void enqueue(int segmentIndex, float segmentPercent, float relativeX, float relativeY, RunnerObject* obj);
	void enqueue(ItemQueueData data);


	int size() {return mSize;}
	int maxSize() { return mMaxSize;}
	void atRelativeIndex(int index, int &segmentIndex, float &segmentPercent, float &relativeX, float &relativeY, Ogre::SceneNode* &sceneNode);
	void setRelativeIndex(int index, ItemQueueData data);

	ItemQueueData atRelativeIndex(int index);
	ItemQueueData ItemQueue::dequeue();


protected:
	
	int mHead;
	int mTail;
	int mSize;
	int mMaxSize;

	ItemQueueData *mData;
};