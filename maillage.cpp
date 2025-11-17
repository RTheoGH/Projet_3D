#include "maillage.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

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

