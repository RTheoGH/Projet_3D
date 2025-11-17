#include "cube.h"
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>

Cube::Cube()
    : m_count(0)
{
    m_data.clear();
    m_data.reserve(36 * 6);

    const float m = 0.1f;

    auto addTriangle = [this](const QVector3D &v0, const QVector3D &v1, const QVector3D &v2) {
        QVector3D n = QVector3D::normal(v1 - v0, v2 - v0);
        add(v0, n);
        add(v1, n);
        add(v2, n);
    };

    addTriangle(QVector3D(-m,-m, m), QVector3D( m,-m, m), QVector3D( m, m, m));
    addTriangle(QVector3D( m, m, m), QVector3D(-m, m, m), QVector3D(-m,-m, m));

    addTriangle(QVector3D( m,-m,-m), QVector3D(-m,-m,-m), QVector3D(-m, m,-m));
    addTriangle(QVector3D(-m, m,-m), QVector3D( m, m,-m), QVector3D( m,-m,-m));

    addTriangle(QVector3D( m,-m, m), QVector3D( m,-m,-m), QVector3D( m, m,-m));
    addTriangle(QVector3D( m, m,-m), QVector3D( m, m, m), QVector3D( m,-m, m));

    addTriangle(QVector3D(-m,-m,-m), QVector3D(-m,-m, m), QVector3D(-m, m, m));
    addTriangle(QVector3D(-m, m, m), QVector3D(-m, m,-m), QVector3D(-m,-m,-m));

    addTriangle(QVector3D(-m, m, m), QVector3D( m, m, m), QVector3D( m, m,-m));
    addTriangle(QVector3D( m, m,-m), QVector3D(-m, m,-m), QVector3D(-m, m, m));

    addTriangle(QVector3D(-m,-m,-m), QVector3D( m,-m,-m), QVector3D( m,-m, m));
    addTriangle(QVector3D( m,-m, m), QVector3D(-m,-m, m), QVector3D(-m,-m,-m));
}

void Cube::add(const QVector3D &v, const QVector3D &n)
{
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x(); *p++ = v.y(); *p++ = v.z();
    *p++ = n.x(); *p++ = n.y(); *p++ = n.z();
    m_count += 6;
}
