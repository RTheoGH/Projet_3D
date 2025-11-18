#include "maillage.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

void Maillage::add(const QVector3D &v, const QVector3D &n)
{
    m_data.append(v.x());
    m_data.append(v.y());
    m_data.append(v.z());
    m_data.append(n.x());
    m_data.append(n.y());
    m_data.append(n.z());
    m_count += 6;
}

bool Maillage::loadOFF(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&file);
    QString header = in.readLine().trimmed();
    if (header != "OFF") return false;

    int nVertices = 0, nFaces = 0, nEdges = 0;
    in >> nVertices >> nFaces >> nEdges;

    QVector<QVector3D> vertices;
    for(int i = 0; i < nVertices; i++){
        float x, y, z;
        in >> x >> y >> z;
        vertices.push_back(QVector3D(x, y, z));
    }

    m_data.clear();
    m_count = 0;

    for(int i = 0; i < nFaces; i++){
        int n, v0, v1, v2;
        in >> n >> v0 >> v1 >> v2;
        if(n != 3) continue;

        QVector3D p0 = vertices[v0];
        QVector3D p1 = vertices[v1];
        QVector3D p2 = vertices[v2];

        QVector3D normal = QVector3D::normal(p1 - p0, p2 - p0);

        add(p0, normal);
        add(p1, normal);
        add(p2, normal);
    }

    return true;
}

bool Mesh::loadOFF(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&file);
    QString header = in.readLine().trimmed();
    if(header != "OFF") return false;

    int nVertices, nFaces, nEdges;
    in >> nVertices >> nFaces >> nEdges;

    vertices.resize(nVertices);
    for(int i = 0; i < nVertices; i++)
        in >> vertices[i][0] >> vertices[i][1] >> vertices[i][2];

    triangles.clear();
    normals.resize(nVertices);
    for(int i = 0; i < nVertices; i++) normals[i] = QVector3D(0,0,0);

    for(int i = 0; i < nFaces; i++){
        int n, v0, v1, v2;
        in >> n >> v0 >> v1 >> v2;
        if(n != 3) continue;

        triangles.push_back(v0);
        triangles.push_back(v1);
        triangles.push_back(v2);

        QVector3D faceNormal = QVector3D::normal(vertices[v1]-vertices[v0], vertices[v2]-vertices[v0]);
        normals[v0] += faceNormal;
        normals[v1] += faceNormal;
        normals[v2] += faceNormal;
    }

    // Normalisation
    for(auto &n : normals) n.normalize();

    valid = true;
    return true;
}


void Mesh::bindBuffers()
{
    if (gpu_uploaded) return;

    // create wrappers
    if (!vao) vao = std::make_unique<QOpenGLVertexArrayObject>();
    if (!vbo_pos) vbo_pos = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!vbo_norm) vbo_norm = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    if (!ebo) ebo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    if (!uv_buffer) uv_buffer = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    vao->create();
    QOpenGLVertexArrayObject::Binder vaoBinder(vao.get());

    // positions
    vbo_pos->create();
    vbo_pos->bind();
    vbo_pos->allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo_pos->release();

    // normals
    if (!normals.isEmpty()) {
        vbo_norm->create();
        vbo_norm->bind();
        vbo_norm->allocate(normals.constData(), normals.size() * sizeof(QVector3D));
        f->glEnableVertexAttribArray(1);
        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        vbo_norm->release();
    }

    // uvs
    if (!uv_list.isEmpty()) {
        uv_buffer->create();
        uv_buffer->bind();
        uv_buffer->allocate(uv_list.constData(), uv_list.size() * sizeof(QVector2D));
        f->glEnableVertexAttribArray(2);
        f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        uv_buffer->release();
    }

    // indices
    ebo->create();
    ebo->bind();
    ebo->allocate(triangles.constData(), triangles.size() * sizeof(unsigned int));

    // ebo->release();

    vao->release();

    // ebo->release();

    gpu_uploaded = true;
}


