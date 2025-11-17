#ifndef CUBE_H
#define CUBE_H

#include <QVector3D>
#include <QVector>
#include <QOpenGLFunctions>

class Cube
{
public:
    Cube();

    void add(const QVector3D &v, const QVector3D &n);

    const GLfloat* data() const { return m_data.constData(); }
    int count() const { return m_count; }

private:
    QVector<GLfloat> m_data;
    int m_count;
};

#endif
