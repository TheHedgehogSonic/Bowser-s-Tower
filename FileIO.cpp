#include "FileIO.h"
#include <iostream>

using namespace std;

FileIO::FileIO() {
	filePath = "C:\\Users\\there\\source\\repos\\Bowser's Tower\\Bowser's Tower\\High Score.txt";
}

FileIO::~FileIO() {
	// Destructor
}

POS32 FileIO::getHighScore() {
	return highScore;
}

void FileIO::setFilePath(string set) {
	filePath = set;
}

POS32 FileIO::loadHighScore() {
	filePath = "C:\\Users\\there\\source\\repos\\Bowser's Tower\\Bowser's Tower\\High Score.txt";
	reader.open(filePath);
	reader >> highScore;
	reader.close();
	// Returns the high score
	return highScore;
}

string FileIO::readFact(POS factNum) {
	string fact;
	filePath = "C:\\Users\\there\\source\\repos\\Bowser's Tower\\Bowser's Tower\\Facts\\" + to_string(factNum) + ".txt";

	reader.open(filePath);

	while (getline(reader, fact)) {
		reader >> fact;
	}

	reader.close();

	return fact;
}

void FileIO::saveHighScore(POS32 newHigh) {
	filePath = "C:\\Users\\there\\source\\repos\\Bowser's Tower\\Bowser's Tower\\High Score.txt";
	writer.open(filePath);
	writer << newHigh;
	writer.close();
}
