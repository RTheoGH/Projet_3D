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

int clamp(int angle){
    int minX = 0 * 16;     // 0°
    int maxX = -180 * 16;   // 180°

    if (angle > minX) return minX;
    if (angle <= maxX) return maxX;

    return angle;
}

void GLWidget::setXRotation(int angle)
{
    //qNormalizeAngle(angle);
    angle = clamp(angle);

    //std::cout << "angle : " << angle << std::endl;

    if (angle != m_xRot) {
        m_xRot = angle;
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    int mesh_index = 0;
    for (auto &mptr : scene_meshes) {
        if (!mptr || !mptr->gpu_uploaded) continue;

        if (mptr->has_heightmap) {
            glActiveTexture(GL_TEXTURE0);
            mptr->heightmap->bind();
            int loc = m_program->uniformLocation("current_hm");
            m_program->setUniformValue(loc, 0);
            glActiveTexture(GL_TEXTURE1);
            scene_meshes[0]->heightmap->bind();
            int locSand = m_program->uniformLocation("heightmapSand");
            m_program->setUniformValue(locSand, 1);
            glActiveTexture(GL_TEXTURE2);
            scene_meshes[1]->heightmap->bind();
            int locWater = m_program->uniformLocation("heightmapWater");
            m_program->setUniformValue(locWater, 2);
            glActiveTexture(GL_TEXTURE3);
            scene_meshes[2]->heightmap->bind();
            int locLava = m_program->uniformLocation("heightmapLava");
            m_program->setUniformValue(locLava, 3);

            int iloc = m_program->uniformLocation("hm_index");
            m_program->setUniformValue(iloc, mesh_index);

            // set height scale (exemple)/*
            int hloc = m_program->uniformLocation("height_scale");
            m_program->setUniformValue(hloc, 3.0f); // ajuste 3.0f comme tu veux*/
        }
        if (!mptr->textureAlbedo.isNull()){
            glActiveTexture(GL_TEXTURE4);
            mptr->albedo->bind();
            int loc = m_program->uniformLocation("albedo");
            m_program->setUniformValue(loc, 4);
        }

        QOpenGLVertexArrayObject::Binder vaoBinder(mptr->vao.get());
        glDrawElements(GL_TRIANGLES, mptr->triangles.size(), GL_UNSIGNED_INT, nullptr);
        mesh_index++;
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
    // On stocke la position uniquement pour le dessin ou la rotation
    bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

    if (ctrlPressed && event->buttons() & Qt::LeftButton) {
        // Rotation : on initialise la dernière position pour la rotation
        m_last_rot_position = event->pos();
    } else if (!(ctrlPressed) && event->buttons() & Qt::LeftButton) {
        // Dessin
        m_drawing = true;
        QVector3D pointOnPlane = screenPosToPlane(event->pos());
        drawOnHeightmap(pointOnPlane, false);
    } else if (!(ctrlPressed) && event->buttons() & Qt::RightButton) {
        // Dessin
        m_drawing = true;
        QVector3D pointOnPlane = screenPosToPlane(event->pos());
        drawOnHeightmap(pointOnPlane, true);
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

    if (ctrlPressed && (event->buttons() & Qt::LeftButton)) {
        // Rotation
        int dx = event->x() - m_last_rot_position.x();
        int dy = event->y() - m_last_rot_position.y();

        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);

        // Mise à jour de la dernière position de rotation
        m_last_rot_position = event->pos();
    }
    else if (ctrlPressed && (event->buttons() & Qt::RightButton)) {
        // Pan
        int dx = event->x() - m_last_rot_position.x();
        int dy = event->y() - m_last_rot_position.y();

        m_tx += dx * 0.01f;
        m_ty -= dy * 0.01f;

        // Mise à jour de la dernière position
        m_last_rot_position = event->pos();
        update();
    }
    else if (m_drawing && (event->buttons() & Qt::LeftButton)) {
        // Dessin
        QVector3D pointOnPlane = screenPosToPlane(event->pos());
        drawOnHeightmap(pointOnPlane, false);
    }
    else if (m_drawing && (event->buttons() & Qt::RightButton)) {
        // Dessin
        QVector3D pointOnPlane = screenPosToPlane(event->pos());
        drawOnHeightmap(pointOnPlane, true);
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_drawing = false;
}

void GLWidget::wheelEvent(QWheelEvent *event){
    m_zoom += event->angleDelta().y() * 0.001f;
    update();
}

#include <limits>

QVector3D GLWidget::screenPosToPlane(const QPoint &pos)
{
    if (scene_meshes.empty()) return QVector3D();

    // 1️⃣ Calculer le rayon depuis l'écran
    float nx = 2.0f * pos.x() / width() - 1.0f;
    float ny = 1.0f - 2.0f * pos.y() / height();

    QVector4D ray_clip(nx, ny, -1.0f, 1.0f);
    QVector4D ray_eye = m_projection.inverted() * ray_clip;
    ray_eye.setZ(-1.0f);
    ray_eye.setW(0.0f);

    QMatrix4x4 view = m_view;
    view.rotate(m_xRot / 16.0f, 1,0,0);
    view.rotate(m_yRot / 16.0f, 0,1,0);
    view.rotate(m_zRot / 16.0f, 0,0,1);

    QVector4D ray_world4 = view.inverted() * ray_eye;
    QVector3D ray_world(ray_world4.x(), ray_world4.y(), ray_world4.z());
    ray_world.normalize();

    QVector3D cam_pos = (view.inverted() * QVector4D(0,0,0,1)).toVector3D();

    // 2️⃣ Parcourir les triangles du mesh pour trouver intersection
    Mesh* mesh = nullptr;
    for (auto &m : scene_meshes) {
        if (m->has_heightmap) {
            mesh = m.get();
            break;
        }
    }
    if (!mesh) return QVector3D();

    QVector3D closest_point;
    float closest_t = std::numeric_limits<float>::max();

    // Pour chaque triangle
    for (size_t i = 0; i + 2 < mesh->triangles.size(); i += 3) {
        QVector3D v0 = mesh->vertices[mesh->triangles[i]];
        QVector3D v1 = mesh->vertices[mesh->triangles[i+1]];
        QVector3D v2 = mesh->vertices[mesh->triangles[i+2]];

        float t, u, v;
        if (rayIntersectsTriangle(cam_pos, ray_world, v0, v1, v2, t, u, v)) {
            if (t < closest_t) {
                closest_t = t;
                closest_point = cam_pos + t * ray_world;
            }
        }
    }

    return closest_point;
}

// Fonction de test intersection rayon-triangle
bool GLWidget::rayIntersectsTriangle(
    const QVector3D &orig, const QVector3D &dir,
    const QVector3D &v0, const QVector3D &v1, const QVector3D &v2,
    float &t, float &u, float &v)
{
    const float EPSILON = 1e-6f;
    QVector3D edge1 = v1 - v0;
    QVector3D edge2 = v2 - v0;
    QVector3D h = QVector3D::crossProduct(dir, edge2);
    float a = QVector3D::dotProduct(edge1, h);
    if (fabs(a) < EPSILON)
        return false; // rayon parallèle au triangle
    float f = 1.0f / a;
    QVector3D s = orig - v0;
    u = f * QVector3D::dotProduct(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;
    QVector3D q = QVector3D::crossProduct(s, edge1);
    v = f * QVector3D::dotProduct(dir, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;
    t = f * QVector3D::dotProduct(edge2, q);
    if (t > EPSILON)
        return true;
    else
        return false;
}




void GLWidget::drawOnHeightmap(const QVector3D &point, bool invert)
{
    if (scene_meshes.empty()) return;

//    Mesh *mesh = nullptr;
//    for (auto &m : scene_meshes) {
//        if (m->has_heightmap) {
//            mesh = m.get();
//            break;
//        }
//    }
//    if (!mesh || !mesh->heightmap) return;

    Mesh *mesh = scene_meshes[activeMeshIndex].get();
    if (!mesh || !mesh->heightmap) return;

    QImage &img = mesh->heightmapImage;

    float sizeX = 10.0f;
    float sizeZ = 10.0f;

    int x = ((point.x() + sizeX / 2.0f) / sizeX) * img.width();
    int y = ((point.z() + sizeZ / 2.0f) / sizeZ) * img.height();

    x = qBound(0, x, img.width() - 1);
    y = qBound(0, y, img.height() - 1);

    int delta = invert ? -m_brush_strength : m_brush_strength;

    for (int i = -m_brush_radius; i <= m_brush_radius; ++i) {
        for (int j = -m_brush_radius; j <= m_brush_radius; ++j) {
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < img.width() && ny >= 0 && ny < img.height()) {
                int value = qGray(img.pixel(nx, ny));
                value = qBound(0, value + delta, 255);
                img.setPixel(nx, ny, qRgb(value, value, value));
            }
        }
    }

    mesh->heightmap->destroy();
    mesh->heightmap->setData(img);
    mesh->heightmap->bind();

    emit HeightmapChanged(activeMeshIndex, mesh->heightmapImage);

    update();
}

void GLWidget::setActiveMesh(int index)
{
    if (index >= 0 && index < static_cast<int>(scene_meshes.size())){
        activeMeshIndex = index;
    }
}



void GLWidget::addMesh(std::unique_ptr<Mesh> mesh, bool perlin)
{
    if (!mesh || !mesh->valid) return;

    makeCurrent();
    initializeOpenGLFunctions();
    mesh->bindBuffers();

    scene_meshes.push_back(std::move(mesh));
    Mesh* mptr = scene_meshes.back().get();

    // AUTO : si le mesh veut une heightmap et qu’elle n’est pas encore créée
    if (mptr->has_heightmap && mptr->heightmap == nullptr) {

        QImage img;

        if(!perlin){
            // img = QImage(":/textures/heightmap.png");
            img = QImage(512, 512, QImage::Format_Grayscale8);
            img.fill(0);
        } else {
            img = mptr->heightmapImage;
        }

        if (img.isNull()) {
            qWarning() << "heightmap image is null!";
        } else {

            mptr->heightmapImage = img.convertToFormat(QImage::Format_Grayscale8);
            mptr->heightmap = new QOpenGLTexture(img);
            mptr->heightmap->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            mptr->heightmap->setMagnificationFilter(QOpenGLTexture::Linear);
            mptr->heightmap->generateMipMaps();
        }
    }

    // chargement de l'albedo
    if (!mptr->textureAlbedo.isNull()){
        mptr->albedo = new QOpenGLTexture(mptr->textureAlbedo);
        mptr->albedo->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        mptr->albedo->setMagnificationFilter(QOpenGLTexture::Linear);
        mptr->albedo->generateMipMaps();
    }

    doneCurrent();
    update();
}

const std::vector<std::unique_ptr<Mesh>>& GLWidget::get_scene_meshes() const
{
    return scene_meshes;
}
