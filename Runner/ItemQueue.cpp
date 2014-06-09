#include "ItemQueue.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "RunnerObject.h"

//int mHead;
//int mTail;
//int mSize;
//int mMaxSize;

//Ogre::SceneNode **sceneNodeData;
//int *segmentIndex;
//float *segmentPercent;
//float *relativeX;
//float *relativeY;

ItemQueue::ItemQueue(int maxSize)
{
	mMaxSize = maxSize;
	mData = (ItemQueueData *) malloc(maxSize * sizeof(ItemQueueData));
	mSize = 0;
	mHead = 0;
	mTail = 0;
}

ItemQueue::~ItemQueue()
{

    ItemQueueData d;
    while (size() > 0)
    {
        d = dequeue();
        if (d.object != NULL)
        {
            delete d.object;
        }
    }
}

void 
ItemQueue::enqueue(int segmentIndex, float segmentPercent, float relativeX, float relativeY, RunnerObject* obj)
{
	ItemQueueData d(segmentIndex, segmentPercent, relativeX, relativeY, obj);
	enqueue(d);
}

ItemQueueData 
	ItemQueue::dequeue()
{
	ItemQueueData d;
	if (mSize > 0)
	{
		d = mData[mHead];
		mHead++;
		if (mHead >= mMaxSize)
		{
			mHead = 0;
		}
		mSize--;
	}
	return d;
}

void ItemQueue::enqueue(ItemQueueData data)
{
	if (mSize == mMaxSize)
	{
		ItemQueueData *newData = (ItemQueueData*) malloc(mMaxSize * 2 * sizeof(ItemQueueData));
		for (int i = 0; i < mMaxSize; i++)
		{
			newData[i] = mData[mHead++];
			if (mHead >= mMaxSize)
			{
				mHead = 0;
			}
		}
		free(mData);
		mData = newData;
		mHead = 0;
		mTail = mMaxSize;
		mMaxSize *= 2;
	}
	mSize++;
	mData[mTail] = data;
	mTail++;
	if (mTail >= mMaxSize)
	{
		mTail = 0;
	}
}

void 
ItemQueue::setRelativeIndex(int index, ItemQueueData data)
{
	if (index < mMaxSize - mHead)
	{
		 mData[(index + mHead)] = data;
	}

}

void ItemQueue::atRelativeIndex(int index, int &segmentIndex, float &segmentPercent, float &relativeX, float &relativeY, Ogre::SceneNode* &sceneNode)
{
	ItemQueueData d = atRelativeIndex(index);
	segmentIndex = d.segmentIndex;
	segmentPercent = d.segmentPercent;
	relativeX = d.relativeX;
	relativeY = d.relativeY;

}
ItemQueueData ItemQueue::atRelativeIndex(int index)
{
	if (index < mMaxSize - mHead)
	{
		return mData[(index + mHead)];
	}

	return mData[(index - (mMaxSize - mHead))];

}



