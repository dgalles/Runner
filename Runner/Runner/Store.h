#pragma once
#include "Menu.h"
#include "OgreString.h"

class Store : public Menu
{
public:



	Store(void);
	Store(Ogre::String header, Ogre::String name, float xpos, float ypos, float ydelta, 
		    std::function<int(void)> getCoins, std::function<void(int)> setCoins, Menu *parent = NULL);

	void AddStoreElem(Ogre::String name,  std::function<int(void)> getValue, std::function<void(int)>,
		              int minValue, int maxValue, std::vector<int> prices, bool canSell = true);

	virtual void enable();

protected:

	Ogre::TextAreaOverlayElement *mBank;
	std::function<int(void)> mGetCoins;
	std::function<void(int)> mSetCoins;

	void changeBank(int delta);

	class StoreItem : public Store::MenuItem
	{
	public:
		StoreItem(Ogre::String text, Ogre::String name, Menu *parent, float x, float y, std::function<int(void)> getValue, std::function<void(int)>,
			int minValue, int maxVal, std::vector<int> prices, bool canSell);
		virtual void Enter();
		virtual void Increase();
		virtual void Decrease();

		void resetStoreItem();

	protected:


		Ogre::OverlayContainer *mBlockPanel;
		std::vector<Ogre::OverlayContainer *> mBlocks;

		
		int mMinValue;
		int mMaxValue;
		bool mCanSell;
		std::vector<int> mPrices;
		std::function<int(void)> mGetValue;
		std::function<void(int)> mSetValue;

	};


	~Store(void);
};

