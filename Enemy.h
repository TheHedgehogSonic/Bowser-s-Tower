#ifndef ENEMY_H
#define ENEMY_H
#include <iostream>
#include "Character.h"

using namespace std;

typedef unsigned short POS;

class Enemy : public Character {
public:
	Enemy();
	Enemy(POS hpMax, POS atk, POS def);
	virtual ~Enemy();
	string getName();
	POS getType();
	POS getMinEXP();
	POS getMaxEXP();
	POS getMinLoot();
	POS getMaxLoot();
	void scaleHP();
	void scaleRewards();
	void multiplyStats(float factor);
	void setName(POS set);
	void setStatScaler(float set);
	void setBossName(POS set);
	void setType(POS set);
	void setMinEXP(POS set);
	void setMaxEXP(POS set);
	void setMinLoot(POS set);
	void setMaxLoot(POS set);
	void print();
private:
	enum possibleTypes {NORMAL = 0, SPIKY = 1, AERIAL = 2, FIRE = 3, GHOST = 4, WATER = 6, CAVE = 7};
	const string NORMAL_NAMES[3] = {"Goomba", "Shy Guy", "Koopa"};
	const string SPIKY_NAMES[2] = { "Spiny", "Pokey" };
	const string AERIAL_NAMES[3] = { "Paratroopa", "Lakitu", "Bullet Bill" };
	const string FIRE_NAMES[2] = { "Lava Bubble", "Charvaargh" };
	const string GHOST_NAMES[2] = { "Boo", "Peepa" };
	const string BOSS_NAMES[5] = { "Goomboss", "Petey Piranha", "Boom Boom", "Bowser", "Dry Bowser" };
	const string WATER_NAMES[3] = { "Cheep Cheep", "Blooper", "Porcupuffer" };
	const string CAVE_NAMES[3] = { "Gloomba", "Scaredy Rat", "Freezie" };
	string name;
	float statScaler; // NEW: Multiplies stats depending on number of players
	POS type;
	POS minEXP, maxEXP;
	POS minLoot, maxLoot;
};

#endif