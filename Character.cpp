#include "Character.h"

using namespace std;

typedef unsigned short POS;

Character::Character() { // Note to self: Computers are stupid
	hp = hpMax = attack = defense = 0;
	healing = false;
}

Character::Character(POS hpMax, POS atk, POS def) {
	this->hp = this->hpMax = hpMax;
	this->attack = atk;
	this->defense = def;
	healing = false;
}

Character::~Character() {
	// Destructor
}

short Character::getHP() {
	return hp;
}

POS Character::getHPMax() {
	return hpMax;
}

POS Character::getAttack() {
	return attack;
}

POS Character::getDefense() {
	return defense;
}

void Character::addToHP(short amount) {
	hp += amount;
	// Magic control statements to check if the value is too high or low
	hp = (hp > hpMax ? hpMax : hp);
	if (!healing) { // Check if the user used a Mushroom
		hp = (hp < 0 ? 0 : hp);
	}
}

void Character::addToHPMax(short amount) {
	hpMax += amount;
}

void Character::addToAttack(short amount) {
	attack += amount;
	attack = (attack <= 2 ? 2 : attack); // Cap check (Minimum of 2)
}

void Character::addToDefense(short amount) {
	defense += amount;
	defense = (defense <= 1 ? 1 : defense);
}

void Character::setHP(short set) {
	hp = set;

	hp = (hp > hpMax ? hpMax : hp);
	hp = (hp < 0 ? 0 : hp);
}

void Character::setHPMax(POS set) {
	hpMax = set;
}

void Character::setAttack(POS set) {
	attack = set;
}

void Character::setDefense(POS set) {
	defense = set;
}

void Character::setHealing(bool toggle) {
	healing = toggle;
}

void Character::resetStats(POS newHPMax, POS newAtk, POS newDef) {
	hp = hpMax = newHPMax;
	attack = newAtk;
	defense = newDef;
}

bool Character::isAlive() {
	return hp > 0;
}