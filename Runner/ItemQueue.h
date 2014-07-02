#pragma once

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



template <class Queue_Type> class ItemQueue
{
public:
	ItemQueue(int MaxSize);
    ~ItemQueue();

	void enqueue(Queue_Type data);
	void push(Queue_Type data);

	int size() {return mSize;}
	int maxSize() { return mMaxSize;}
	void setRelativeIndex(int index, Queue_Type data);

	Queue_Type atRelativeIndex(int index);
	Queue_Type ItemQueue::dequeue();
	

protected:
	
	void growQueue();

	int mHead;
	int mTail;
	int mSize;
	int mMaxSize;

	Queue_Type *mData;
};


/// OK, so putting code in the .h file is frowned upon.  Only doing it so templates work properly,
/// so this thing will link properly.  Hopefully fixed in C++11 extern template ...

template <class Queue_Type> ItemQueue<Queue_Type>::ItemQueue(int maxSize)
{
	mMaxSize = maxSize;
	mData = (Queue_Type *) malloc(maxSize * sizeof(Queue_Type));
	mSize = 0;
	mHead = 0;
	mTail = 0;
}

template <class Queue_Type> ItemQueue<Queue_Type>::~ItemQueue()
{

    Queue_Type d;
    //while (size() > 0)
    //{
    //    d = dequeue();
    //    if (d.object != NULL)
    //    {
    //        delete d.object;
    //    }
    //}
}


template <class Queue_Type>   Queue_Type
	ItemQueue<Queue_Type>::dequeue()
{
	Queue_Type d;
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


template <class Queue_Type> void ItemQueue<Queue_Type>::growQueue()
{
	if (mSize == mMaxSize)
	{
		Queue_Type *newData = (Queue_Type*) malloc(mMaxSize * 2 * sizeof(Queue_Type));
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

}

template <class Queue_Type>  void ItemQueue<Queue_Type>::enqueue(Queue_Type data)
{
	growQueue();
	mSize++;
	mData[mTail] = data;
	mTail++;
	if (mTail >= mMaxSize)
	{
		mTail = 0;
	}
}

template <class Queue_Type>  void ItemQueue<Queue_Type>::push(Queue_Type data)
{
	growQueue();
	mSize++;
	mHead--;
	if (mHead < 0)
	{
		mHead +=mMaxSize;
	}
	mData[mHead] = data;
}


template <class Queue_Type>  void 
ItemQueue<Queue_Type>::setRelativeIndex(int index, Queue_Type data)
{
	if (index < mMaxSize - mHead)
	{
		 mData[(index + mHead)] = data;
	}

}

template <class Queue_Type>  Queue_Type ItemQueue<Queue_Type>::atRelativeIndex(int index)
{
	if (index < mMaxSize - mHead)
	{
		return mData[(index + mHead)];
	}

	return mData[(index - (mMaxSize - mHead))];

}
