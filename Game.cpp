#include <iostream>
#include "windows.h"
#include <ctime>
#include <cmath>
#include "Hero.h"
#include "Inventory.h"
#include "VSInv.h"
#include "Enemy.h"
#include "FileIO.h"
/////////////////////////////////////////////////////////////////////
// Welcome to Bowser's Tower! An original game by Bennett Johnson. //
// Here, you'll fight your way up to the tower until you're strong //
// enough to beat the Big-Bowz Man himself! How many coins can you //
// plunder in on your way to the top? Enter if you dare...         //
// All used characters are owned by Nintendo (please have mercy).  //
/////////////////////////////////////////////////////////////////////
using namespace std;
// For color text
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
// Typedefs
typedef unsigned int POS32;
typedef unsigned short POS;
// Enumerations
enum sequenceNum {TITLE, CHARACTERS, VS_FLOORS, ROOM, ENEMY, EVENT, PIPE, BOSS, VS_BONUS, VS_BATTLE, YOUR_TURN, WHO_TO_ATTACK,
	VS_ATTACK, INVENTORY, VS_INV, PERSON, ENEMY_TURN, VICTORY, PREPARATION, LEVEL, SHOP, VS_SHOP, UPGRADE, WIN, LOSE, VS_WIN};
enum battleMode {SOLO, COOP, VERSUS};
enum character { MARIO = 0, LUIGI = 1, TOAD = 2, TOADETTE = 3, YOSHI = 4, SPEK = 5 };
// Constants
static const string DOBERVICH_BUMPER_STICKER = "USE YOUR VARIABLES!"; // VERY IMPORTANT
static const POS characterColor[6] = { 12, 10, 9, 13, 14, 1 };
// Objects
FileIO file;
Hero hero;
Hero side;
Inventory inv;
VSInv heroInv;
VSInv sideInv;
Enemy hoard[4];
// Pointers
Hero* activeHero = &hero;
Hero* idleHero = &side;
VSInv* activeInv = &heroInv;
// Variables invisible to the player
POS upgradeScheme[6][5]; // NEW: Added Team Points and Luck
POS sequence = TITLE;
POS prevSequence = TITLE;
POS playingAs = MARIO;
POS coPlayingAs = LUIGI; // NEW: Character tracker for Player 2 (sorry, Luigi)
POS gameMode = SOLO; // NEW: Now includes 2-player co-op and versus!
POS difficulty = 1;
float damageScaler; // NEW: Takes into accountability team attacks to multiply damage
POS enemiesToSpawn = 0, enemyTypeRange = 0;
POS remaining;
POS eventRoll; // NEW: In the advanced ruleset, could cause pipes and blocks to appear
POS highestFloor = 0;
POS floorsToPlay = 40;
POS turns = 0;
POS itemChoice; // NEW: Keeps track of item choice when using an item
bool enemyBuffs = true;
char response = 0;
bool running = true;
bool ruleSet = false;
bool readInstructions = false, readStory = false, readPatches = false;
// Variables visible to the player
POS32 score;
POS currentFloor = 1; // I can't just name it "floor" because that's a built in function.
POS attackType = 0;
// Function prototypes
void setColor(POS color);
Hero initialize(Hero h);
void welcome();
void goodbye();
void story();
void ending();
void instructions();
void patchNotes();
void ascend();
void switchTurn();
void itemAction();
void towerView(POS& playerFloor, POS floorsToDraw);
bool hasQuit();
short rngesus(short low, short high);
POS getInput(POS numOptions);
// Driver
int main() {
	POS moneyReward; // NEW: Ensures each player gets equal coins when winning a battle
	POS teamCost; // NEW: Keeps track of the cost of what a team attack will be
	POS leveling; // NEW: Makes sure the right player gets the right stat boosts
	POS fightSequence; // NEW: Keeps track of the battle type whether it's against opponents or enemies
	srand((unsigned)time(0));
	short damage;
	bool connects;
	// Setting player numbers
	hero.setPlayerNum(1);
	side.setPlayerNum(2);
	// Game loop
	while (running) {
		switch (sequence) {
		case TITLE: // Lets the player choose options and presets before they begin their journey
			welcome();
			// Options
			cout << "The high score is " << file.loadHighScore() << "\n\n";
			setColor(15);
			cout << "1. Start\n2. Change Ruleset (" << (ruleSet ? "Advanced" : "Basic") << ")\n" <<
				"3. View Story\n4. How to Play\n5. Patch Notes\n";
			getInput(5);

			switch (response) {
			case 1: // Can run a number of protocols dependent on which mode is selected
				fightSequence = WHO_TO_ATTACK;
				heroInv.setBuyCap(5);
				sideInv.setBuyCap(5);
				activeInv = &heroInv;
				currentFloor = 1;
				highestFloor = 1;
				// NEW: Input number of players
				setColor(15);
				cout << "Input number of players.\n";
				cout << "1. 1 Player\n2. 2 Player Cooperative\n3. 2 Player Versus\n";
				getInput(3);
				// NEW: Makes sure the players are in the right order each time
				activeHero = &hero;
				idleHero = &side;
				activeHero->setTurn(false);
				idleHero->setTurn(false);
				// Starting the game
				gameMode = response - 1; // NEW: Toggles game mode

				switch (gameMode) {
				case VERSUS:
					hero.setLevelSpeed(1); // Lower numbers equate to faster levels
					side.setLevelSpeed(1);
					break;
				default:
					hero.setLevelSpeed(2);
					side.setLevelSpeed(2);
				}
				hero = initialize(hero);
				side = initialize(side);

				sequence = CHARACTERS;
				break;
			case 2:
				ruleSet = !ruleSet; // Fast way to toggle bools
				setColor(13);
				cout << "Rule set has been set to " << (ruleSet ? "advanced." : "basic.") << "\n" << endl;
				break;
			case 3:
				if (!readStory) {
					story();
					readStory = true;
				}
				else {
					setColor(15);
					cout << "What's the matter? Can't read?\n\n";
				}
				break;
			case 4:
				if (!readInstructions) {
					instructions();
					readInstructions = true;
				}
				else {
					setColor(15);
					cout << "What's the matter? Can't read?\n\n";
				}
				break;
			case 5:
				if (!readPatches) {
					patchNotes();
					readPatches = true;
				}
				else {
					setColor(15);
					cout << "What's the matter? Can't read?\n\n";
				}
				break;
			}
			break;
		case CHARACTERS: // Access the character select via the title screen
			setColor(15);
			if (gameMode > SOLO) {
				cout << "Player 1:";
			}
			
			cout << "\nChoose a character.\n";
			cout << "1. Choose Mario (heavyhitter with lots of attack)\n2. Choose Luigi (tank with lots of defense)\n3. Choose Toad (healthy with lots of HP)\n";
			cout << "4. Choose Toadette (Energetic with lots of TP)\n5. Choose Yoshi (Lucky with lots of luck)\n6. Choose Spek (all around robot with balanced stats)\n";
			getInput(6);

			switch (response) { // NEW: Apparently, I completely missed the possibility of an AIOOBE
			case 0:
				sequence = TITLE;
				break;
			default:
				playingAs = response - 1;
			}
			// NEW: Lets the second player choose
			if (gameMode > SOLO) {
				setColor(15);
				cout << "Player 2:\nNow choose yours (and yes, you can be the same character)\n";
				getInput(6);
				
				switch (response) {
				case 0:
					sequence = TITLE;
					break;
				default:
					coPlayingAs = response - 1;
				}
			}
			else { // Make sure they don't get in the way of registering Game Overs in 1 player
				side.setHP(0);
			}
			// NEW: Sets HP scaler for increased difficulty in Co-op
			for (POS i = 0; i < 4; i++) {
				hoard[i].setStatScaler(gameMode == COOP ? 2.5f : 1);
			}

			sequence = ENEMY;

			if (gameMode == VERSUS) {
				sequence = VS_FLOORS;
			}

			break;
		case VS_FLOORS: // An initiation sequence for versus mode
			setColor(11);
			cout << "Choose the abount of floors to play for.\n";
			setColor(15);
			cout << "1. 10 floors for a quick battle\n2. 20 floors for a casual battle\n"
				<< "3. 40 floors of absolute mayhem\n";
			getInput(3);
			// Giving an amount of floors to play
			switch (response) {
			case 1:
				floorsToPlay = 10;
				break;
			case 2:
				floorsToPlay = 20;
				break;
			case 3:
				floorsToPlay = 40;
				break;
			default:
				setColor(4);
				cout << "This sequence should not exist. Please contact the dev for help.\n";
			}

			inv.setEffectiveness(response * 1.4f); // Setting up item effectiveness accordingly
			sequence = ROOM; // Update to choosing room difficulty
			break;
		case ROOM: // Lets fighters decide the difficulty they wish to try
			setColor(15);
			cout << "Player " << activeHero->getPlayerNum() << ", choose a room. Higher difficulties grant higher rewards.\n";
			cout << "1. A mild room\n2. A fair room\n3. A relentless room\n";
			getInput(3);
			// Deciding the room to give to the player
			enemiesToSpawn = response;
			enemyTypeRange = response + 1;
			difficulty = response;

			sequence = ENEMY;
			break;
		case ENEMY: // This is the phase where enemies get spawned in
			activeHero->setTurn(true);
			if (gameMode == COOP) {
				idleHero->setTurn(true);
			}
			towerView(currentFloor, floorsToPlay);
			turns = 0;

			if (currentFloor % 10 == 5) { // Check to give a shop
				sequence = SHOP;
				// Message at the shop
				setColor(14);
				cout << "You've stumbled upon a shop. Be sure to stock up!\n";

				if (gameMode > SOLO) {
					switchTurn();
					cout << "Player " << activeHero->getPlayerNum() << ", ";
				}

				cout << "You have " << activeHero->getCoins() << " coins.\n";

				if (gameMode > SOLO) {
					cout << "Player " << idleHero->getPlayerNum() << ", You have " << idleHero->getCoins() << " coins.\n";
				}

				inv.setBuyCap(difficulty * 2 + 1);
				break;
			}
			else if (currentFloor % 10 == 0) {
				enemyBuffs = false;

				if (currentFloor >= 40 && gameMode < VERSUS) {
					setColor(6);
					if (currentFloor >= 100 && ruleSet) {
						cout << "Dry Bowser has a bone to pick with you on the 100th floor.\n";
					}
					else {
						cout << "Bowser awaits you on the 40th floor.\n";
					}
					setColor(15);
					cout << "1. Fight him\n2. Keep going up another 10 floors\n";
					// Choose between fighting Bowser and going up
					getInput(2);

					switch (response) {
					case 1:
						currentFloor = (highestFloor > 90 && ruleSet ? 100 : 40);
						enemyBuffs = true;
						sequence = BOSS;
						break;
					case 2:
						setColor(14);
						activeHero->addToEXP(50 * (currentFloor / 10));
						activeHero->addToCoins(50 * (currentFloor / 10));
						cout << "\nYou got " << 50 * (currentFloor / 10) << " bonus EXP and coins!\n";

						if (activeHero->checkForLevelUp() || idleHero->checkForLevelUp()) {
							sequence = LEVEL;
						}
						else if (gameMode < VERSUS) {
							sequence = UPGRADE;
						}
						break;
					}
				}
				else {
					sequence = BOSS;
				}
				break;
			}

			enemyBuffs = true;

			setColor(15);
			activeHero->print();
			
			if (gameMode == COOP) {
				idleHero->print();
			}
			// NEW: Doesn't update enemies to spawn in versus mode, as not to interfere
			if (gameMode < VERSUS) {
				enemiesToSpawn = difficulty / 2 + 1; // Setting up enemy spawn rate
			}

			enemiesToSpawn = (enemiesToSpawn > 4 ? 4 : enemiesToSpawn); // Limits enemies to at most 4
			remaining = enemiesToSpawn; // This is how the game checks for winning

			for (POS i = 0; i < enemiesToSpawn; i++) {
				hoard[i].setType(rngesus(0, enemyTypeRange)); // Set their type
				hoard[i].setName(rngesus(0, 2)); // Update their name
				// Stats
				hoard[i].setHPMax(rngesus(difficulty * 3, difficulty * 6));
				hoard[i].setHP(hoard[i].getHPMax()); // Gets what the Max HP just got set to and applies to HP
				hoard[i].scaleHP(); // NEW: Amplifies enemies for hard mode
				hoard[i].setAttack(rngesus(difficulty + 1, pow(difficulty, 2) + 2));
				hoard[i].setDefense(rngesus(difficulty - 1, difficulty * 2));
				// Raise defense higher for ghosts
				hoard[i].setDefense(hoard[i].getType() == 4 ? hoard[i].getDefense() * 2 : hoard[i].getDefense());
				// Rewards
				hoard[i].setMinEXP(10 + difficulty);
				hoard[i].setMaxEXP(15 + difficulty);
				hoard[i].setMinLoot(2 + difficulty);
				hoard[i].setMaxLoot(5 + difficulty);

				if (gameMode == VERSUS) {
					hoard[i].scaleRewards();
				}
			}

			setColor(6);
			cout << "Floor " << currentFloor << ":\n";
			setColor(15);
			cout << "On your way up the " << (gameMode == VERSUS ? "arena" : "tower") << ", you encounter ";
			// What to print for grammar
			switch (enemiesToSpawn) {
			case 1:
				cout << "a " << hoard[0].getName() << ".\n";
				break;
			default:
				cout << "these enemies:\n";
				for (POS i = 0; i < enemiesToSpawn; i++) {
					cout << hoard[i].getName() << endl;
				}
			}
			sequence = YOUR_TURN;
			break;
		case EVENT: // When you find yourself looking at stage gimmicks
			switch (eventRoll) { // Event roll
			case 17:
			case 18:
				setColor(11);
				cout << "What's this? You found hidden ? Blocks! You don't have much time before the minions catch up, so you'll have to\nchoose just one.\n";
				setColor(15);
				cout << "1. Hit the left block\n2. Hit the middle block\n3. Hit the right block\n";
				// Choose a block
				getInput(3);

				for (POS i = 0; i < 3; i++) {
					POS j = rngesus(0, 8);

					if (i == response - 1) {
						inv.addAnItem(j);
						setColor(14);
						cout << "You got a " << inv.getItem(j) << "!\n";
						system("pause");
					}
				}

				sequence = ENEMY;
				break;
			case 19:
			case 20:
				setColor(11);
				cout << "There's a pipe over in the corner. Will you jump in?\n";
				setColor(15);
				cout << "1. Yes\n2. No\n";
				getInput(2);

				sequence = ENEMY;
				if (response == 1) {
					sequence = PIPE;
				}
				else {
					eventRoll = 1; // Prevents status effects from taking effect even if the player says no
				}
				break;
			}
			break;
		case PIPE:
			activeHero->setTurn(true);
			if (gameMode > SOLO) {
				idleHero->setTurn(true);
			}

			enemyBuffs = true;
			// This enemy spawning algorithm is virtually identical to the OG with some key differences.
			setColor(15);
			activeHero->print();

			if (gameMode == COOP) {
				idleHero->print();
			}

			enemiesToSpawn = difficulty / 2 + 1;
			enemiesToSpawn = (enemiesToSpawn > 4 ? 4 : enemiesToSpawn);
			remaining = enemiesToSpawn;

			for (POS i = 0; i < enemiesToSpawn; i++) {
				hoard[i].setType(eventRoll - 13); // NEW: Now sets enemies from caves or water
				hoard[i].setName(rngesus(0, 2)); // Update their name
				// Stats
				hoard[i].setHPMax(rngesus(difficulty * 6, difficulty * 8));
				hoard[i].setHP(hoard[i].getHPMax()); // Gets what the Max HP just got set to and applies to HP
				hoard[i].scaleHP();
				hoard[i].setAttack(rngesus(difficulty + 2, pow(difficulty, 2) + 4));
				hoard[i].setDefense(rngesus(difficulty + 1, difficulty * 2)); // PROTIP: Don't set the minimum below possible maximums
				// Rewards
				hoard[i].setMinEXP(20 + difficulty);
				hoard[i].setMaxEXP(30 + difficulty);
				hoard[i].setMinLoot(6 + difficulty);
				hoard[i].setMaxLoot(15 + difficulty);
			}

			setColor(6);
			cout << "Inside Pipe\n";
			setColor(15);
			cout << "The pipe has plunged you " << (eventRoll == 19 ? "underwater" : "underground") << ", and you encounter ";
			// What to print for grammar
			switch (enemiesToSpawn) {
			case 1:
				cout << "a " << hoard[0].getName() << ".\n";
				break;
			default:
				cout << "these enemies: \n";
				for (POS i = 0; i < enemiesToSpawn; i++) {
					cout << hoard[i].getName() << endl;
				}
			}
			// NEW: Determine which death clock to choose
			switch (eventRoll) {
			case 19:
				activeHero->setAir(enemiesToSpawn * 2 - 1);
				idleHero->setAir(enemiesToSpawn * 2 - 1);
				cout << "\nCareful! Run out of air, and it's lights out.\n";
				break;
			case 20:
				activeHero->togglePoison();
				idleHero->togglePoison();
				cout << "\nCareful! The cave's air is poisoned, so be quick.\n";
				break;
			}
			sequence = YOUR_TURN;
			break;
		case BOSS: // When you reach a 10th floor, a boss appears
			if (gameMode == VERSUS) { // NEW: Blocker for people in versus mode
				if (currentFloor == floorsToPlay) {
					activeHero->setLuck(0);
					idleHero->setLuck(0);
					sequence = VS_BATTLE;
					break;
				}
				else {
					sequence = VS_BONUS;
					break;
				}
			}

			enemiesToSpawn = remaining = 1; // Set the first enemy to a boss
			// Setting up boss stats
			if (currentFloor < 40) { // If you haven't made it to Floor 40, the boss won't be Bowser.
				hoard[0].setBossName(currentFloor / 10 - 1);
				hoard[0].setHPMax(difficulty * 40);
				hoard[0].setAttack(4 * (currentFloor / 10));
				hoard[0].setDefense(2 * (currentFloor / 10 - 1));

				hoard[0].setMinEXP(100 * (currentFloor / 10));
				hoard[0].setMaxEXP(100 * (currentFloor / 10));
				hoard[0].setMinLoot(100 * (currentFloor / 10));
				hoard[0].setMaxLoot(100 * (currentFloor / 10));
			} // NEW: There's a new boss waiting for you at the 100th floor
			else if (currentFloor >= 40 && currentFloor < 100) { // These are Bowser's stats
				hoard[0].setBossName(3);
				hoard[0].setHPMax(300);
				hoard[0].scaleHP();
				hoard[0].setAttack(20);
				hoard[0].setDefense(8);

				hoard[0].setMinEXP(0);
				hoard[0].setMaxEXP(0);
				hoard[0].setMinLoot(500);
				hoard[0].setMaxLoot(500);
			}
			else if (ruleSet) { // These are Dry Bowser's stats
				hoard[0].setBossName(4);
				hoard[0].setHPMax(1000);
				hoard[0].scaleHP();
				hoard[0].setAttack(40);
				hoard[0].setDefense(12);

				hoard[0].setMinEXP(0);
				hoard[0].setMaxEXP(0);
				hoard[0].setMinLoot(2500);
				hoard[0].setMaxLoot(2500);
			}
			// Output
			hoard[0].setHP(hoard[0].getHPMax()); // Setting up boss HP
			setColor(8);
			cout << "The room is dark... overhead, you see " << hoard[0].getName() << " looming over you.\n";
			cout << "Prepare yourself!\n";
			hoard[0].setType(5);

			sequence = YOUR_TURN;
			break;
		case VS_BONUS: // Give a random item to the player with the most coins
			switchTurn();
			activeHero->setVsComplete(true);
			idleHero->setVsComplete(true);
			
			setColor(14);
			moneyReward = rngesus(0, 4); // The variable has a double meaning! |:)
			cout << "Checkpoint! ";
			// Checking who has more coins
			if (hero.getCoins() > side.getCoins()) { // If it's Player 1
				cout << "Player 1 has ";
				heroInv.addAnItem(moneyReward);
			}
			else if (side.getCoins() > hero.getCoins()) { // If it's Player 2
				cout << "Player 2 has ";
				sideInv.addAnItem(moneyReward);
			}
			else { // If it's a tie
				cout << "Both players are tied for ";
				heroInv.addAnItem(moneyReward);
				sideInv.addAnItem(moneyReward);
			}

			cout << "the money lead, so they get a " << activeInv->getItem(moneyReward) << "!\n";

			ascend();
			sequence = ROOM;
			break;
		case VS_BATTLE: // Sets up the fight sequence so now you fight your opponent
			setColor(8);
			cout << "SUDDEN DEATH! It's all come down to this. You've cleared out the arena from\n"
				<< "bottom to top, and who remains is no one but each other. Only one can wear the crown!\n";

			switchTurn();
			fightSequence = VS_ATTACK;
			sequence = YOUR_TURN;
			break;
		case YOUR_TURN: // Determines what you want to do
			switch (gameMode) {
			case COOP: // NEW: Dialogue for turn order and setting team value
				teamCost = activeHero->getAttack() / 2;
				setColor(11);
				cout << "Player " << activeHero->getPlayerNum() << ", you're "
					<< (idleHero->canTakeTurn() ? "in the front.\n" : (idleHero->isAlive() ? "in the back.\n" : "the only one left.\n"));
				break;
			case VERSUS:
				setColor(11);
				cout << "Player " << activeHero->getPlayerNum() << ":\n";
				break;
			}
			setColor(15);
			cout << "--------------------------------------\n";
			cout << "1. Jump\n2. Hammer\n3. Power: " << activeHero->getPower() << " (" << activeHero->getUses() << ")\n4. Item\n5. View stats\n";
			
			if (gameMode == COOP) {
				setColor(14);
				cout << "6. Team Jump -- TP Cost: " << teamCost
				<< "\n7. Team Hammer -- TP Cost: " << teamCost
					<< "\n8. Switch position\n";
			}
			
			setColor(15);
			getInput(gameMode == COOP ? 8 : 5);
			// Checking for move
			switch (response) {
			case 1: // Jump
			case 2: // Hammer
				damageScaler = 1;
				attackType = response;
				sequence = fightSequence;
				break;
			case 6: // Team Jump
			case 7: // Team Hammer
				if (activeHero->getTP() >= teamCost && idleHero->isAlive()) {
					damageScaler = 1.75f;
					activeHero->addToTP(-teamCost); // I didn't forget. Thanks, past me.
					setColor(characterColor[coPlayingAs]);
					cout << "You now have " << activeHero->getTP() << " TP.\n";
					setColor(14);
					cout << "Your partner rushes to your side as together you ready your attack.\n";
					attackType = response - 5;
					sequence = WHO_TO_ATTACK;
				}
				else if (!idleHero->isAlive()) { // You need both players for that to work
					setColor(7);
					cout << "Your partner is dead.\n";
				}
				else {
					setColor(7);
					cout << "You don't have enough Team Points.\n";
				}
				break;
			case 3: // Powerup
				if (activeHero->getPowerNum() <= 0) {
					cout << "You currently do not have a powerup equipped.\n";
					sequence = YOUR_TURN; // Ensuring you don't skip a turn for a mistake
					break;
				}
				else {
					attackType = 2 + activeHero->getPowerNum();
				}
				sequence = fightSequence;
				break;
			case 4: // Take inventory
				prevSequence = YOUR_TURN;
				sequence = INVENTORY;
				break;
			case 5: // View stats
				setColor(15);
				activeHero->print();
				break;
			case 8: // Switching turn order
				if (idleHero->canTakeTurn()) {
					switchTurn();
				}
				else {
					setColor(7);
					cout << "Your partner is unavailable.\n";
				}
				break;
			}
			break;
		case WHO_TO_ATTACK: // Choosing which enemy you wish to attack and executing an action
			setColor(15);
			damage = 0;
			connects = false;

			if (enemiesToSpawn > 1) { // If there's more than one enemy
				cout << "0. Go back\n";
				for (POS i = 0; i < enemiesToSpawn; i++) {
					if (hoard[i].isAlive()) {
						cout << i + 1 << ". " << hoard[i].getName() << endl;
					}
				}
				getInput(enemiesToSpawn);

				if (response == 0) {
					sequence = YOUR_TURN;
					break;
				}
				// Prevents attacking already dead enemies and wasting turns
				if (!hoard[response - 1].isAlive()) {
					cout << "That enemy is already dead.\n";
					break;
				}
			}
			else if (enemiesToSpawn == 1 || attackType == 4) { // NEW: This piece of code was edited to be cleaner
				response = 1; // Skip choosing entirely
			}
			// Attacking
			switch (attackType) { // Decide outcomes based on whether you jumped or hammered
			case 1: // If you jump on an enemy
				if (hoard[response - 1].getType() != 1 && hoard[response - 1].getType() != 3) {
					damage = (activeHero->getAttack() - hoard[response - 1].getDefense()) * -1;
					damage *= damageScaler; // NEW: Now supports team attacks
					damage = (damage > 0 ? 0 : damage); // If the damage dealt is any less than 0, don't bother
					damage *= activeHero->criticalRoll(rngesus(0, 100)); // NEW: Gives a chance for a critical hit
					hoard[response - 1].addToHP(damage);
					connects = true;
				} // But this is all only if you use the right attack
				else { // If the enemy is spiky or made of fire
					activeHero->addToHP(-1); // Take one damage for jumping 
					setColor(2);
					cout << "Ouch! That look like it hurt.\n";
					setColor(characterColor[playingAs]);
					cout << "You now have " << activeHero->getHP() << " HP.\n";
				}
				break;
			case 2: // If you use the hammer
				if (hoard[response - 1].getType() != 2 && hoard[response - 1].getType() != 4) {
					damage = (activeHero->getAttack() - hoard[response - 1].getDefense()) * -1;
					damage *= damageScaler;
					damage = (damage > 0 ? 0 : damage);
					damage *= activeHero->criticalRoll(rngesus(0, 100));
					hoard[response - 1].addToHP(damage);
					connects = true;
				}
				else { // If the enemy is airborne at all 
					setColor(2);
					cout << "The hammer didn't reach its designated target.\n";
					setColor(characterColor[playingAs]);
					cout << "You now have " << activeHero->getHP() << " HP.\n";
				}
				break;
			case 3: // If you use fire
				damage = (activeHero->getAttack() - hoard[response - 1].getDefense()) * -1;
				damage = (damage > 0 ? 0 : damage);
				hoard[response - 1].addToHP(damage * 4);
				setColor(14);
				cout << "WOW! What a strong shot!\n";
				activeHero->deductUse();
				connects = true;
				break;
			case 4: // If you swipe your raccoon tail
				damage = (activeHero->getAttack() - hoard[response - 1].getDefense()) * -1;
				damage = (damage > 0 ? 0 : damage);
				// The idea you have an enemy you select in the scattershot to take double damage came from a bug.
				hoard[response - 1].addToHP(damage);
				for (POS i = 0; i < 4; i++) {
					hoard[i].addToHP(damage);
				}
				// Output
				setColor(14);
				cout << "Your tail did a scattershot on all the enemies.\n";
				activeHero->deductUse();
				connects = true;
				break;
			case 5: // If you use ice
				damage = (activeHero->getAttack() - hoard[response - 1].getDefense()) * -1;
				// Detecting if the enemy is fire, and if so, triples the Ice Flower's damage
				damage = (hoard[response - 1].getType() == 3 ? damage * 5 : damage);
				damage = (damage > 0 ? 0 : damage);
				hoard[response - 1].addToHP(damage);
				hoard[response - 1].addToAttack(-2);
				hoard[response - 1].addToDefense(-1);
				// Output
				setColor(14);
				cout << "The ice lowered your enemy's attack and defense.\n";
				activeHero->deductUse();
				connects = true;
				break;
			}
			// Despite the confusing function name, this subtracts HP from the enemy
			setColor(15);
			if (attackType <= 2) {
				if (damage < 0 && connects) {
					cout << "You attack the " << hoard[response - 1].getName() << " for " << -damage << " damage.\n";
				}
				else {
					cout << "The " << hoard[response - 1].getName() << " didn't take any damage.\n";
				}
			}
			else {
				if (activeHero->getUses() == 0) {
					setColor(characterColor[playingAs]);
					cout << "Your powers have run out, and you have returned to normal.\n";
				}
			}
			// Check if enemies are dead
			remaining = 0;
			for (POS i = 0; i < 4; i++) {
				if (hoard[i].isAlive()) { // Loop through all enemies to check which are alive to tally
					remaining++;
				}
			}
			// Setting the player turns to false
			activeHero->setTurn(false);
			// Checking which sequence to switch to
			if (remaining > 0) {
				if (idleHero->canTakeTurn()) {
					sequence = YOUR_TURN;
				}
				else {
					sequence = ENEMY_TURN;
				}
				// Switching turns
				if (gameMode == COOP && idleHero->isAlive()) {
					switchTurn();
				}
			}
			else {
				sequence = VICTORY;
			}
			break;
		case VS_ATTACK: // WIP
			activeHero->setTurn(true);
			idleHero->setTurn(true);
			
			damage = (activeHero->getAttack() - idleHero->getDefense()) / 2;
			damage = (damage < 1 ? 1 : damage);
			// Attack types
			setColor(14);
			switch (attackType) {
			case 3: // Fire
				damage *= 2;
				cout << "OOH! That's gonna leave a mark!\n";
				activeHero->deductUse();
				break;
			case 4: // Racoon
				damage *= 1.5;
				cout << "Your tail hit your opponent hard!\n";
				activeHero->deductUse();
				break;
			case 5: // Ice
				idleHero->addToAttack(-1);
				idleHero->addToDefense(-1);
				cout << "The ice lowered your opponent's attack and defense!\n";
				activeHero->deductUse();
				break;
			}

			idleHero->addToHP(-damage);
			cout << "You attack your opponent for " << damage << " damage.\n";
			setColor(3);
			cout << "Player " << idleHero->getPlayerNum() << ", you now have " << idleHero->getHP() << " HP.\n";
			turns++;
			// Raising the stakes
			if (turns % 2 == 0) {
				activeHero->addToAttack(1);
				idleHero->addToAttack(1);
			}

			if (activeHero->getHP() <= 0 || idleHero->getHP() <= 0) {
				sequence = VS_WIN;
			}
			else {
				if (response != 0) {
					switchTurn();
				}
				sequence = YOUR_TURN;
			}
			break;
		case INVENTORY: // Brings you to where you can use items
			if (gameMode == VERSUS) { // NEW: Blocker measure to keep the correct inventory loadouts
				sequence = VS_INV;
				break;
			}

			setColor(15);
			cout << "Which item will you use? (0 to go back):\n";
			for (POS i = 0; i < 8; i++) {
				cout << i + 1 << ". " << inv.getItem(i) << " (" << inv.getNumItems(i) << ")\n";
			}

			getInput(8);
			itemChoice = response;

			if (response == 0) {
				sequence = prevSequence;
			}
			else {
				if (inv.getNumItems(response - 1) > 0) { // Leave this line unchanged for updates
					// Please never ask me how long it took to get Mushrooms to work properly.
					setColor(14);
					if (prevSequence != PREPARATION || (prevSequence == PREPARATION && inv.isUsableInPrep(response))) {
						sequence = PERSON;
					}
					else {
						setColor(7);
						cout << "This item cannot be used before preparation.\n";
					}
				}
				else {
					setColor(7);
					cout << "You don't have any of those.\n";
					cout << "----------------------------\n";
				}
			}

			break;
		case VS_INV: // Uses items in versus mode
			setColor(15);
			cout << "Which item will you use? (0 to go back):\n";
			for (POS i = 0; i < 5; i++) {
				cout << i + 1 << ". " << activeInv->getItem(i) << " (" << activeInv->getNumItems(i) << ")\n";
			}

			getInput(5);

			if (response == 0) {
				sequence = prevSequence;
			}
			else {
				if (activeInv->getNumItems(response - 1) > 0) { // Leave this line unchanged for updates
					setColor(14);
					switch (response) { // Switching values to make them correspond correctly to actions
					case 1:
						itemChoice = response;
						itemAction();
						activeInv->consumeItem(0);
						break;
					case 5:
						activeInv->steal(activeInv == &heroInv ? sideInv : heroInv, rngesus(0, 4));
						activeInv->consumeItem(4);
						break;
					default:
						itemChoice = response + 1;
						itemAction();
						activeInv->consumeItem(itemChoice - 2);
					}
					activeHero->setTurn(false);
				}
				else {
					setColor(7);
					cout << "You don't have any of those.\n";
					cout << "----------------------------\n";
					continue;
				}
			}

			switch (prevSequence) { // Switching back
			case YOUR_TURN:
				if (activeHero->canTakeTurn()) {
					sequence = YOUR_TURN;
				}
				else {
					if (fightSequence == VS_ATTACK) {
						if (response != 0) {
							switchTurn();
						}
						sequence = YOUR_TURN;
					}
					else {
						sequence = ENEMY_TURN;
					}
				}
				break;
			case PREPARATION:
				sequence = PREPARATION;
			}
			break;
		case PERSON: // NEW: A brand new item sequence that works for co-op
			if (gameMode == COOP && inv.isUsableInPrep(response)) {
				setColor(15);
				cout << "To whom will you give it?\n1. You (Player " << activeHero->getPlayerNum() << ")\n" <<
					"2. Your partner (Player " << idleHero->getPlayerNum() << ")\n";
				getInput(2);
				// Checks who to give the item to
				switch (response) {
				case 1:
					setColor(14);
					cout << "Player " << activeHero->getPlayerNum() << ":\n";
					itemAction();
					break;
				case 2: // Clever workaround for partner items since nothing else worked
					switchTurn();
					setColor(14);
					cout << "Player " << activeHero->getPlayerNum() << ":\n";
					itemAction();
					switchTurn();
					break;
				}
			}
			else {
				itemAction();
			}
			// End by consuming the item the player used
			inv.consumeItem(itemChoice - 1); // Leave this line unchanged for updates
			// NEW: Check if enemies are dead after using an item
			remaining = 0;
			for (POS i = 0; i < 4; i++) {
				if (hoard[i].isAlive()) { // Loop through all enemies to check which are alive to tally
					remaining++;
				}
			}

			switch (prevSequence) {
			case YOUR_TURN:
				activeHero->setTurn(false);
				
				if (remaining > 0) { // UPDATED: Now checks for victory after item usage
					if (idleHero->canTakeTurn()) {
						sequence = YOUR_TURN;
					}
					else {
						sequence = ENEMY_TURN;
					}
					
					if (gameMode == COOP && idleHero->isAlive()) {
						switchTurn();
					}
				}
				else {
					sequence = VICTORY;
				}
				break;
			case PREPARATION:
				sequence = PREPARATION;
			}

			break;
		case ENEMY_TURN: // When the enemy attacks you
			for (POS i = 0; i < enemiesToSpawn; i++) {
				if (hoard[i].isAlive()) {
					damage = (hoard[i].getAttack() - activeHero->getDefense()) * -1;
					damage = (damage > 0 ? 0 : damage);
					setColor(7);

					activeHero->addToHP(damage);
					if (damage < 0) {
						cout << "The " << hoard[i].getName() << " strikes you for " << -damage << " damage.\n";
					}
					else {
						switch (rngesus(1, 5)) {
						case 1: // NEW: I reduced the chances of you blocking.
							cout << "You blocked the attack from the " << hoard[i].getName() << "!\n";
							break;
						default:
							activeHero->addToHP(-1);
							cout << "The " << hoard[i].getName() << " barely chips you for 1 damage.\n";
						}
					}
					setColor(characterColor[playingAs]);
					if (gameMode > SOLO) {
						cout << "Player " << activeHero->getPlayerNum() << ":\n";
					}

					cout << "You now have " << activeHero->getHP() << " HP.\n";
					// Check if it's time to buff enemies
					if (turns % 2 == 0 && enemyBuffs) {
						if (activeHero->getDefense() <= hoard[i].getAttack()) {
							hoard[i].addToAttack(currentFloor == 100 ? 3 : 1);
						} // If the hero has less defense than an enemy can attack
						else { // Or else set enemies to always hit for 2 damage
							hoard[i].setAttack(activeHero->getDefense() + 2);
						}
					}
				}
			}
			// NEW: After a turn, if the player is in a gimmicked stage, determine how to hurt them
			switch (eventRoll) {
			case 19: // Water
				activeHero->deductAir();
				idleHero->deductAir();
				setColor(2);
				cout << "You now have " << activeHero->getAir() << " Oxygen.\n";

				if (activeHero->getAir() == 0) {
					activeHero->addToHP(activeHero->getAir() - 1);
					idleHero->addToHP(idleHero->getAir() - 1);
					setColor(1);
					cout << "You're drowning, and lost " << -activeHero->getAir() + 1 << " HP in the process.\n";
					if (gameMode == COOP) {
						cout << "Player " << activeHero->getPlayerNum() << ", ";
					}

					cout << "You now have " << activeHero->getHP() << " HP.\n";

					if (gameMode == COOP) {
						cout << "Player " << idleHero->getPlayerNum() << ", " <<
							"You now have " << idleHero->getHP() << "HP.\n";
					}
				}

				break;
			case 20: // Cave
				activeHero->addToHP(difficulty * -1);
				setColor(2);
				cout << "The poison took away " << difficulty << " HP.\n";
				if (gameMode == COOP) {
					cout << "Player " << activeHero->getPlayerNum() << ", ";
				}

				cout << "You now have " << activeHero->getHP() << " HP.\n";

				if (gameMode == COOP) {
					cout << "Player " << idleHero->getPlayerNum() << ", " <<
						"You now have " << idleHero->getHP() << "HP.\n";
				}

				break;
			}
			activeHero->setTurn(true);
			if (gameMode == COOP) {
				idleHero->setTurn(true);
			}
			// Check if you are dead
			if (activeHero->isAlive()) {
				turns++;
				sequence = YOUR_TURN;
			}
			else {
				sequence = LOSE;
			}
			break;
		case VICTORY: // The message you see after winning a fight and getting rewards
			if (gameMode == COOP) {
				idleHero->setHP(idleHero->isAlive() ? idleHero->getHP() : 1); // NEW: Set HP to one if dead upon victory
			}
			
			moneyReward = rngesus(hoard[0].getMinLoot(), hoard[0].getMaxLoot());
			setColor(14);
			cout << "You did it!\n\n";
			// Reap the rewards
			for (POS i = 0; i < enemiesToSpawn; i++) { // EXP and Coins across the enemies all for you
				activeHero->addToEXP(rngesus(hoard[i].getMinEXP(), hoard[i].getMaxEXP()));
				activeHero->addToEXP(hoard[i].getType() == 4 ? 30 : 0);
				activeHero->addToCoins(moneyReward);
				if (gameMode == COOP) {
					idleHero->addToCoins(moneyReward);
				}
			}
			// Check if you beat Bowser and finished the game
			if (currentFloor == 40 || currentFloor == 100) {
				sequence = WIN;
				break;
			}
			// The name of the function says it all
			if (activeHero->checkForLevelUp() || idleHero->checkForLevelUp()) {
				sequence = LEVEL;
			}
			else {
				if (currentFloor % 10 == 0 && gameMode < VERSUS) {
					sequence = UPGRADE;
				}
				else {
					ascend();
					sequence = PREPARATION;
				}
			}

			break;
		case PREPARATION: // Allows the player to use items before climbing a floor
			setColor(15);
			cout << "1. Keep going up\n2. Prepare for battle\n";
			getInput(2);

			switch (response) {
			case 1:
				eventRoll = (ruleSet && gameMode < VERSUS ? rngesus(1, 20) : 1); // Roll a new event
				// Event stuff
				if (eventRoll < 17) { // continue as normal
					sequence = ENEMY;
					if (gameMode == VERSUS && currentFloor % 5 != 0) { // NEW: Switch turns for versus players
						switchTurn();
						sequence = ROOM;
					}
				}
				else {
					sequence = EVENT; // 17 18: ?-Block, 19, 20: Pipe
				}
				break;
			case 2:
				prevSequence = PREPARATION;
				sequence = INVENTORY;
				break;
			}

			break;
		case LEVEL: // NEW: Now checks for either hero's level
			if (!activeHero->checkForLevelUp()) {
				switchTurn();
			}
			// NEW: Checks which attributes to give to which hero
			if (activeHero == &hero) {
				leveling = playingAs;
			}
			else {
				leveling = coPlayingAs;
			}

			if (gameMode > SOLO) {
				setColor(14);
				cout << "Player " << activeHero->getPlayerNum() << ":\n";
			}
			// Fix Player 2's upgrade scheme behaving like Player 1's.
			cout << "\nLEVEL UP!!\n"; // Leveling up Max HP, Attack, then Defense respectively
			activeHero->levelUp(upgradeScheme[leveling][0], upgradeScheme[leveling][1], upgradeScheme[leveling][2],
				upgradeScheme[leveling][3], upgradeScheme[leveling][4]);
			// Printing
			cout << "Your Max HP is now " << activeHero->getHPMax() << ", and you got a full restore\n";
			cout << "Your Attack power is now " << activeHero->getAttack() << endl;
			cout << "Your Defense power is now " << activeHero->getDefense() << endl;
			cout << "Your Max TP is now " << activeHero->getTPMax() << ", and you got a full restore\n";
			cout << "Your Luck is now " << activeHero->getLuck() << endl;
			cout << "You can upgrade one of these again!\n\n";
			/*
			* They say to use static cast<var> is to prevent people from casting so much.
			* I say SCREW THAT! If the old way is still possible, I'm sticking to that for
			* the sake of doing things fast.
			*/
			setColor(characterColor[leveling]);
			cout << "1. Upgrade Max HP by " << (POS)(difficulty - 1) / 2 + 1 <<
				"\n2. Upgrade Attack by " << (POS)(difficulty - 1) / 2 + 1 <<
				"\n3. Upgrade Defense by " << (POS)(difficulty - 1) / 2 + 1 << 
				"\n4. Upgrade Max TP by " << (POS)(difficulty - 1) / 2 + 1 <<
				"\n5. Upgrade Luck by " << (POS)(difficulty - 1) / 2 + 1 << endl;
			getInput(5);

			setColor(14);
			switch (response) {
			case 1:
				activeHero->addToHPMax((POS)(difficulty - 1) / 2 + 1);
				activeHero->setHP(activeHero->getHPMax());
				cout << "You now have " << activeHero->getHPMax() << " Max HP.\n";
				break;
			case 2:
				activeHero->addToAttack((POS)(difficulty - 1) / 2 + 1);
				cout << "You now have " << activeHero->getAttack() << " Attack.\n";
				break;
			case 3:
				activeHero->addToDefense((POS)(difficulty - 1) / 2 + 1);
				cout << "You now have " << activeHero->getDefense() << " Defense.\n";
				break;
			case 4:
				activeHero->addToTPMax((POS)(difficulty - 1) / 2 + 1);
				cout << "You now have " << activeHero->getTPMax() << " Max TP.\n";
				break;
			case 5:
				activeHero->addToLuck((POS)(difficulty - 1) / 2 + 1);
				cout << "You now have " << activeHero->getLuck() << " Luck.\n";
				break;
			}
			// Checking if the game has to repeat the sequence from a multi level-up
			activeHero->updateEXPToNext();

			if (!activeHero->checkForLevelUp() && !idleHero->checkForLevelUp()) {
				if (currentFloor % 10 == 0 && gameMode < VERSUS) {
					sequence = UPGRADE;
				}
				else {
					ascend();
					sequence = PREPARATION;
				}
			}

			break;
		case SHOP: // You get to buy items here. HOO-ray.
			if (gameMode == VERSUS) { // Setting up a breakpoint for versus mode
				activeHero->setTurn(true);
				activeHero->setVsComplete(true);
				idleHero->setTurn(true);
				idleHero->setVsComplete(true);
				sequence = VS_SHOP;
				break;
			}

			if (inv.getBuyCap() == 0) {
				activeHero->setTurn(false);
				if (idleHero->canTakeTurn()) { // NEW: Grants ability to alternate
					inv.setBuyCap(difficulty * 2 + 1);
					switchTurn();
					break;
				}
				else {
					ascend();
					sequence = PREPARATION;
					break;
				}
			}
			
			setColor(14);
			// Sets a limit to how many items you can buy
			if (gameMode > SOLO) {
				cout << "Player " << activeHero->getPlayerNum() << ", ";
			}

			cout << "You can buy " << inv.getBuyCap() << " more items.\n";
			setColor(15);
			cout << "0. Stop buying\n";
			// Display items
			for (POS i = 0; i < 9; i++) {
				cout << i + 1 << ". Buy a " << inv.getItem(i) << " (" << inv.getPrice(i) << ")\n";
			}

			getInput(9);
			// Checking if the player wants to leave
			if (response == 0) {
				activeHero->setTurn(false);
				if (idleHero->canTakeTurn()) { // NEW: Grants ability to alternate
					inv.setBuyCap(difficulty * 2 + 1);
					switchTurn();
					break;
				}
				else {
					ascend();
					sequence = PREPARATION;
					break;
				}
			}
			// Check if the player has enough coins
			if (activeHero->getCoins() >= inv.getPrice(response - 1)) { // If you have enough
				inv.addAnItem(response - 1); // Inserts bought item into inventory
				cout << "You bought the " << inv.getItem(response - 1) << ".\n";
				activeHero->addToCoins(inv.getPrice(response - 1) * -1); // Spend the coins
				cout << "You now have " << activeHero->getCoins() << " coins.\n";
				// Limit the amount of items bought in one go
				inv.deductStock();
			}
			else {
				setColor(7);
				cout << "Looks like you're a few coins short.\n";
				cout << "------------------------------------\n";
			}

			break;
		case VS_SHOP: // Shopping at Archrival Arena
			if (activeInv->getBuyCap() == 0) {
				activeHero->setTurn(false);
				if (idleHero->canTakeTurn()) {
					switchTurn();
					activeInv->setBuyCap(5);
					break;
				}
				else {
					switchTurn();
					ascend();
					sequence = ROOM;
					break;
				}
			}

			setColor(14);
			// Sets a limit to how many items you can buy
			cout << "Player " << activeHero->getPlayerNum() << ", ";
			cout << "You can buy " << activeInv->getBuyCap() << " more items.\n";
			setColor(15);
			cout << "0. Stop buying\n";
			// Display items
			for (POS i = 0; i < 5; i++) {
				cout << i + 1 << ". Buy a " << activeInv->getItem(i) << " (" << activeInv->getPrice(i) << ")\n";
			}

			getInput(5);
			// Checking if the player wants to leave
			if (response == 0) {
				activeHero->setTurn(false);
				if (idleHero->canTakeTurn()) {
					switchTurn();
					activeInv->setBuyCap(5);
					break;
				}
				else {
					switchTurn();
					ascend();
					sequence = ROOM;
					break;
				}
			}
			// Check if the player has enough coins
			if (activeHero->getCoins() >= activeInv->getPrice(response - 1)) { // If you have enough
				activeInv->addAnItem(response - 1); // Inserts bought item into inventory
				cout << "You bought the " << activeInv->getItem(response - 1) << ".\n";
				activeHero->addToCoins(activeInv->getPrice(response - 1) * -1); // Spend the coins
				cout << "You now have " << activeHero->getCoins() << " coins.\n";
				// Limit the amount of items bought in one go
				activeInv->deductStock();
			}
			else { // And if you can't afford the item
				setColor(7);
				cout << "Looks like you're a few coins short.\n";
				cout << "------------------------------------\n";
			}
			break;
		case UPGRADE: // Every 10 floors, you get the opportunity to upgrade a stat
			setColor(11);
			cout << "Upgrade time! Choose which one of these upgrades you want to take.\n";
			// All the different items
			setColor(15);
			cout << "1. " << inv.getSpecialItem(0) << ": Increases the rate of Max HP upgrades each level\n" <<
				"2. " << inv.getSpecialItem(1) << ": Increases the rate of Attack upgrades each level\n" <<
				"3. " << inv.getSpecialItem(2) << ": Increases the rate of Defense upgrades each level\n" <<
				"4. " << inv.getSpecialItem(3) << ": Increases the rate of Max TP upgrades each level\n" <<
				"5. " << inv.getSpecialItem(4) << ": Increases the rate of Luck upgrades each level\n"
				"6. " << inv.getSpecialItem(5) << ": Increases the effectiveness of items for higher heals and more uses\n";
			// Maybe I'll add in more when I get to coding events | EDIT: Turns out I didn't
			getInput(6);
			// Upgrades after being chosen
			switch (response) {
			case 6:
				inv.upEffectiveness();
				break;
			default:
				upgradeScheme[playingAs][response - 1]++;
				upgradeScheme[coPlayingAs][response - 1]++;
				break;
			}

			setColor(15);
			cout << "You chose the " << inv.getSpecialItem(response - 1) << ".\n";
			// Inflate item prices
			if (currentFloor % 20 == 0) {
				difficulty++;
				inv.jackUpPrice();
			}
			// Adding in more enemies
			switch (currentFloor) {
			case 10:
				enemyTypeRange = 2;
				break;
			case 20:
				enemyTypeRange = 3;
				break;
			case 30:
				enemyTypeRange = 4;
				break;
			}

			ascend();
			sequence = PREPARATION;
			break;
		case WIN: // When the player beats the game
			setColor(11);
			cout << "\n=================\n";
			cout << "= GAME COMPLETE =\n";
			cout << "=================\n\n";
			// Tallying the score
			setColor(15);
			// How many floors you climbed
			for (POS i = 10; i < ((highestFloor / 10) * 10 + 1); i += 10) {
				score += 1000 + ((i - 10) * 50);
				cout << "Floor " << i << " Bonus: " << 1000 + ((i - 10) * 50) << endl;
			}
			// What your level was at the end
			score += 1000 * activeHero->getLevel();
			cout << "Level Bonus: " << 1000 * activeHero->getLevel() << endl;
			// Most importantly, the coins
			score += activeHero->getCoins() * 100;
			cout << "Coin Bonus: " << activeHero->getCoins() * 100 << endl;
			// Which all culminates to your grand total
			setColor(13);
			cout << "Grand total: " << score << endl;
			ending(); // Display the ending
			
			switch (currentFloor) {
			case 100:
				if (ruleSet) { // NEW: The secret message now unlocks after beating hard mode
					cout << "\nRemember, Mr. Dobervich says to always " << DOBERVICH_BUMPER_STICKER << endl;
					break;
				}
			default:
				cout << "There exists yet a greater threat on the 100th floor. Switch to the\n"
					<< "Advanced Rule Set to take him on! Think you can?\n";
			}
			// NEW: Saving functionality
			if (score > file.getHighScore()) {
				file.saveHighScore(score);
				cout << "High score was saved successfully!\n";
			}

			sequence = TITLE;
			break;
		case LOSE: // What the player comes across each time they die
			if (gameMode == VERSUS) {
				sequence = VS_WIN;
				break;
			}
			// Checks if you have a 1UP
			if (inv.getNumItems(8) > 0) {
				activeHero->setHP(activeHero->getHPMax());
				inv.consumeItem(8); // Note to self: Don't leave debug mode on for other players.
				setColor(1);
				cout << "But what's this...? A 1UP Mushroom. You're saved!\n";
				cout << "You now have " << inv.getNumItems(8) << " 1UP Mushrooms.\n";
				sequence = YOUR_TURN;
				break;
			}

			if (idleHero->isAlive()) { // If the other hero is still alive,
				switchTurn();
				sequence = YOUR_TURN; // Switch back
			}
			else { // Or else it's game over.
				setColor(4);
				cout << "\n=============\n";
				cout << "= GAME OVER =\n";
				cout << "=============\n\n";

				setColor(15);
				cout << "You traveled up ";
				setColor(5);
				cout << highestFloor << " floors before biting the dust.\n\n";

				sequence = TITLE;
			}
			break;
		case VS_WIN: // WDetermines the winner in versus mode
			setColor(11);
			cout << "===========================\n";
			if (hero.isAlive()) { // If Player one was left standing
				cout << "= Player 1 is the winner! =\n";
			}
			else if (side.isAlive()) { // If Player two was left standing
				cout << "= Player 2 is the winner! =\n";
			}
			cout << "===========================\n\n";

			sequence = TITLE;
			break;
		default: // Exit the loop and therefore the game
			running = false;
		}
		// Checks if the player has quit to update accordingly
		if (hasQuit()) {
			sequence = 32768;
		}
	}

	goodbye();
	return 0;
}
/* 
* Changes text color to:
* 0: Black | 1: Navy Blue | 2: Forest Green | 3: Aqua
* 4: Maroon | 5: Dark Purple | 6: Gold | 7: Light Gray
* 8: Dark Gray | 9: Cobalt Blue | 10: Green | 11: Cyan
* 12: Red | 13: Purple | 14: Tan | 15: White
*/
void setColor(POS color) {
	SetConsoleTextAttribute(h, color);
}
// Initializes each upgrade factor
Hero initialize(Hero h) {
	score = 0;
	enemyTypeRange = 0;
	difficulty = 1;
	// Mario
	upgradeScheme[MARIO][0] = 1;
	upgradeScheme[MARIO][1] = 2;
	upgradeScheme[MARIO][2] = 0;
	upgradeScheme[MARIO][3] = 2;
	upgradeScheme[MARIO][4] = 0;
	// Luigi
	upgradeScheme[LUIGI][0] = 0;
	upgradeScheme[LUIGI][1] = 1;
	upgradeScheme[LUIGI][2] = 2;
	upgradeScheme[LUIGI][3] = 1;
	upgradeScheme[LUIGI][4] = 1;
	// Toad
	upgradeScheme[TOAD][0] = 2;
	upgradeScheme[TOAD][1] = 0;
	upgradeScheme[TOAD][2] = 1;
	upgradeScheme[TOAD][3] = 0;
	upgradeScheme[TOAD][4] = 2;
	// Toadette
	upgradeScheme[TOADETTE][0] = 2;
	upgradeScheme[TOADETTE][1] = 0;
	upgradeScheme[TOADETTE][2] = 1;
	upgradeScheme[TOADETTE][3] = 2;
	upgradeScheme[TOADETTE][4] = 1;
	// Yoshi
	upgradeScheme[YOSHI][0] = 1;
	upgradeScheme[YOSHI][1] = 0;
	upgradeScheme[YOSHI][2] = 2;
	upgradeScheme[YOSHI][3] = 0;
	upgradeScheme[YOSHI][4] = 2;
	// Spek
	for (POS i = 0; i < 5; i++) {
		upgradeScheme[SPEK][i] = 1;
	}
	// Enemies
	for (POS i = 0; i < 4; i++) {
		hoard[i].resetStats(0, 0, 0);
	}
	// You
	h.resetStats(10, 5, 1, 8, 2);
	h.setAir(2);
	inv.resetStock();
	heroInv.resetStock();
	sideInv.resetStock();
	h.setVsComplete(false);
	h.bankruptcy();
	eventRoll = 1;

	return h;
}
// What the player first sees before they begin
void welcome() {
	setColor(6);
	cout << "===============================\n";
	cout << "= Welcome to Bowser's Tower!  =\n";
	cout << "= Enter if you dare... V2.0.0 =\n";
	cout << "===============================\n\n";
	setColor(15);
}
// What the player sees when they quit
void goodbye() {
	setColor(6);
	cout << "\n============\n";
	cout << "= Goodbye. =\n";
	cout << "============\n";
	setColor(15);
}
// How did Bowser get a tower? Find out here!
void story() {
	setColor(4);
	cout << "////////////////////////////////////////////////////////////////////\n";
	cout << "/ SOLO:                                                            /\n";
	cout << "/ After the events of Mario Clash, the Mushroom Kingdom's people   /\n";
	cout << "/ had wondered how Bowser had even gotten a tower to begin with.   /\n";
	cout << "/ Meanwhile, the Mario Crew was completely unaware Bowser had been /\n";
	cout << "/ using a magical power orb to bend to all his evil desires on a   /\n";
	cout << "/ whim, drawing magic straight out of Star Road. Bowser may have   /\n";
	cout << "/ been down for the count that day, but the orb still thrived...   /\n";
	cout << "/ After Mario bested him at Clash Tower, Bowser had had enough,    /\n";
	cout << "/ and now weilds the dark magic of the orb once more to re-enforce /\n";
	cout << "/ what used to be Clash Tower, into Trial Tower, where minions are /\n";
	cout << "/ multiplying at an alarming rate! Toad notices the tower looming  /\n";
	cout << "/ way above the sky, and wastes no time alerting Mario and Luigi.  /\n";
	cout <<	"/ Obviously none of them can accept the fact Bowser has an unruly  /\n";
	cout << "/ amount of power. Upon reaching the enterance after a long climb  /\n";
	cout << "/ to reach World 7 they reach a forboding sign daring them to try. /\n";
	cout << "/                                                                  /\n";
	cout << "/ CO-OP:                                                           /\n";
	cout << "/ The Mario Crew had failed to subdue Bowser's Tower and all the   /\n";
	cout << "/ minions that laid dormant inside. The minions found their dead   /\n";
	cout << "/ bodies, and hurled them out as hard as they could, so hard. the  /\n";
	cout << "/ heroes woke up upon regeneration to find they crash landed right /\n";
	cout << "/ back in Peach's Castle. They enlisted the help of a couple of    /\n";
	cout << "/ friends to help them after they had accidentally bought Bowser   /\n";
	cout << "/ enough time to strengthen the power of his orb, and minions with /\n";
	cout << "/ it. They all went back to Trial Tower, that was now remodeled by /\n";
	cout << "/ by Bowser into the much more ominous... Treachery Tower!         /\n";
	cout << "/                                                                  /\n";
	cout << "/ VERSUS:                                                          /\n";
	cout << "/ After relinquishing the orb's energy, Peach offered up a reward  /\n";
	cout << "/ to the one mainly responsible for ridding the Mushroom Kingdom   /\n";
	cout << "/ of the orb's vileness of 1 million coins, a lifetime supply of   /\n";
	cout << "/ food stamps, and best of all, the title of Superstar. This hefty /\n";
	cout << "/ reward prompted the excitement of all the heroes, and as you may /\n";
	cout << "/ have guessed, no one could agree on who. The Toads voted they    /\n";
	cout << "/ settle this argument on who could prove against each other that  /\n";
	cout << "/ they were stronger, and had what it took to ruin Bowser's day.   /\n";
	cout << "/ Nearby constructor Toads heard the debate, and offered to create /\n";
	cout << "/ fighting grounds out of what little power was left in the orb.   /\n";
	cout << "/ And thus, Archrival Arena was born. Who's the best? Stay gaming  /\n";
	cout << "/ to find out!                                                     /\n";
	cout << "////////////////////////////////////////////////////////////////////\n\n";
}
// What happened to Trial Tower? Find out here!
void ending() {
	setColor(11);
	switch (gameMode) {
	case SOLO:
		cout << "\n////////////////////////////////////////////////////////////////////////////////\n";
		cout << "/ With Bowser defeated, the Mario Crew jump out a nearby window of the tower.  /\n";
		cout << "/ Much of it disappeared without a trace, disintegrating until only the foyer  /\n";
		cout << "/ and a couple of other connected rooms remained. As it turned out, what was   /\n";
		cout << "/ once Trial Tower, aside from Clash Tower was a forgotten land beyond Planet  /\n";
		cout << "/ Mushroom's ozone layers, known as Truffle Towers. Some time after the crew's /\n";
		cout << "/ venture to restore the ancient land, Truffle Towers would soon become a      /\n";
		cout << "/ famous tourist attraction, with most Mushroom Kingdom residents going there  /\n";
		cout << "/ to relax, to migrate, or even to make discoveries of their own about the     /\n";
		cout << "/ Mushroom Kingdom's skylands. And all was peaceful until Waluigi learned the  /\n";
		cout << "/ secrets of what made the Mushroom Kingdom so bouncy and fun... and he sought /\n";
		cout << "/ to have it all to himself. What will happen next when Mario, Luigi and Toad  /\n";
		cout << "/ try to stop his terror? To be continued in Dance Dance Revolution Mario Mix! /\n";
		cout << "////////////////////////////////////////////////////////////////////////////////\n\n";
		break;
	case COOP:
		cout << "\n//////////////////////////////////////////////////////////////////////////////////\n";
		cout << "/ Bowser, against all odds was shown who was Boss in the Mushroom Kingdom. The   /\n";
		cout << "/ tower exploded into a beam of blindingly bright light as all the power the orb /\n";
		cout << "/ was struggling to contain all at once came bursting out in every which way! No /\n";
		cout << "/ evil energy remained, as it morphed back into its original form: A small white /\n";
		cout << "/ dot. Who can say where the orb really came from. Perhaps another dimension.    /\n";
		cout << "/ Nothing of the tower remained except the foyer and a few small connected rooms /\n";
		cout << "/ as it rose high above the sky from whence it came. What was once Treachery     /\n";
		cout << "/ Tower, that was once Trial Tower that was once Clash Tower was Truffle Towers. /\n";
		cout << "/ It was a land above the Mushroom Kingdom's ozone layers containing the secret  /\n";
		cout << "/ to what made the Mushroom Kingdom so bouncy and fun. Most of the Toads who     /\n";
		cout << "/ recently migrated there didn't care much of knowing the secret, but others,    /\n";
		cout << "/ like Waluigi sought to have it all to himself and wreak havoc! Will the heroes /\n";
		cout << "/ be able to stop the guy? To be continued in Dance Dance Revolution Mario Mix!  /\n";
		cout << "/ ...Right after everyone fights over Peach's heroic rewards, haha. Good luck!   /\n";
		cout << "//////////////////////////////////////////////////////////////////////////////////\n\n";
		break;
	}
}
// Displays the instructions
void instructions() {
	setColor(14);
	cout << "////////////////////////////////////////////////////////////////////\n";
	cout << "/ RULES:                                                           /\n";
	cout << "/ The aim of the game is to climb up the tower until you think you /\n";
	cout << "/ can beat Bowser who awaits you at the 40th floor. The more coins /\n";
	cout << "/ you end off with, the higher your score will be. Every 5 floors, /\n";
	cout << "/ you'll stumble across a shop to spend coins on powerups or items /\n";
	cout << "/ to heal yourself. Every 10 floors, you will be given an upgrade  /\n";
	cout << "/ of your choice. These can be a multitude of good things such as  /\n";
	cout << "/ making Mushrooms more effective or making stats increase faster  /\n";
	cout << "/ upon leveling up. Focus on taking out as many enemies as you can /\n";
	cout << "/ to stop Bowser! How high can you get?                            /\n";
	cout << "/ Type anything that isn't a number to quit the game at any time.  /\n";
	cout << "/                                                                  /\n";
	cout << "/ CO-OP:                                                           /\n";
	cout << "/ Enemies' HP have been ballooned to keep in line with being hard. /\n";
	cout << "/ People who thought you could win without trying because you have /\n";
	cout << "/ a gamer by your side, nice try! And you know who you are! Team   /\n";
	cout << "/ moves can now be pulled off for extra damage against enemies. To /\n";
	cout << "/ perform one, it costs the Team Points (TP) of half your attack   /\n";
	cout << "/ power. Turn order can be swapped, and with it, your position     /\n";
	cout << "/ compared to the enemies, which actively is used to manipulate    /\n";
	cout << "/ who the enemies will attack. Teamwork is key in all the chaos!   /\n";
	cout << "/                                                                  /\n";
	cout << "/ VERSUS:                                                          /\n";
	cout << "/ Take turns with your opponent climbing the tower, as you try to  /\n";
	cout << "/ get as strong as you can, before coming face to face with each   /\n";
	cout << "/ other. Choose floors from easy, medium, or hard difficulty for   /\n";
	cout << "/ reward scaling. If you end up with more coins than your opponent /\n";
	cout << "/ after reaching a 10th floor, you'll get a prize to help on your  /\n";
	cout << "/ way to the top. You can use Plunder Chests to steal a random     /\n";
	cout << "/ item from your opponent if you feel like ruining a friendship.   /\n";
	cout << "/                                                                  /\n";
	cout << "/ ITEMS:                                                           /\n";
	cout << "/ Mushrooms heal HP equal to 5 + 5 times however many Orb Essences /\n";
	cout << "/ you have on hand. Other items act as powerups and can be used    /\n";
	cout << "/ 2 times + however many Orb essences you have. Fire Flowers deal  /\n";
	cout << "/ thrice as much damage as a normal attack will on a single enemy. /\n";
	cout << "/ Swiping your tail with a Super Leaf does a group attack, dealing /\n";
	cout << "/ double damage to the enemy of your choice. Ice flowers lower an  /\n";
	cout << "/ enemy's Attack by 2, and Defense by 1, and is 5 times more       /\n";
	cout << "/ effective on fire enemies. It's worth noting powerups don't      /\n";
	cout << "/ trigger an attack, but use a turn to grant you the ability to    /\n";
	cout << "/ use its powers for a set amount of turns.                        /\n";
	cout << "/                                                                  /\n";
	cout << "/ NEW MECHANICS:                                                   /\n";
	cout << "/ Every two turns, normal enemies will increase in attack power.   /\n";
	cout << "/ You are limited to only being able to buy a few items at a time  /\n";
	cout << "/ from shops, regardless of coins. Ocasionally, you'll run across  /\n";
	cout << "/ a set of ?-Blocks you can hit to earn a random item (no, they're /\n";
	cout << "/ not rigged), or pipes taking you to harder floors for greater    /\n";
	cout << "/ rewards. Can either be a toxic cave or underwater. Use a Courage /\n";
	cout << "/ Medal to give 50 * Orb Essences EXP at any time! Level ups only  /\n";
	cout << "/ take effect after winning a battle. Use a Daredevil Medal to     /\n";
	cout << "/ get better rewards off of enemies, but have them hit harder. Use /\n";
	cout << "/ a Star to get out of a rough situation in a pinch, as it clears  /\n";
	cout << "/ all the enemies in a given room! Maple Syrup jars will refill TP /\n";
	cout << "/ at the same rate Mushrooms heal for. If you're playing solo, the /\n";
	cout << "/ Maple Syrup will behave the same as Mushrooms so you don't miss  /\n";
	cout << "/ out.                                                             /\n";
	cout << "/                                                                  /\n";
	cout << "/ This probably goes without saying, but don't jump on spiky and   /\n";
	cout << "/ fire enemies, and don't hammer anyone who can fly.               /\n";
	cout << "////////////////////////////////////////////////////////////////////\n\n";
}
// NEW: Lets the user know of new updates
void patchNotes() {
	setColor(11);
	cout << "-----------------------------------------------------------------------\n";
	cout << "Patch Notes for V2.0.0:\n";
	cout << "-Multiplayer is here! YAAAAY!!! Now includes co-op mode and versus mode\n";
	cout << "-New stats in the form of luck for critical hits and team points\n";
	cout << "-New characters Toadette and Yoshi join the fight in the great ascent\n";
	cout << "-Advanced ruleset was created for gamers looking for something fresh\n";
	cout << "-Pipes and ?-Blocks can appear on floors to shake up the gameplay\n";
	cout << "-New stories to go with new modes\n";
	cout << "-Added in a surprise for players who've beaten the game\n";
	cout << "-General bug fixes\n\n";
	
	cout << "-----------------------------------------------\n";
	cout << "Patch Notes for V1.1.1:\n";
	cout << "-Added in the functionality to save high scores\n\n";

	cout << "---------------------------------------------------------------\n";
	cout << "Patch Notes for V1.1.0:\n";
	cout << "-Added in E. Gadd's lab assistant Spek as a playable character\n";
	cout << "-Made a limit to how many items can be bought at one shop visit\n";
	cout << "-Greatly reduced your odds of blocking attacks from enemies\n";
	cout << "-You can now check your stats at any given time in battle\n";
	cout << "-Added the ability to prepare for battle each floor\n";
	cout << "-Normal enemies now get stronger every so often\n";
	cout << "-General bug fixes\n\n";
}
// A condensed process of climbing the tower and making it so the game properly calculates your score
void ascend() {
	if (gameMode < VERSUS) {
		currentFloor++;
		highestFloor = currentFloor;
	}
	else { // NEW: Swtiches the active player
		activeHero->setVsComplete(true);
		activeHero->setTurn(false);

		if (idleHero->vsFloorIsComplete()) { // If your opponent also beat the floor
			currentFloor++;
			activeHero->setVsComplete(false); // Resetting stats for next floor
			idleHero->setVsComplete(false);
		}
	}
}

