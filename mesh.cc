#include "mesh.hh"
#include <iostream>
#include <algorithm>
#include <utility>
#define PI 3.14159265

/*
    TODO:

    implement Bowyer-Watson
    generate Urquhart graph
*/

// Returns direction of the o-a geometric vector. Useful for ordering
float angle(mvert *o, mvert *a) { // vector from a to b
    float acc = atan2 (a->getY()-o->getY(), a->getX()-o->getX());

    return acc;
}


mesh& mesh::addVert(float x, float y) {
    mvert *v = new mvert(x,y);
    verts.push_back(v);
    return *this;
}

mesh& mesh::addEdge(mvert* a, mvert* b) {
    if (a->getEdge(b) != nullptr)
        return *this;

    medge *e = new medge(a,b);
    edges.push_back(e);

    a -> edges.insert(make_pair(angle(a,b),e));
    b -> edges.insert(make_pair(angle(b,a),e));

    return *this;
}
mesh& mesh::addEdge(int a, int b) {
    addEdge(getVert(a),getVert(b));
    return *this;
}

mesh& mesh::addFace(mvert* a, mvert* b, mvert* c) {
    mface *f = new mface(a,b,c);
    faces.push_back(f);
    return *this;
}
mesh& mesh::addFace(int a, int b, int c) {
    addFace(getVert(a),getVert(b),getVert(c));
    return *this;
}

mesh& mesh::addFace(medge* e, mvert* v) {
    addFace(e->vert1, e->vert2, v);
    return *this;
}

mesh& mesh::removeVert(mvert* v) {
    for(auto it = v->edges.begin(); it!=v->edges.end(); it++) {
        removeEdge(it->second);
    }

    for(auto it = verts.begin(); it!=verts.end(); it++)
        if (*it == v) {
            verts.erase(it);
            it--;
        }
    delete v;
    v = nullptr;
    return *this;
}

mesh& mesh::removeEdge(medge* e) {
    if(e->aFace != nullptr)
        removeFace(e->aFace);
    if(e->bFace != nullptr)
        removeFace(e->bFace);

    for(auto it = e->vert1->edges.begin(); it!=e->vert1->edges.end(); it++)
        if (it->second == e) {
            e->vert1->edges.erase(it);
            //it--;
        }
    for(auto it = e->vert2->edges.begin(); it!=e->vert2->edges.end(); it++)
        if (it->second == e) {
            e->vert2->edges.erase(it);
            //it--;
        }
/*
    auto position1 = find(e->vert1->edges.begin(), e->vert1->edges.end(), e);
        if (position1 != e->vert1->edges.end())
            e->vert1->edges.erase(position1);

    auto position2 = find(e->vert2->edges.begin(), e->vert2->edges.end(), e);
        if (position2 != e->vert2->edges.end())
            e->vert2->edges.erase(position2);*/

    for(auto it = edges.begin(); it!=edges.end(); it++)
        if (*it == e) {
            edges.erase(it);
            it--;
        }
    delete e;
    e = nullptr;
    return *this;
}

mesh& mesh::removeFace(mface* f) {
    for(int i=0; i<3; i++) {
        if (f->edges[i]->aFace == f)
            f->edges[i]->aFace = nullptr;

        if (f->edges[i]->bFace == f)
            f->edges[i]->bFace = nullptr;
    }/*
    for(auto it = faces.begin(); it!=faces.end(); ++it) {
        std::cout << *it << std::endl;
        std::cin.get();
        if (*it == f) {
            faces.erase(it);
        }
    }*/
    auto position = find(faces.begin(), faces.end(), f);
    if (position != faces.end())
        faces.erase(position);
    delete f;
    f = nullptr;
    return *this;
}

int mesh::vertCount() {
    return verts.size();
}
int mesh::edgeCount() {
    return edges.size();
}
int mesh::faceCount() {
    return faces.size();
}
mvert* mesh::getVert(int i) {
    return verts[i];
}
medge* mesh::getEdge(int i) {
    return edges[i];
}
mface* mesh::getFace(int i) {
    return faces[i];
}

float medge::getLength() {
    return (vert1->getX() - vert2->getX()) * (vert1->getX() - vert2->getX())
         + (vert1->getY() - vert2->getY()) * (vert1->getY() - vert2->getY());
}

float facecalc(mvert* v1, mvert* v2, mvert* v3, char ctr) {

    float dA = v1->getX() * v1->getX() + v1->getY() * v1->getY();
    float dB = v2->getX() * v2->getX() + v2->getY() * v2->getY();
    float dC = v3->getX() * v3->getX() + v3->getY() * v3->getY();

    float aux1 = (dA*(v3->getY() - v2->getY()) + dB*(v1->getY() - v3->getY()) + dC*(v2->getY() - v1->getY()));
    float aux2 = -(dA*(v3->getX() - v2->getX()) + dB*(v1->getX() - v3->getX()) + dC*(v2->getX() - v1->getX()));
    float div = (2*(v1->getX()*(v3->getY() - v2->getY()) + v2->getX()*(v1->getY()-v3->getY()) + v3->getX()*(v2->getY() - v1->getY())));

    float sx = aux1/div;
    float sy = aux2/div;

    float rad = (sx - v1->getX())*(sx - v1->getX()) + (sy - v1->getY())*(sy - v1->getY());

	switch(ctr) {
        case 'x':
            return sx;
            break;
        case 'y':
            return sy;
            break;
	}
	return rad;
}
mface::mface(mvert* v1, mvert* v2, mvert* v3):
    cx  (facecalc(v1,v2,v3,'x')),
    cy  (facecalc(v1,v2,v3,'y')),
    crad(facecalc(v1,v2,v3,'r'))
{
    // init face
    edges[0] = v1->getEdge(v2);
    edges[1] = v2->getEdge(v3);
    edges[2] = v3->getEdge(v1);

    // adjust edges
    for(int i=0; i<3; i++)
        if (edges[i]->aFace == nullptr)
            edges[i]->aFace = this;
        else
            edges[i]->bFace = this;
}

bool mface::isInCircle(mvert* v) {
    return ((v->getX()-cx) * (v->getX()-cx) + (v->getY()-cy) * (v->getY()-cy)) < crad;
}

medge::medge(mvert* v1, mvert* v2):
    vert1(v1),
    vert2(v2)
{
    this->aFace = nullptr;
    this->bFace = nullptr;
/*
    this->aPrev = nullptr;
    this->aNext = nullptr;
    this->bPrev = nullptr;
    this->bNext = nullptr;*/
}


mvert::mvert(float x, float y) {
    this->x = x;
    this->y = y;
}
float mvert::getX() {
    return x;
}
float mvert::getY() {
    return y;
}

medge* mvert::getEdge(mvert* v) {
    medge* e;
    if (!edges.empty())
        for(auto it=edges.begin(); it!=edges.end(); ++it) {
            e = it->second;
            if (e->vert1 == v || e->vert2 == v)
                return e;
        }
    return nullptr;
}

