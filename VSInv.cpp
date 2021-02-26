#include "VSInv.h"

VSInv::VSInv() {
	for (POS i = 0; i < 5; i++) {
		vsitems[i] = 0;
	}
	buyCap = 3;
	effectiveness = 1;
}

VSInv::~VSInv() {
	// Destructor
}

string VSInv::getItem(POS type) {
	switch (type) {
	case 0:
		return "Mushroom";
	case 1:
		return "Fire Flower";
	case 2:
		return "Super Leaf";
	case 3:
		return "Ice Flower";
	case 4:
		return "Plunder Chest";
	}
}

POS VSInv::getNumItems(POS type) {
	return vsitems[type];
}

POS VSInv::getPrice(POS type) {
	return vsprice[type];
}

void VSInv::addAnItem(POS type) {
	vsitems[type]++;
}

void VSInv::consumeItem(POS type) {
	vsitems[type]--;
}

void VSInv::jackUpPrice() {
	for (POS i = 0; i < 5; i++) {
		vsprice[i] *= 1.5;
	}
}

void VSInv::steal(VSInv& victim, POS stealItem) {
	while (victim.getNumItems(stealItem % 5) == 0 && stealItem < 10) {
		stealItem++;
	}

	if (stealItem < 10) {
		victim.consumeItem(stealItem % 5);
		addAnItem(stealItem % 5);
		cout << "You stole a " << getItem(stealItem % 5) << " from your opponent!\n";
	}
	else {
		cout << "Your opponent's inventory is empty, and the plunder chest failed.\n";
	}
}

void VSInv::resetStock() {
	vsitems[0] = 3; // Give 3 Mushrooms
	for (POS i = 1; i < 5; i++) {
		vsitems[i] = 0;
	}

	vsprice[0] = 2;
	vsprice[1] = 5;
	vsprice[2] = 3;
	vsprice[3] = 15;
	vsprice[4] = 30;
	effectiveness = 1;
}
