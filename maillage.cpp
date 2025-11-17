#include "maillage.h"
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>

void Maillage::add(const QVector3D &v, const QVector3D &n)
{
    qDebug() << "Ajout du sommet" << v << "avec normal" << n;
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
    qDebug() << "Chargement du maillage...";
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    QString header = in.readLine().trimmed();
    if (header != "OFF") {
        qDebug() << "Fichier OFF invalide";
        return false;
    }

    int nVertices = 0, nFaces = 0, nEdges = 0;
    in >> nVertices >> nFaces >> nEdges;

    QVector<QVector3D> vertices;
    for (int i = 0; i < nVertices; i++) {
        float x, y, z;
        in >> x >> y >> z;
        vertices.append(QVector3D(x, y, z));
    }

    m_data.clear();
    m_count = 0;

    for (int i = 0; i < nFaces; i++) {
        int n, v0, v1, v2;
        in >> n >> v0 >> v1 >> v2;
        if (n != 3) {
            qDebug() << "Triangles ?";
            continue;
        }
        // Triangles
        QVector3D p0 = vertices[v0];
        QVector3D p1 = vertices[v1];
        QVector3D p2 = vertices[v2];
        QVector3D normal = QVector3D::normal(p1 - p0, p2 - p0);
        add(p0, normal);
        add(p1, normal);
        add(p2, normal);
    }

    //qDebug() << "Vertices : " << vertices;

    return true;
}

bool Mesh::loadOFF(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    QString header = in.readLine().trimmed();
    if (header != "OFF") return false;

    int nVertices, nFaces, nEdges;
    in >> nVertices >> nFaces >> nEdges;

    vertices.resize(nVertices);
    for (int i = 0; i < nVertices; i++)
        in >> vertices[i][0] >> vertices[i][1] >> vertices[i][2];

    triangles.clear();
    normals.clear();

    for (int i = 0; i < nFaces; i++) {
        int n, v0, v1, v2;
        in >> n >> v0 >> v1 >> v2;

        if (n != 3) continue;

        triangles.append(v0);
        triangles.append(v1);
        triangles.append(v2);

        // normal par face
        QVector3D p0 = vertices[v0];
        QVector3D p1 = vertices[v1];
        QVector3D p2 = vertices[v2];
        QVector3D nrm = QVector3D::normal(p1 - p0, p2 - p0);

        normals.append(nrm);
        normals.append(nrm);
        normals.append(nrm);
    }

    valid = true;
    return true;
}

