#ifndef CHARACTER_H
#define CHARACTER_H
#include <iostream>

using namespace std;

typedef unsigned short POS;

class Character {
public:
	Character(); // Note to self: All parents need default constructors (no parameters)
	Character(POS hpMax, POS atk, POS def);
	virtual ~Character();
	short getHP();
	POS getHPMax();
	POS getAttack();
	POS getDefense();
	void addToHP(short amount);
	void addToHPMax(short amount);
	void addToAttack(short amount);
	void addToDefense(short amount);
	void setHP(short set);
	void setHPMax(POS set);
	void setAttack(POS set);
	void setDefense(POS set);
	void setHealing(bool toggle);
	void resetStats(POS newHPMax, POS newAtk, POS newDef);
	bool isAlive();
protected:
	short hp;
	POS hpMax, attack, defense;
	bool healing;
};

#endif