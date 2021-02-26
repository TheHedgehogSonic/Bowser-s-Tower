#ifndef HERO_H
#define HERO_H
#include <iostream>
#include "Character.h"

using namespace std;

typedef unsigned short POS;

class Hero: public Character {
public: // Functions
	Hero();
	Hero(POS hpMax, POS atk, POS def, POS tpMax, POS luck);
	virtual ~Hero();
	short getAir();
	POS getTP();
	POS getTPMax();
	POS getLuck();
	POS getEXP();
	POS getEXPToNext();
	POS getLevel();
	POS getCoins();
	string getPower();
	POS getPowerNum();
	POS getUses();
	POS getPlayerNum();
	bool isPoisoned();
	bool canTakeTurn();
	bool vsFloorIsComplete();
	void deductAir();
	void addToTP(short amount);
	void addToTPMax(short amount);
	void addToLuck(short amount);
	void addToEXP(POS amount);
	void addToCoins(POS amount);
	void setAir(short set);
	void setTP(POS set);
	void setTPMax(POS set);
	void setLuck(POS set);
	void levelUp(POS newHPMax, POS newAtk, POS newDef, POS newTPMax, POS newLuck);
	void setPower(POS set);
	void deductUse();
	void setUses(POS set);
	void setLevelSpeed(POS set);
	void setPlayerNum(POS set);
	void togglePoison();
	void setTurn(bool set);
	void setVsComplete(bool set);
	POS criticalRoll(POS roll);
	void resetStats(POS newHPMax, POS newAtk, POS newDef, POS newTPMax, POS newLuck, bool mode = false);
	void updateEXPToNext();
	bool checkForLevelUp();
	void bankruptcy();
	void print();
private: // Variables
	short air; // NEW: Tracks oxygen in water
	POS tp, tpMax, luck;
	POS exp, expToNext, level;
	POS coins;
	POS power, uses;
	POS levelSpeed; // NEW: Tracks how fast a player levels in game | set faster in VS.
	POS playerNum; // NEW: Tracks the player number for multiplayer
	bool poisoned; // NEW: Tracks poison in a cave
	bool hasTurn;
	bool vsFloorComplete;
};

#endif