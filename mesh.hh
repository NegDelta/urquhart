#ifndef MESH_H
#define MESH_H

#include <list>
#include <vector>
#include <set>
#include <cmath>

using namespace std;

class mesh;
class mface;
class medge;
class mvert;

class mesh {
    vector<mvert*> verts;
    vector<medge*> edges;
    vector<mface*> faces;
    public:

    mesh& addVert(float,float);
    mesh& addEdge(mvert*, mvert*); // throws E if intersecting
    mesh& addEdge(int, int);
    mesh& addFace(mvert*, mvert*, mvert*);
    mesh& addFace(int, int, int);
    mesh& addFace(medge*, mvert*);
    mesh& removeVert(mvert*);
    mesh& removeEdge(medge*);
    mesh& removeFace(mface*);

    int vertCount();
    int edgeCount();
    int faceCount();
    mvert* getVert(int);
    medge* getEdge(int);
    mface* getFace(int);
};

class mface { // always a triangle, see ctor

    public:
    const float cx, cy, crad; // circumcircle parameters
    medge* edges[3];

    mface(mvert*,mvert*,mvert*);
    bool isInCircle(mvert*); // check if point is in circumcircle. Throws E if not a triangle
};

class medge {
    public:
    mvert * const vert1;
    mvert * const vert2; // comprised vertices
    mface *aFace, *bFace; // adjacent faces
    //medge *aPrev, *aNext, *bPrev, *bNext; // polar angle (anti-clockwise) ordering

    medge(mvert*,mvert*);
    float getLength();
};

class mvert {

    float x, y;
    public:
    set< pair<float, medge*> > edges;

    mvert(float,float);
    medge* getEdge(mvert*); // returns connecting edge iff adjacent
    float getX();
    float getY();
};

#endif // MESH_H
