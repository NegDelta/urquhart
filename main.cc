#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <cmath>
#include "bitmap.hh"
#include "mesh.hh"

#define CANVAS_W 500
#define CANVAS_H 500
#define GRAPH_RADIUS 240
#define NODE_COUNT 100
#define PI 3.141592

using namespace std;

fstream file;
bitmap B(CANVAS_W, CANVAS_H);
mesh M;

	set<medge*> badtrg;
	set<mvert*> vertbuf;
	set<medge*> edgebuf;

void drawVert(bitmap& b, mvert* v) {
    b.drawRect(v->getX()-1, v->getY()-1,
               v->getX()+1, v->getY()+1, C_YELLOW);
}
void drawEdge(bitmap& b, medge* e, color_t col) {
    mvert v1 = *(e->vert1);
    mvert v2 = *(e->vert2);
    b.drawLine(v1.getX(),v1.getY(),
               v2.getX(),v2.getY(), col);
}
void drawFcir(bitmap& b, mface* f) {
    b.drawLine(f->cx-1, f->cy,
               f->cx+1, f->cy, C_CYAN);
    b.drawLine(f->cx, f->cy-1,
               f->cx, f->cy+1, C_CYAN);
}
void drawMesh(bitmap& b, mesh m) {
    b.drawRect(0, 0, b.getWidth(), b.getHeight(), C_BLACK);
    for(int i = 0; i < m.edgeCount(); i++) {
        drawEdge(b, m.getEdge(i), C_RED);
        //cout << i << '/' << m.edgeCount() << "edges\n";
    }
    for(int i = 0; i < m.vertCount(); i++) {
        drawVert(b, m.getVert(i));
        //cout << i << '/' << m.vertCount() << "verts\n";
    }
    for(int i = 0; i < m.faceCount(); i++) {
        //drawFcir(b, m.getFace(i));
        //cout << i << '/' << m.vertCount() << "verts\n";
    }
    for(auto it=badtrg.begin(); it!=badtrg.end(); it++)
    {
        drawEdge(b, *it, C_GREEN);
    }
    for(auto it=edgebuf.begin(); it!=edgebuf.end(); it++)
    {
        drawEdge(b, *it, C_BLUE);
    }
}

void derp() {

    drawMesh(B,M);

	file.open("out.ppm", fstream::trunc | fstream::out);
    file << B;

	file.close();
    //cin.ignore();
}

float rand01() {
    return float(rand())/(float)RAND_MAX;
}

int main() {
    srand(time(NULL));


	vector< pair<float,float> > t(NODE_COUNT);

    /* */ // concentrated
	float angl,rad;
	for(int i=0; i<NODE_COUNT; i++) {
        angl = rand01()*2*PI;
        rad  = rand01()*GRAPH_RADIUS;
        t[i].first  = CANVAS_W/2 + rad*cos(angl);
        t[i].second = CANVAS_H/2 + rad*sin(angl);
	} /* */

	/* * random walk
	float maxx=0, maxy=0, minx=0, miny=0;
    t[0].first  = 0;
    t[0].second = 0;
	for(int i=1; i<NODE_COUNT; i++) {
        t[i].first  = t[i-1].first  + rand01() - 0.5;
        t[i].second = t[i-1].second + rand01() - 0.5;
        maxx = max(maxx, t[i].first );
        minx = min(minx, t[i].first );
        maxy = max(maxy, t[i].second);
        miny = min(miny, t[i].second);
	}
    for(int i=0; i<NODE_COUNT; i++) {
        t[i].first  = CANVAS_W/2 + (2*(t[i].first  - minx)/(maxx-minx) -1) * GRAPH_RADIUS;
        t[i].second = CANVAS_H/2 + (2*(t[i].second - miny)/(maxy-miny) -1) * GRAPH_RADIUS;
	} /* */

    ////////////////////////// BEGIN URQUHART
	M.addVert(CANVAS_W/2 + 0.0             , CANVAS_H/2 + 4.0*GRAPH_RADIUS);
	M.addVert(CANVAS_W/2 + 5.0*GRAPH_RADIUS, CANVAS_H/2 - 3.0*GRAPH_RADIUS);
	M.addVert(CANVAS_W/2 - 5.0*GRAPH_RADIUS, CANVAS_H/2 - 3.0*GRAPH_RADIUS);
	M.addEdge(1,2).addEdge(0,2).addEdge(0,1);
	M.addFace(0,1,2);
	mvert *iv;
	medge *ie;
	//derp();
	for(int i=0; i<NODE_COUNT; i++) {
        M.addVert(t[i].first, t[i].second); // i+3
        //cout << "i = " << i << endl;
        //derp();
        iv = M.getVert(i+3);

        for(int k=M.faceCount()-1; k>=0; k--) { // a triaaaangle?~
            if (M.getFace(k)->isInCircle(iv)) { // it is a BAD TRIANGLE!! (dun dun)
                //cout << " k = " << k << endl;
                for(int j=0; j<3; j++) { // a toggle operation on all 3...
                    ie = M.getFace(k)->edges[j]; // ...edges
                    if(badtrg.find(ie) == badtrg.end()) {
                        badtrg.insert(ie);
                        //cout << "badtrg add" << endl;
                    } else {
                        badtrg.erase(badtrg.find(ie));
                        edgebuf.insert(ie);
                        //cout << "badtrg remove" << endl;
                    }
                    //derp();
                }
                M.removeFace(M.getFace(k)); // must remove
                //cout << " end k = " << k << endl;
                //derp();
            }
        }
        // now, keikaku says badtrg:set is literally a cycle
        // first, get rid of those inners
        for(auto it=edgebuf.begin(); it!=edgebuf.end(); it++) {
            M.removeEdge(*it);
        }
        // let's sort out the verts to connect w/ iv
        for(auto it=badtrg.begin(); it!=badtrg.end(); it++)
        {
            vertbuf.insert((*it)->vert1);
            vertbuf.insert((*it)->vert2);
            //cout << "vertbuf size = " << vertbuf.size() << endl;
        }
        for(auto it=vertbuf.begin(); it!=vertbuf.end(); it++)
        {
            M.addEdge(iv,*it);
        }
        for(auto it=badtrg.begin(); it!=badtrg.end(); it++)
        {
            M.addFace(*it, iv);
        }

        badtrg.clear();
        vertbuf.clear();
        edgebuf.clear();

        //cout << "end i = " << i << endl;
        //derp();
	}
	M.removeVert(M.getVert(2));
	M.removeVert(M.getVert(1));
	M.removeVert(M.getVert(0));

    float tm;
    for(int i = 0; i<M.faceCount(); i++) {
        tm = 0;
        for(int j = 0; j<3; j++)
            tm = max(tm, M.getFace(i)->edges[j]->getLength());
        for(int j = 0; j<3; j++)
            if (tm == M.getFace(i)->edges[j]->getLength())
                edgebuf.insert(M.getFace(i)->edges[j]);
    }
    //derp();
    while(!edgebuf.empty()) {
        M.removeEdge(*edgebuf.begin());
        edgebuf.erase(edgebuf.begin());
        //derp();
    }
    ////////////////////////// END URQUHART


	drawMesh(B,M);

	file.open("out.ppm", fstream::trunc | fstream::out);
    file << B;

	file.close();
	return 0;
}
