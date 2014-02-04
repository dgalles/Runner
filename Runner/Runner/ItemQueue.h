namespace Ogre {
    class SceneNode;
    class SceneManager;
}

class ItemQueueData
{
public:

	ItemQueueData()
		: segmentIndex(0), segmentPercent(0.0f), relativeX(0.0f), relativeY(0.0f), sceneNode(0) {}

	ItemQueueData(int segIndex, float segPercent, float relX, float relY, Ogre::SceneNode *sn)
		: segmentIndex(segIndex), segmentPercent(segPercent), relativeX(relX), relativeY(relY), sceneNode(sn) {}

	ItemQueueData(int segIndex, float segPercent, float relX, float relY)
		: segmentIndex(segIndex), segmentPercent(segPercent), relativeX(relX), relativeY(relY), sceneNode(0) {}

	Ogre::SceneNode *sceneNode;
	int segmentIndex;
	float segmentPercent;
	float relativeX;
	float relativeY;
};



class ItemQueue
{
public:
	ItemQueue(int MaxSize);

	void enqueue(int segmentIndex, float segmentPercent, int relativeX, float relativeY, Ogre::SceneNode* sceneNode);
	void enqueue(ItemQueueData data);


	int size() {return mSize;}
	int maxSize() { return mMaxSize;}
	void atRelativeIndex(int index, int &segmentIndex, float &segmentPercent, int &relativeX, float &relativeY, Ogre::SceneNode* &sceneNode);
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