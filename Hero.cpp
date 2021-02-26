#include "Hero.h"
#include "Enemy.h"
#include <iostream>

Hero::Hero() {
	hp = hpMax = attack = defense = 0;
	tp = tpMax = luck = 0;
	exp = expToNext = coins = 0;
	level = 1;
	power = 0;
	uses = 0;
	air = 2;
}

Hero::Hero(POS hpMax, POS atk, POS def, POS tpMax, POS luck) {
	this->hp = this->hpMax = hpMax;
	this->attack = atk;
	this->defense = def;
	this->tp = this->tpMax = tpMax;
	this->luck = luck;
	exp = expToNext = coins = 0;
	level = 1;
	power = 0;
	updateEXPToNext(); // Must always come after levels are updated
}

Hero::~Hero() {
	// Deconstructor
}

short Hero::getAir() {
	return air;
}

POS Hero::getTP() {
	return tp;
}

POS Hero::getTPMax() {
	return tpMax;
}

POS Hero::getLuck() {
	return luck;
}

POS Hero::getEXP() {
	return exp;
}

POS Hero::getEXPToNext() {
	return expToNext;
}

POS Hero::getLevel() {
	return level;
}

POS Hero::getCoins() {
	return coins;
}

string Hero::getPower() {
	switch (power) {
	case 1:
		return "Fire";
		break;
	case 2:
		return "Raccoon";
		break;
	case 3:
		return "Ice";
		break;
	}
	return "None";
}

POS Hero::getPowerNum() {
	return power;
}

POS Hero::getUses() {
	return uses;
}

POS Hero::getPlayerNum() {
	return playerNum;
}

bool Hero::isPoisoned() {
	return poisoned;
}

bool Hero::canTakeTurn() {
	if (!isAlive()) {
		return false;
	}
	return hasTurn;
}

bool Hero::vsFloorIsComplete() {
	return vsFloorComplete;
}

void Hero::deductAir() {
	air--;
}

void Hero::addToTP(short amount) {
	tp += amount;
	// Magic control statements to check if the value is too high or low
	tp = (tp > tpMax ? tpMax : tp);
	if (!healing) { // Check if the user used a Maple Syrup
		tp = (tp < 0 ? 0 : tp);
	}
}

void Hero::addToTPMax(short amount) {
	tpMax += amount;
}

void Hero::addToLuck(short amount) {
	luck += amount;
}

void Hero::addToEXP(POS amount) {
	exp += amount;
}

void Hero::addToCoins(POS amount) {
	coins += amount;
}

void Hero::setAir(short set) {
	air = set;
	air = (air >= 2 ? air : 2);
}

void Hero::setTP(POS set) {
	tp = set;
}

void Hero::setTPMax(POS set) {
	tpMax = set;
}

void Hero::setLuck(POS set) {
	luck = set;
}

void Hero::levelUp(POS newHPMax, POS newAtk, POS newDef, POS newTPMax, POS newLuck) {
	level++;
	addToHPMax(newHPMax);
	addToAttack(newAtk);
	addToDefense(newDef);
	addToTPMax(newTPMax);
	addToLuck(newLuck);
	setHP(hpMax);
	setTP(tpMax);
}

void Hero::setPower(POS set) {
	power = set;
}

void Hero::deductUse() {
	uses--;
	power = (uses == 0 ? 0 : power);
}

void Hero::setUses(POS set) {
	uses = set;
}

void Hero::setLevelSpeed(POS set) {
	levelSpeed = set;
}

void Hero::setPlayerNum(POS set) {
	playerNum = set;
}

void Hero::togglePoison() {
	poisoned = (poisoned ? false : true);
}

void Hero::setTurn(bool set) {
	hasTurn = set;
}

void Hero::setVsComplete(bool set) {
	vsFloorComplete = set;
}

POS Hero::criticalRoll(POS roll) {
	if (roll >= 0 && roll <= luck) {
		cout << "CRITICAL HIT!\n";
		return 2;
	}
	return 1;
}

void Hero::resetStats(POS newHPMax, POS newAtk, POS newDef, POS newTPMax, POS newLuck, bool mode) {
	level = 1;
	exp = 0;
	hp = hpMax = newHPMax;
	attack = newAtk;
	defense = newDef;
	tp = tpMax = newTPMax;
	luck = newLuck;
	expToNext = 50 * levelSpeed;
	power = uses = 0;
}

void Hero::updateEXPToNext() {
	expToNext += (25 + 25 * level) * levelSpeed;
}

bool Hero::checkForLevelUp() {
	return exp >= expToNext;
}

void Hero::bankruptcy() {
	coins = 0;
}

void Hero::print() {
	cout << "---------------\n";
	cout << "Hero stats are:" << endl;
	cout << "LV: " << level << endl;
	cout << "EXP: " << exp << " / " << expToNext << "\n";
	cout << "COINS: " << coins << endl;
	cout << "HP: " << hp << " / " << hpMax << endl;
	cout << "TP: " << tp << " / " << tpMax << endl;
	cout << "ATK: " << attack << endl;
	cout << "DEF: " << defense << endl;
	cout << "LUCK: " << luck << endl;
	cout << "---------------\n";
}
