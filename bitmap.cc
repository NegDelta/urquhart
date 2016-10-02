#include <vector>
#include <iostream>
#include "bitmap.hh"

using namespace std;

bitmap::bitmap(int w, int h) {
	vector<color_t> line(w, C_BLACK);

	this->width = w;
	this->height = h;
	this->defcol = C_WHITE;
	this->data = vector< vector<color_t> > (h, line);
}

int bitmap::getWidth() {
	return this->width;
}

int bitmap::getHeight() {
	return this->height;
}

void bitmap::drawLine(int x0, int y0, int x1, int y1, color_t col){
    if (x0 == x1) {
        int step = y1>y0 ? 1 : -1;
        for (int y=y0; step*y < step*y1; y+=step) {
            setPixel(x0,y,col);
        }
    } else {
        int deltax = x1 - x0;
        int deltay = y1 - y0;
        float error = -1.0;
        float deltaerr = float(deltay) / float(deltax);   // Assume deltax != 0 (line is not vertical),
               // note that this division needs to be done in a way that preserves the fractional part
        int y = y0;
        int stepx = x1>x0 ? 1 : -1;
        int stepy = y1>y0 ? 1 : -1;
        deltaerr *= stepx * stepy;
        for (int x=x0; stepx*x < stepx*x1; x+=stepx) {
            setPixel(x,y,col);
            error = error + deltaerr;
            while (error >= 0.0) {
                setPixel(x,y,col);
                y += stepy;
                error -= 1.0;
            }
        }
    }
}
void bitmap::drawLine(int x0, int y0, int x1, int y1){
    drawLine(x0,y0,x1,y1,defcol);
}

void bitmap::drawRect(int x0, int y0, int x1, int y1, color_t col) {
    if (x1<x0)
        swap(x0,x1);
    if (y1<y0)
        swap(y0,y1);
    for(int x=x0; x<=x1; x++)
        for(int y=y0; y<=y1; y++)
            setPixel(x,y,col);
}
void bitmap::drawRect(int x0, int y0, int x1, int y1) {
    drawRect(x0, y0, x1, y1, defcol);
}

ostream& operator<< (ostream& os, const bitmap& b) {
	os << "P3" << '\n';
	os << b.width << " " << b.height << " 1\n";

	for(int ih = 0; ih < b.height; ih++) {
        for(int iw = 0; iw < b.width; iw++) {
            os << b.data[ih][iw]/4     << ' ';
            os << b.data[ih][iw]/2 % 2 << ' ';
            os << b.data[ih][iw]   % 2 << ' ';
        }
        os << '\n';
	}

	return os;
}

color_t bitmap::getPixel(int w, int h) {
    return this->data[h][w];
}

bitmap& bitmap::setPixel(int w, int h, color_t col) {
    if (w >= 0 && w<getWidth() &&
        h >= 0 && h<getHeight()) {
        this->data[h][w] = col;
    //cout << "setpixel " << w << ' ' << h << ' ' << char('0'+col) << '\n';
    }
    return *this;
}