void switchTurn() {
	Hero* placeholder;
	// Switcheroo, we can, too!
	placeholder = activeHero;
	activeHero = idleHero;
	idleHero = placeholder;
	// And for the inventory in versus
	activeInv = (activeInv == &heroInv ? &sideInv : &heroInv);
}

void itemAction() {
	// REMEMBER TO CLEAN THIS CODE
	switch (itemChoice) { // NEW: Made to be cleaner
	case 1: // Mushroom
		activeHero->setHealing(true); // Turn on healing
		activeHero->addToHP(inv.getEffectiveness() * 5);
		// Let the player know
		cout << "You restored " << inv.getEffectiveness() * 5 << " HP!\n";
		cout << "You now have " << activeHero->getHP() << " HP.\n";
		// End the healing process to be able to get hit again
		activeHero->setHealing(false);
		break;
	case 2: // Make sure to change for single player mode
		activeHero->setHealing(true); // Turn on healing
		if (gameMode == COOP) { // If the maple syrup can do its job proper in co-op mode
			activeHero->addToTP(inv.getEffectiveness() * 5);

			cout << "You restored " << inv.getEffectiveness() * 5 << " TP!\n";
			cout << "You now have " << activeHero->getTP() << " TP.\n";
		}
		else { // Otherwise, it acts as a mushroom so solo players don't lose out on anything
			activeHero->addToHP(inv.getEffectiveness() * 5);
			// Let the player know
			cout << "You restored " << inv.getEffectiveness() * 5 << " HP!\n";
			cout << "You now have " << activeHero->getHP() << " HP.\n";
			// End the healing process to be able to get hit again
			activeHero->setHealing(false);
		}
		activeHero->setHealing(false);
		break;
	case 3: // Fire Flower
	case 4: // Super Leaf
	case 5: // Ice Flower
		activeHero->setPower(itemChoice - 2);
		activeHero->setUses(inv.getEffectiveness() + 1);
		cout << "You now weild the power of " << activeHero->getPower() << "!\n";
		break;
	case 6: // Courage medal
		activeHero->addToEXP(50 * inv.getEffectiveness());
		setColor(14);
		cout << "You got " << 50 * inv.getEffectiveness() << " EXP!\n";
		break;
	case 7: // Daredevil Medal
		for (POS i = 0; i < 4; i++) {
			hoard[i].multiplyStats(1 + 0.5 * inv.getEffectiveness());
		}
		setColor(4);
		cout << "The enemies have gotten stronger. Beware!!\n";
		break;
	case 8: // Star
		for (POS i = 0; i < 4; i++) {
			hoard[i].setHP(hoard[i].getType() == 5 ? hoard[i].getHP() : 0);
		}
		setColor(14);
		cout << "With a flash of light, the enemies all drop dead.\n";
		if (hoard[0].getType() == 5) {
			cout << "...Is what WOULD'VE happened if bosses powered by the orb didn't tank stars.\n";
		}
	}
}

