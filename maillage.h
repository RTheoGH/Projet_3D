#ifndef MAILLAGE_H
#define MAILLAGE_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QOpenGLFunctions>
#include <QString>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <iostream>
#include <memory>

class Maillage
{
public:
    explicit Maillage() : m_count(0) {};
    explicit Maillage(const QString &fileName) { loadOFF(fileName); }

    const GLfloat* data() const { return m_data.constData(); }
    int count() const { return m_count; }

    bool loadOFF(const QString &fileName);

private:
    void add(const QVector3D &v, const QVector3D &n);

private:
    QVector<GLfloat> m_data;
    int m_count;
};

struct HeightLayer{
    QImage img;
    QVector<float> heights;
};

struct Material{
    enum MaterialType{
        SAND,
        WATER,
        LAVA
    };

    MaterialType type;
    QImage albedo;
    float smooth_strength; // à voir
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

    std::unique_ptr<QOpenGLVertexArrayObject> vao;
    std::unique_ptr<QOpenGLBuffer> vbo_pos;
    std::unique_ptr<QOpenGLBuffer> vbo_norm;
    std::unique_ptr<QOpenGLBuffer> ebo;
    std::unique_ptr<QOpenGLBuffer> uv_buffer;

    bool valid = false;
    bool gpu_uploaded = false;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh&&) = default;

    bool has_heightmap = false;
    QOpenGLTexture* heightmap = nullptr;
    QImage heightmapImage;
    QOpenGLTexture* albedo = nullptr;
    QImage textureAlbedo;

    std::vector<HeightLayer> layers;
    bool saveAllHeightmaps(const QString &dir);
    bool loadAllHeightmaps(const QString& dir);

    void bindBuffers();
    void computeNormals();
};

class Plane : public Mesh
{
public:
    bool has_heightmap = true;

    Plane(float sizeX = 10, float sizeY = 10, unsigned int resolutionX = 32, unsigned int resolutionY = 32, QString texturePath = nullptr)
    {
        vertices.clear();
        normals.clear();
        uv_list.clear();
        triangles.clear();

        // Création des vertices et UVs
        for(unsigned int i = 0; i < resolutionX; i++){
            for(unsigned int j = 0; j < resolutionY; j++){
                float x = ((float)i/(resolutionX-1) - 0.5f) * sizeX;
                float y = 0.0f;
                float z = ((float)j/(resolutionY-1) - 0.5f) * sizeY;
                vertices.push_back(QVector3D(x, y, z));
                normals.push_back(QVector3D(0.0f, -1.0f, 0.0f));

                float u = (float)i / (resolutionX-1);
                float v = (float)j / (resolutionY-1);
                uv_list.push_back(QVector2D(u, v));
            }
        }

        // Création des triangles
        for(unsigned int i = 0; i < resolutionX-1; i++){
            for(unsigned int j = 0; j < resolutionY-1; j++){
                unsigned int idx0 = i * resolutionY + j;
                unsigned int idx1 = (i+1) * resolutionY + j;
                unsigned int idx2 = i * resolutionY + (j+1);
                unsigned int idx3 = (i+1) * resolutionY + (j+1);

                // Triangle 1
                triangles.push_back(idx0);
                triangles.push_back(idx2);
                triangles.push_back(idx3);

                // Triangle 2
                triangles.push_back(idx0);
                triangles.push_back(idx3);
                triangles.push_back(idx1);
            }
        }

        valid = true;
        Mesh::has_heightmap = true;
        // std::cout<<"heightmap : "<<has_heightmap<<std::endl;
        if(texturePath != nullptr){
            std::cout<<"récupération de la texture"<<std::endl;
            Mesh::textureAlbedo = QImage(texturePath);
        }

        // bindBuffers();

//         heightmap = new QOpenGLTexture(QImage(":/textures/heightmap.png"));
        // heightmap->setMagnificationFilter(QOpenGLTexture::Linear);


    }

};

// sélection : cercle géodésique

#endif
