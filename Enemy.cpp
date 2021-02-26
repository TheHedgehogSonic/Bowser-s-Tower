#include "Enemy.h"
#include "Hero.h"
#include <iostream>

Enemy::Enemy() {
	hp = hpMax = attack = defense = 0;
	minEXP = maxEXP = minLoot = maxLoot = 0;
	type = NORMAL;
	setName(0);
}

Enemy::Enemy(POS hpMax, POS atk, POS def) {
	this->hp = this->hpMax = hpMax;
	this->attack = atk;
	this->defense = def;
	name = "Enemy";
	minEXP = maxEXP = minLoot = maxLoot = 0;
	type = NORMAL;
}

Enemy::~Enemy() {
	// Destructor
}

string Enemy::getName() {
	if (type == 5) {
		return "Boss";
	}
	return name;
}

POS Enemy::getType() {
	return type;
}

POS Enemy::getMinEXP() {
	return minEXP;
}

POS Enemy::getMaxEXP() {
	return maxEXP;
}

POS Enemy::getMinLoot() {
	return minLoot;
}

POS Enemy::getMaxLoot() {
	return maxLoot;
}

void Enemy::scaleHP() {
	hpMax *= statScaler;
	hp *= statScaler;
}

void Enemy::multiplyStats(float factor) {
	minEXP *= factor;
	maxEXP *= factor;
	minLoot *= factor;
	maxLoot *= factor;
	attack *= factor;
}

void Enemy::setName(POS set) {
	switch (type) {
	case NORMAL:
		set = (set >= 3 ? set % 3 : set); // Array Index OoB Exception prevention
		name = NORMAL_NAMES[set];
		break;
	case SPIKY:
		set = (set >= 2 ? set % 2 : set);
		name = SPIKY_NAMES[set];
		break;
	case AERIAL:
		set = (set >= 3 ? set % 3 : set);
		name = AERIAL_NAMES[set];
		break;
	case FIRE:
		set = (set >= 2 ? set % 2 : set);
		name = FIRE_NAMES[set];
		break;
	case GHOST:
		set = (set >= 2 ? set % 2 : set);
		name = GHOST_NAMES[set];
		break;
	case WATER:
		set = (set >= 3 ? set % 3 : set);
		name = WATER_NAMES[set];
		break;
	case CAVE:
		set = (set >= 3 ? set % 3 : set);
		name = CAVE_NAMES[set];
		break;
	default:
		name = "Enemy";
	}
}

void Enemy::setStatScaler(float set) {
	statScaler = set;
}

void Enemy::setBossName(POS set) {
	name = BOSS_NAMES[set];
}

void Enemy::setType(POS set) {
	type = set;
}

void Enemy::setMinEXP(POS set) {
	minEXP = set;
}

void Enemy::setMaxEXP(POS set) {
	maxEXP = set;
}

void Enemy::setMinLoot(POS set) {
	minLoot = set;
}

void Enemy::setMaxLoot(POS set) {
	maxLoot = set;
}

void Enemy::print() {
	cout << "----------------\n";
	cout << name << " stats are:\n";
	cout << "HP: " << hp << " / " << hpMax << "\n";
	cout << "ATK: " << attack << "\n";
	cout << "DEF: " << defense << "\n";
	cout << "REWARD: " << minEXP << " - " << maxEXP << "\n";
	cout << "LOOT: " << minLoot << " - " << maxLoot << "\n";
	cout << "----------------\n";
}

void Enemy::scaleRewards() {
	minEXP *= 1.5;
	maxEXP *= 1.5;
}
