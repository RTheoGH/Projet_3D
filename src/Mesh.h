#ifndef PROJECTMESH_H
#define PROJECTMESH_H

#include <vector>
#include "point3.h"


struct Vector2{
    float x;
    float y;

    Vector2(){}
    Vector2(float vX, float vY){ x = vX; y = vY; }
    Vector2(float value){ x = value; y= value; }

    Vector2 operator + (Vector2 v) { return Vector2(v.x+x, v.y+y); }
    double dot(Vector2 v){ return x*v.x + y*v.y; }
};

struct Vertex{
    point3d p;
    Vertex() {}
    Vertex(double x , double y , double z) : p(x,y,z) {}
    double & operator [] (unsigned int c) { return p[c]; }
    double operator [] (unsigned int c) const { return p[c]; }
};
struct Triangle{
    unsigned int corners[3];
    unsigned int & operator [] (unsigned int c) { return corners[c]; }
    unsigned int operator [] (unsigned int c) const { return corners[c]; }
    unsigned int size() const { return 3 ; }
};
struct Mesh{
    std::vector< Vertex > vertices;
    std::vector< Triangle > triangles;
    void operator = (Mesh m){
        vertices = m.vertices;
        triangles = m.triangles;
        // std::cout<<"allo"<<std::endl;
    }
};

struct Plane : Mesh{

    std::vector< Vector2 > uv_list;
    float sizeX;
    float sizeY;
    unsigned int resolutionX;
    unsigned int resolutionY;

    Plane(float sizeX = 10, float sizeY = 10, unsigned int resolutionX = 32, unsigned int resolutionY = 32){
        for(unsigned int i = 0; i<resolutionX; i++){
            for(unsigned int j = 0; j<resolutionY; j++){
                vertices.push_back(Vertex(((float)i/(resolutionX-1)-0.5)*sizeX, 0, ((float)j/(resolutionY-1)-0.5)*sizeY));

                float u = (float)i / (float) (resolutionX-1);
                float v = (float)j / (float) (resolutionY-1);
                uv_list.push_back(Vector2(u, v));
            }
        }
        for(unsigned int i = 0; i<resolutionX; i++){
            for(unsigned int j = 0; j<resolutionY; j++){
                if(i<resolutionX-1 && j<resolutionY-1){
                    unsigned int self_vertex = i*resolutionY+j;
                    unsigned int voisin_haut = (i+1)*resolutionY+j;
                    unsigned int voisin_droite = i*resolutionY+j+1;
                    unsigned int voisin_haut_droite = (i+1)*resolutionY+j+1;

                    Triangle t1; t1.corners[0] = self_vertex; t1.corners[1] = voisin_droite; t1.corners[2] = voisin_haut_droite;
                    Triangle t2; t2.corners[0] = self_vertex; t2.corners[2] = voisin_haut; t2.corners[1] = voisin_haut_droite;
                    triangles.push_back(t1); triangles.push_back(t2);
                }
            }
        }
    }

};

#endif // PROJECTMESH_H