void towerView(POS& playerFloor, POS floorsToDraw) {
	string design; // Variable to determine what the floor will look like
	setColor(7);
	setColor(15);
	cout << "TOWER VIEW:\n" << endl;

	for (POS i = floorsToDraw; i > 0; i--) {
		setColor(8);
		switch (i % 10) {
		case 0:
			design = "|----==----|\n";
			break;
		case 5:
			design = "|  --==--  |\n";
			break;
		default:
			design = "|    ==    |\n";
		}
		// Checking to see if the player is there
		if (i == playerFloor) {
			design.replace(5, 2, (gameMode > SOLO ? "@@" : "@="));
			setColor(characterColor[playingAs]);
		}
		// Printing the tower
		cout << design;
	}
}
// A simple function to check if the player has quit the game.
bool hasQuit() {
	return response > 9;
}
// Generates a random number between two parameters named after the god of luck
short rngesus(short low, short high) {
	return rand() % ((high - low) + 1) + low;
}
// Runs each time the player is required to input something
POS getInput(POS numOptions) {
	cout << "What will you do?: ";
	setColor(3);
	cin >> response;
	response -= 49; // A software trick that makes the % work properly
	// Array Index OoB Exception prevention
	if (response >= numOptions && !hasQuit()) {
		response %= numOptions;
	}
	// Return what the player wanted to do
	return ++response;
}