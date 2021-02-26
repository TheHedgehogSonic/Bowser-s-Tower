#pragma once
#include "Inventory.h"
#include <iostream>
// AND apparently you can't have enumerations with matching names in both child and parent classes
enum vsItemTypes { VS_MUSHROOM, VS_FIRE_FLOWER, VS_SUPER_LEAF, VS_ICE_FLOWER, CHEST };

class VSInv : public Inventory {
public:
	VSInv();
	~VSInv();
	string getItem(POS type);
	POS getNumItems(POS type);
	POS getPrice(POS type);
	void addAnItem(POS type);
	void consumeItem(POS type);
	void jackUpPrice();
	void steal(VSInv& victim, POS stealItem); // Yes, you can pull a Satou.
	void resetStock();
private:
	POS vsprice[5] = { 2, 5, 3, 15, 30 };
	POS vsitems[5];
};