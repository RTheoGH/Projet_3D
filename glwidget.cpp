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
#include <iostream>
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
    makeCurrent();
    // Option 1 : laisser unique_ptr faire le boulot, mais certains wrappers veulent destroy()
    for (auto &mptr : scene_meshes) {
        if (!mptr) continue;
        if (mptr->has_heightmap) { delete mptr->heightmap; mptr->heightmap = nullptr; }
        if (mptr->vao) mptr->vao->destroy();
        if (mptr->vbo_pos) mptr->vbo_pos->destroy();
        if (mptr->vbo_norm) mptr->vbo_norm->destroy();
        if (mptr->ebo) mptr->ebo->destroy();
        if (mptr->uv_buffer) mptr->uv_buffer->destroy();
    }

    scene_meshes.clear();
    doneCurrent();

    if (m_program) { delete m_program; m_program = nullptr; }
}



void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);


    initializeOpenGLFunctions();
//    glClearColor(0, 0, 0, m_transparent ? 0 : 1);
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    m_program = new QOpenGLShaderProgram;
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl"))
        close();

    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl"))
        close();

    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("uv", 2);

    if (!m_program->link())
        close();

    if (!m_program->bind())
        close();

    m_mvp_matrix_loc = m_program->uniformLocation("mvp_matrix");
    m_normal_matrix_loc = m_program->uniformLocation("normal_matrix");
    m_light_pos_loc = m_program->uniformLocation("light_position");

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);


    m_view.setToIdentity();
    m_view.setToIdentity();
    m_view.translate(0,0,m_zoom);

    m_program->setUniformValue(m_light_pos_loc, QVector3D(0, 0, 70));

    m_program->release();
}

// void GLWidget::uploadMeshToGPU(Mesh& m)
// {
//     makeCurrent();
//     m.bindBuffers();
// }


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_program->bind();
    m_model.setToIdentity();
    m_model.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_model.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_model.rotate(m_zRot / 16.0f, 0, 0, 1);

    m_program->bind();
    m_view.setToIdentity();
    m_view.translate(m_tx, m_ty, m_zoom);
    m_program->setUniformValue(m_mvp_matrix_loc, m_projection * m_view * m_model);
    m_program->setUniformValue(m_normal_matrix_loc, m_model.normalMatrix());

    for (auto &mptr : scene_meshes) {
        if (!mptr || !mptr->gpu_uploaded) continue;

        if (mptr->has_heightmap) {
            glActiveTexture(GL_TEXTURE0);
            mptr->heightmap->bind();
            // Récupérer la location du sampler une fois (voir initializeGL), sinon :
            int loc = m_program->uniformLocation("heightmap");
            m_program->setUniformValue(loc, 0); // texture unit 0

            // set height scale (exemple)/*
            int hloc = m_program->uniformLocation("height_scale");
            m_program->setUniformValue(hloc, 3.0f); // ajuste 3.0f comme tu veux*/
        }

        QOpenGLVertexArrayObject::Binder vaoBinder(mptr->vao.get());
        glDrawElements(GL_TRIANGLES, mptr->triangles.size(), GL_UNSIGNED_INT, nullptr);
    }


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

    bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

    if (ctrlPressed && (event->buttons() & Qt::LeftButton)) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (ctrlPressed && (event->buttons() & Qt::RightButton)) {
//        setXRotation(m_xRot + 8 * dy);
//        setZRotation(m_zRot + 8 * dx);
        m_tx += dx * 0.01f;
        m_ty -= dy * 0.01f;
        update();
    }
    m_last_position = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event){
    m_zoom += event->angleDelta().y() * 0.001f;
    update();
}

void GLWidget::addMesh(std::unique_ptr<Mesh> mesh)
{
    if (!mesh || !mesh->valid) return;

    makeCurrent();
    initializeOpenGLFunctions();
    mesh->bindBuffers();

    scene_meshes.push_back(std::move(mesh));
    Mesh* mptr = scene_meshes.back().get();

    // AUTO : si le mesh veut une heightmap et qu’elle n’est pas encore créée
    if (mptr->has_heightmap && mptr->heightmap == nullptr) {

        QImage img(":/textures/heightmap.png");
        if (img.isNull()) {
            qWarning() << "heightmap image is null! path=:/textures/heightmap.png";
        } else {
            mptr->heightmap = new QOpenGLTexture(img.mirrored());
            mptr->heightmap->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            mptr->heightmap->setMagnificationFilter(QOpenGLTexture::Linear);
            mptr->heightmap->generateMipMaps();
        }
    }

    doneCurrent();
    update();
}


