#ifndef MAILLAGE_H
#define MAILLAGE_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QOpenGLFunctions>

class Maillage
{
public:
    explicit Maillage() : m_count(0) {};
    explicit Maillage(const QString &fileName);

    const GLfloat* data() const { return m_data.constData(); }
    int count() const { return m_count; }

    bool loadOFF(const QString &fileName);

private:
    void add(const QVector3D &v, const QVector3D &n);

private:
    QVector<GLfloat> m_data;
    int m_count;
};

class Mesh
{
public:
    Mesh() = default;
    explicit Mesh(const QString &fileName) { loadOFF(fileName); }

    bool loadOFF(const QString &fileName);

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<unsigned int> triangles;
    QVector<QVector2D> uv_list;

    bool valid = false;
};

class Plane : public Mesh{
public:
    float sizeX;
    float sizeY;
    unsigned int resolutionX;
    unsigned int resolutionY;

    Plane(float sizeX = 10, float sizeY = 10, unsigned int resolutionX = 32, unsigned int resolutionY = 32){
        for(unsigned int i = 0; i<resolutionX; i++){
            for(unsigned int j = 0; j<resolutionY; j++){
                vertices.push_back(QVector3D(((float)i/(resolutionX-1)-0.5)*sizeX, 0, ((float)j/(resolutionY-1)-0.5)*sizeY));
                normals.push_back(QVector3D(0.0, 1.0, 0.0));

                float u = (float)i / (float) resolutionX-1;
                float v = (float)j / (float) resolutionY-1;
                uv_list.push_back(QVector2D(u, v));
            }
        }
        for(unsigned int i = 0; i<resolutionX; i++){
            for(unsigned int j = 0; j<resolutionY; j++){
                if(i<resolutionX-1 && j<resolutionY-1){
                    unsigned int voisin_haut = (i+1)*resolutionY+j;
                    unsigned int voisin_droite = i*resolutionY+j+1;
                    unsigned int voisin_haut_droite = (i+1)*resolutionY+j+1;

                    triangles.push_back(i); triangles.push_back(voisin_droite); triangles.push_back(voisin_haut_droite);
                    triangles.push_back(i); triangles.push_back(voisin_haut); triangles.push_back(voisin_haut_droite);
                }
            }
        }
    }
};


#endif
