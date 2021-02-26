#include "Inventory.h"
#include <iostream>

Inventory::Inventory() {
	for (POS i = 0; i < 9; i++) {
		items[i] = 0;
	}
	buyCap = 3;
	effectiveness = 1;
}

Inventory::Inventory(POS mushrooms, POS fireFlowers, POS superLeaves, POS iceFlowers, POS lives) {
	items[MUSHROOM] = mushrooms;
	items[FIRE_FLOWER] = fireFlowers;
	items[SUPER_LEAF] = superLeaves;
	items[ICE_FLOWER] = iceFlowers;
	items[LIFE] = lives;
	buyCap = 3;
	effectiveness = 1;
}

Inventory::~Inventory() {
	// Destructor
}

string Inventory::getItem(POS type) {
	switch (type) { // Breaks not required because of returns
	case 0:
		return "Mushroom";
	case 1:
		return "Maple Syrup";
	case 2:
		return "Fire Flower";
	case 3:
		return "Super Leaf";
	case 4:
		return "Ice Flower";
	case 5:
		return "Courage Medal";
	case 6:
		return "Daredevil Medal";
	case 7:
		return "Star";
	case 8:
		return "1UP";
	}
	return "Missingno";
}

POS Inventory::getNumItems(POS type) {
	return items[type];
}

string Inventory::getSpecialItem(POS type) {
	switch (type) {
	case 0:
		return "Peach's Tears";
	case 1:
		return "Super Boost";
	case 2:
		return "Fabric Padding";
	case 3:
		return "Energy Spell";
	case 4:
		return "Grambi's Blessing";
	case 5:
		return "Orb Essence";
	}
	return "Missingno";
}

POS Inventory::getPrice(POS index) {
	return price[index];
}

POS Inventory::getBuyCap() {
	return buyCap;
}

POS Inventory::getEffectiveness() {
	return effectiveness;
}

void Inventory::addAnItem(POS type) {
	items[type]++;
}

void Inventory::consumeItem(POS type) {
	items[type]--;
}

void Inventory::jackUpPrice() {
	POS newPrice;
	for (POS i = 0; i < 9; i++) {
		newPrice = (POS) price[i] * 1.5;
		price[i] = newPrice;
	}
}

void Inventory::deductStock() {
	buyCap--;
}

void Inventory::setBuyCap(POS set) {
	buyCap = set;
}

void Inventory::setEffectiveness(POS set) {
	effectiveness = set;
}

void Inventory::upEffectiveness() {
	effectiveness++;
}

bool Inventory::isUsableInPrep(POS item) {
	return item < 7;
}

void Inventory::resetStock() {
	items[0] = 3;
	for (POS i = 1; i < 9; i++) {
		items[i] = 0;
	}

	price[0] = 5;
	price[1] = 5;
	price[2] = 20;
	price[3] = 10;
	price[4] = 35;
	price[5] = 50;
	price[6] = 25;
	price[7] = 75;
	price[8] = 100;
	buyCap = 3;
	effectiveness = 1;
}
