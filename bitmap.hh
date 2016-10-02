#ifndef BITMAP_H
#define BITMAP_H

#include <vector>
#include <tuple>
#include <iostream>

using namespace std;
typedef unsigned char color_t;

// 3-bit palette
static const color_t C_BLACK = 0;
static const color_t C_BLUE = 1;
static const color_t C_GREEN = 2;
static const color_t C_CYAN = 3;
static const color_t C_RED = 4;
static const color_t C_MAGENTA = 5;
static const color_t C_YELLOW = 6;
static const color_t C_WHITE = 7;

class bitmap {
	int width;
	int height;

	vector< vector<color_t> > data; // inner vector is a horizontal line

	public:
	color_t defcol;

	bitmap(int,int); // constructor
	int getWidth();
	int getHeight();
	color_t getPixel(int, int);
	bitmap& setPixel(int, int, color_t); // allows for chaining
	void drawLine(int,int,int,int,color_t);
	void drawLine(int,int,int,int);
    void drawRect(int,int,int,int,color_t);
    void drawRect(int,int,int,int);

	friend ostream& operator<< (ostream&, const bitmap&); // output in PPM format
};

#endif // BITMAP_H
