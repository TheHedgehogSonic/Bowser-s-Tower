#include "FileIO.h"

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
	reader.open(filePath);
	reader >> highScore;
	reader.close();
	// Returns the high score
	return highScore;
}

void FileIO::saveHighScore(POS32 newHigh) {
	writer.open(filePath);
	writer << newHigh;
	writer.close();
}
