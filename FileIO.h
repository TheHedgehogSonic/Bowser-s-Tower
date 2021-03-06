#ifndef FILEIO_H
#define FILEIO_H
#include <string>
#include <fstream>

using namespace std;

typedef unsigned int POS32;
typedef unsigned short POS;

class FileIO {
public:
	FileIO();
	~FileIO();
	POS32 getHighScore();
	void setFilePath(string set);
	POS32 loadHighScore();
	string readFact(POS factNum);
	void saveHighScore(POS32 newHigh);
private:
	ifstream reader;
	ofstream writer;
	string filePath;
	POS32 highScore;
};

#endif