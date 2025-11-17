/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

#include <QFile>
#include <QTextStream>
#include <QVector3D>
#include <QMessageBox>

#include "maillage.h"

bool GLWidget::m_transparent = false;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_xRot(0),
      m_yRot(0),
      m_zRot(0),
      m_program(0)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        //Completer pour emettre un signal
        emit xRotationChanged(m_xRot);

        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        //Completer pour emettre un signal
        emit yRotationChanged(m_yRot);

        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        //Completer pour emettre un signal
        emit zRotationChanged(m_zRot);

        update();
    }
}

void GLWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();
    m_logoVbo.destroy();
    m_cubeVbo.destroy();
    m_meshVbo_pos.destroy();
    m_meshVbo_norm.destroy();
    m_meshEbo.destroy();

    delete m_program;
    m_program = 0;
    doneCurrent();
}

void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    // Compile vertex shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);

    // Link shader pipeline
    if (!m_program->link())
        close();

    // Bind shader pipeline for use
    if (!m_program->bind())
        close();

    m_mvp_matrix_loc = m_program->uniformLocation("mvp_matrix");
    m_normal_matrix_loc = m_program->uniformLocation("normal_matrix");
    m_light_pos_loc = m_program->uniformLocation("light_position");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    // m_logoVbo.create();
    // m_logoVbo.bind();
    // m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    // --- Mesh buffers initialisation ---

    m_meshVbo_pos.create();
    m_meshVbo_pos.bind();
    m_meshVbo_pos.allocate(0);     // vide pour le moment
    m_meshVbo_pos.release();

    m_meshVbo_norm.create();
    m_meshVbo_norm.bind();
    m_meshVbo_norm.allocate(0);
    m_meshVbo_norm.release();

    m_meshEbo.create();
    m_meshEbo.bind();
    m_meshEbo.allocate(0);
    m_meshEbo.release();


    // m_cubeVbo.create();
    // m_cubeVbo.bind();
    // m_cubeVbo.allocate(m_cube.data(), m_cube.count() * sizeof(GLfloat));

    // QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // f->glEnableVertexAttribArray(0);
    // f->glEnableVertexAttribArray(1);
    // f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), nullptr);
    // f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // m_cubeVbo.release();

    // Our camera never changes in this example.
    m_view.setToIdentity();
    m_view.translate(0, 0, -3);

    // Light position is fixed.
    m_program->setUniformValue(m_light_pos_loc, QVector3D(0, 0, 70));

    m_program->release();
}

void GLWidget::setupVertexAttribs()
{
    // VAO doit être activé
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Positions
    m_meshVbo_pos.bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    m_meshVbo_pos.release();

    // Normales
    m_meshVbo_norm.bind();
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    m_meshVbo_norm.release();

}

void GLWidget::uploadMeshToGPU(const Mesh& m)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    m_meshVbo_pos.bind();
    m_meshVbo_pos.allocate(m.vertices.data(), m.vertices.size() * sizeof(QVector3D));
    m_meshVbo_pos.release();

    m_meshVbo_norm.bind();
    m_meshVbo_norm.allocate(m.normals.data(), m.normals.size() * sizeof(QVector3D));
    m_meshVbo_norm.release();

    m_meshEbo.bind();
    m_meshEbo.allocate(m.triangles.data(), m.triangles.size() * sizeof(unsigned int));
    m_meshEbo.release();

    m_indexCount = m.triangles.size();
}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_model.setToIdentity();
    m_model.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_model.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_model.rotate(m_zRot / 16.0f, 0, 0, 1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();

    // Set modelview-projection matrix
    m_program->setUniformValue(m_mvp_matrix_loc, m_projection * m_view * m_model);
    QMatrix3x3 normal_matrix = m_model.normalMatrix();

    // Set normal matrix
    m_program->setUniformValue(m_normal_matrix_loc, normal_matrix);

    //glDrawArrays(GL_TRIANGLES, 0, m_logo.vertexCount());
    //glDrawArrays(GL_TRIANGLES, 0, m_cube.vertexCount());
    if (!m_meshLoaded) {
        Plane p;
        uploadMeshToGPU(p);

    }

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    // Active les attributs vertex
    m_meshVbo_pos.bind();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), nullptr);

    m_meshVbo_norm.bind();
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), nullptr);

    m_meshEbo.bind();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

    m_meshVbo_pos.release();
    m_meshVbo_norm.release();
    m_meshEbo.release();


    m_program->release();
}

void GLWidget::resizeGL(int w, int h)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_last_position = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_last_position.x();
    int dy = event->y() - m_last_position.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_last_position = event->pos();
}

void GLWidget::setMesh(const Mesh &mesh)
{
    if (!mesh.valid) return;

    makeCurrent();
    m_mesh = mesh;

    // 1) VBO positions
    if (!m_meshVbo_pos.isCreated())
        m_meshVbo_pos.create();
    m_meshVbo_pos.bind();
    m_meshVbo_pos.allocate(mesh.vertices.constData(),
                           mesh.vertices.size() * sizeof(QVector3D));
    m_meshVbo_pos.release();

    // 2) VBO normales
    if (!m_meshVbo_norm.isCreated())
        m_meshVbo_norm.create();
    m_meshVbo_norm.bind();
    m_meshVbo_norm.allocate(mesh.normals.constData(),
                            mesh.normals.size() * sizeof(QVector3D));
    m_meshVbo_norm.release();

    // 3) EBO indices
    if (!m_meshEbo.isCreated())
        m_meshEbo.create();
    m_meshEbo.bind();
    m_meshEbo.allocate(mesh.triangles.constData(),
                       mesh.triangles.size() * sizeof(unsigned int));
    m_meshEbo.release();

    m_indexCount = mesh.triangles.size();
    m_meshLoaded = true;
    update();
}

