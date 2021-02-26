#ifndef INVENTORY_H
#define INVENTORY_H
#include "Hero.h"
#include <iostream>

using namespace std;

typedef unsigned short POS;
// Apparently, numbers don't go in enumeration constant names at all...
enum itemTypes { MUSHROOM, SYRUP, FIRE_FLOWER, SUPER_LEAF, ICE_FLOWER, C_MEDAL, D_MEDAL, STAR, LIFE };

class Inventory {
public:
	Inventory();
	Inventory(POS mushrooms, POS fireFlowers, POS superLeaves, POS iceFlowers, POS lives);
	~Inventory();
	string getItem(POS type);
	POS getNumItems(POS type);
	string getSpecialItem(POS type);
	POS getPrice(POS index);
	POS getBuyCap();
	POS getEffectiveness();
	void addAnItem(POS type);
	void consumeItem(POS type);
	void jackUpPrice(); // Inflates every 20 floors
	void deductStock();
	void setBuyCap(POS set);
	void setEffectiveness(POS set);
	void upEffectiveness();
	bool isUsableInPrep(POS item);
	void resetStock();
protected:
	POS buyCap;
	POS effectiveness;
private:
	POS price[9] = { 5, 5, 20, 10, 35, 50, 25, 75, 100 };
	POS items[9];
};

#endif