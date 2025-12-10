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
#include <QPainter>
#include <QTimer>

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
    // if (m_transparent) {
    //     QSurfaceFormat fmt = format();
    //     fmt.setAlphaBufferSize(8);
    //     setFormat(fmt);
    // }
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
        if (mptr->has_heightmap) {
            delete mptr->heightmapA;
            delete mptr->heightmapB;
            mptr->heightmapA = nullptr;
            mptr->heightmapB = nullptr;
        }
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_program = new QOpenGLShaderProgram(this);
    m_compute = new QOpenGLShaderProgram(this);
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl"))
        close();

    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl"))
        close();

    if (!m_compute->addShaderFromSourceFile(QOpenGLShader::Compute, ":/shaders/cshader.glsl"))
        close();

    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("uv", 2);

    if (!m_program->link())
        close();

    if (!m_compute->link()){
        qDebug() << "marche pas le compute ff";
        close();
    }

    if (!m_program->bind())
        close();

    if (!m_compute->bind())
        close();

    f = context()->versionFunctions<QOpenGLFunctions_4_3_Core>();
    if (!f) {
        runCompute = false;
        qWarning() << "Compute shader impossible.";
    }
    else{
        runCompute = true;
        f->initializeOpenGLFunctions();
    }

    // glActiveTexture(GL_TEXTURE4);
    // int locAlbedo = m_program->uniformLocation("albedo");
    // if (locAlbedo >= 0) m_program->setUniformValue(locAlbedo, 4); // ex unité 4

    // int locCurrentHm = m_program->uniformLocation("current_hm");
    // if (locCurrentHm >= 0) m_program->setUniformValue(locCurrentHm, 0); // unité 0

    m_mvp_matrix_loc = m_program->uniformLocation("mvp_matrix");
    m_model_matrix_loc = m_program->uniformLocation("model_matrix");
    m_normal_matrix_loc = m_program->uniformLocation("normal_matrix");
    m_light_pos_loc = m_program->uniformLocation("light_position");

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_view.setToIdentity();
    m_view.setToIdentity();
    m_view.translate(0,0,m_zoom);

    m_program->setUniformValue(m_light_pos_loc, QVector3D(0, 10, 0));

    m_program->release();

    QImage grassImg(":/textures/grass_texture.png");
    if (!grassImg.isNull()) {
        m_grassTexture = new QOpenGLTexture(grassImg);
        m_grassTexture->setAutoMipMapGenerationEnabled(true);
        m_grassTexture->generateMipMaps();
        m_grassTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        m_grassTexture->setMagnificationFilter(QOpenGLTexture::Linear);
        m_grassTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
        qDebug() << "Texture d'herbe chargée !";
    } else {
        qDebug() << "Impossible de charger grass_texture.png";
    }

    startTimer(16);
}

void GLWidget::timerEvent(QTimerEvent*){
    // int mesh_index = 0;
    // for(auto &mesh : scene_meshes){
    //     emit HeightmapChanged(mesh_index, mesh->heightmapImage);
    //     mesh_index++;
    // }

    update();
}

// void GLWidget::uploadMeshToGPU(Mesh& m)
// {
//     makeCurrent();
//     m.bindBuffers();
// }


void GLWidget::paintGL()
{

    static int paintCount = 0;
    if (paintCount < 5) {  // Log seulement les 5 premiers renders
        qDebug() << "=== paintGL #" << paintCount << "===";
    }
    paintCount++;

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
    m_program->setUniformValue(m_model_matrix_loc, m_model);

    QOpenGLTexture* texForRender;

    std::vector<int> renderOrder = {0, 2, 1};

    for (int mesh_index : renderOrder)
    {
        if (mesh_index >= scene_meshes.size()) continue;
        auto &mptr = scene_meshes[mesh_index];
        if (!mptr || !mptr->gpu_uploaded) continue;

        if (mptr->has_heightmap && f && runCompute && m_compute) {

            QOpenGLTexture* readTexCurrent  = mptr->isInputA ? mptr->heightmapA : mptr->heightmapB;
            QOpenGLTexture* writeTex = mptr->isInputA ? mptr->heightmapB : mptr->heightmapA;

            m_compute->bind();

            f->glBindImageTexture(0, readTexCurrent->textureId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
            f->glBindImageTexture(1, writeTex->textureId(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);

            QOpenGLTexture* readTexSand  = scene_meshes[0]->isInputA ? scene_meshes[0]->heightmapA : scene_meshes[0]->heightmapB;
            QOpenGLTexture* readTexWater = scene_meshes[1]->isInputA ? scene_meshes[1]->heightmapA : scene_meshes[1]->heightmapB;
            QOpenGLTexture* readTexLava  = scene_meshes[2]->isInputA ? scene_meshes[2]->heightmapA : scene_meshes[2]->heightmapB;

            f->glBindImageTexture(2, readTexSand->textureId(),  0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
            f->glBindImageTexture(3, readTexWater->textureId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
            f->glBindImageTexture(4, readTexLava->textureId(),  0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);

            m_compute->setUniformValue(m_compute->uniformLocation("hm_index"), mesh_index);

            float moy_pix = 0.0;
            for(int i = 0; i<mptr->heightmapImage.height(); i++){
                for(int j = 0; j<mptr->heightmapImage.width(); j++){
                    moy_pix += qRed(mptr->heightmapImage.pixel(i, j));
                }
            }
            moy_pix /= mptr->heightmapImage.width() * mptr->heightmapImage.height();
            moy_pix /= 255;
            m_compute->setUniformValue(m_compute->uniformLocation("moy_pix"), moy_pix);

            m_compute->setUniformValue(m_compute->uniformLocation("erosionEnabled"), m_erosionEnabled ? 1 : 0);

            int gx = (mptr->heightmapImage.width() + 15) / 16;
            int gy = (mptr->heightmapImage.height() + 15) / 16;

            f->glDispatchCompute(gx, gy, 1);
            f->glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
            m_compute->release();

            m_program->bind();

            // récup du result
            QImage result(mptr->heightmapImage.size(), QImage::Format_Grayscale8);
            f->glBindTexture(GL_TEXTURE_2D, writeTex->textureId());
            f->glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, result.bits());
            mptr->heightmapImage = result;

            mptr->isInputA = !mptr->isInputA;

            texForRender = mptr->isInputA ? mptr->heightmapA : mptr->heightmapB;

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texForRender->textureId());
            m_program->setUniformValue("current_hm", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, readTexSand->textureId());
            m_program->setUniformValue("heightmapSand", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, readTexWater->textureId());
            m_program->setUniformValue("heightmapWater", 2);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, readTexLava->textureId());
            m_program->setUniformValue("heightmapLava", 3);

            m_program->setUniformValue("hm_index", mesh_index);
            m_program->setUniformValue("height_scale", 3.0f);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, mptr->albedo->textureId());
            m_program->setUniformValue("albedo", 4);

            if (mesh_index == 2 && m_grassTexture) {
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, m_grassTexture->textureId());
                m_program->setUniformValue("grassTexture", 5);
            }
        }

        // draw mesh
        QOpenGLVertexArrayObject::Binder vaoBinder(mptr->vao.get());
        glDrawElements(GL_TRIANGLES, (GLsizei)mptr->triangles.size(), GL_UNSIGNED_INT, nullptr);
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

        qDebug() << "[MOUSE] pos écran =" << event->pos()
                 << " -> pos monde =" << pointOnPlane;

        if (!pointOnPlane.isNull()) {
            drawOnHeightmap(pointOnPlane, false);
        }
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
        if (!pointOnPlane.isNull()) {
            drawOnHeightmap(pointOnPlane, false);
        }
    }
    else if (m_drawing && (event->buttons() & Qt::RightButton)) {
        // Dessin
        QVector3D pointOnPlane = screenPosToPlane(event->pos());
        drawOnHeightmap(pointOnPlane, true);
    }

    QVector3D p = screenPosToPlane(event->pos());
    m_brushPreviewPos = p;
    m_showBrushPreview = true;
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_drawing = false;
    m_showBrushPreview = false;
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event){
    m_zoom += event->angleDelta().y() * 0.001f;
    update();
}

#include <limits>

QVector3D GLWidget::screenPosToPlane(const QPoint &pos)
{
    if (scene_meshes.empty()) return QVector3D();

    // 1️ Calculer le rayon depuis l'écran
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

    // Parcourir les triangles du mesh pour trouver intersection
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

    // float halfX = 10.0f / 2.0f; // sizeX
    // float halfZ = 10.0f / 2.0f; // sizeZ
    // if (closest_t == std::numeric_limits<float>::max()) {
    //     closest_point.setX(qBound(-halfX, cam_pos.x() + ray_world.x() * 10.0f, halfX));
    //     closest_point.setZ(qBound(-halfZ, cam_pos.z() + ray_world.z() * 10.0f, halfZ));
    //     closest_point.setY(0.0f);
    // }
    if (closest_t == std::numeric_limits<float>::max()) {
        return QVector3D();
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

    pushUndoState(activeMeshIndex);

    Mesh *mesh = scene_meshes[activeMeshIndex].get();

    if (!mesh) {
        qDebug() << "Mesh null";
        return;
    }

    if (!mesh->heightmapA || !mesh->heightmapB) {
        qDebug() << "Heightmap textures null";
        return;
    }

    QImage &img = mesh->heightmapImage;

    float sizeX = 10.0f;
    float sizeZ = 10.0f;

    int x = ((point.x() + sizeX * 0.5f) / sizeX) * img.width();
    int y = ((point.z() + sizeZ * 0.5f) / sizeZ) * img.height();

    x = qBound(0, x, img.width() - 1);
    y = qBound(0, y, img.height() - 1);


    int delta = invert ? -m_brush_strength : m_brush_strength;

    for (int i = -m_brush_radius; i <= m_brush_radius; ++i) {
        for (int j = -m_brush_radius; j <= m_brush_radius; ++j) {

            if (m_brushShape == "Circle"){
                if(i*i + j*j > m_brush_radius * m_brush_radius){
                    continue;
                }
            }

            int nx = x + i;
            int ny = y + j;


            if (nx >= 0 && nx < img.width() && ny >= 0 && ny < img.height()) {
                int px = qGray(img.pixel(nx, ny));
                px = qBound(0, px + delta, 255);
                img.setPixel(nx, ny, qRgb(px, px, px));
            }

        }
    }
    QOpenGLTexture* texToUpdate = mesh->isInputA ? mesh->heightmapA : mesh->heightmapB;

    glBindTexture(GL_TEXTURE_2D, texToUpdate->textureId());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.width(), img.height(),
                    GL_RED, GL_UNSIGNED_BYTE, img.constBits());
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) qDebug() << "OpenGL Error après upload:" << err;


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

    if (!mesh || !mesh->valid) {
        qDebug() << "Mesh invalide, retour";
        return;
    }

    makeCurrent();
    initializeOpenGLFunctions();

    mesh->bindBuffers();

    scene_meshes.push_back(std::move(mesh));
    Mesh* mptr = scene_meshes.back().get();

    if (mptr->has_heightmap && mptr->heightmapA == nullptr && mptr->heightmapB == nullptr) {

        QImage img;

        if(!perlin){
            img = QImage(512, 512, QImage::Format_Grayscale8);
            img.fill(0);
        } else {
            img = mptr->heightmapImage;
        }

        if (img.isNull()) {
            qWarning() << "heightmap image is null";
        } else {

            mptr->heightmapImage = img.convertToFormat(QImage::Format_Grayscale8);

            mptr->heightmapA = new QOpenGLTexture(QOpenGLTexture::Target2D);
            mptr->heightmapA->setSize(mptr->heightmapImage.width(), mptr->heightmapImage.height());
            mptr->heightmapA->setFormat(QOpenGLTexture::R8_UNorm);
            mptr->heightmapA->setMagnificationFilter(QOpenGLTexture::Linear);
            mptr->heightmapA->setMinificationFilter(QOpenGLTexture::Linear);
            mptr->heightmapA->setWrapMode(QOpenGLTexture::ClampToEdge);

            mptr->heightmapA->allocateStorage();

            mptr->heightmapA->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt8, mptr->heightmapImage.constBits());

            mptr->heightmapB = new QOpenGLTexture(QOpenGLTexture::Target2D);
            mptr->heightmapB->setSize(mptr->heightmapImage.width(), mptr->heightmapImage.height());
            mptr->heightmapB->setFormat(QOpenGLTexture::R8_UNorm);
            mptr->heightmapB->setMagnificationFilter(QOpenGLTexture::Linear);
            mptr->heightmapB->setMinificationFilter(QOpenGLTexture::Linear);
            mptr->heightmapB->setWrapMode(QOpenGLTexture::ClampToEdge);

            mptr->heightmapB->allocateStorage();
            mptr->heightmapB->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt8, mptr->heightmapImage.constBits());

            int mesh_index = scene_meshes.size() - 1;
            emit HeightmapChanged(mesh_index, mptr->heightmapImage);
        }
    }

    if (!mptr->textureAlbedo.isNull())
    {
        mptr->albedo = new QOpenGLTexture(mptr->textureAlbedo);
        mptr->albedo->setAutoMipMapGenerationEnabled(true);
        mptr->albedo->generateMipMaps();
        mptr->albedo->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        mptr->albedo->setMagnificationFilter(QOpenGLTexture::Linear);
        mptr->albedo->setWrapMode(QOpenGLTexture::ClampToEdge);
    }else{
        qDebug() << "Pas d'albedo";
    }

    doneCurrent();
    update();
}

const std::vector<std::unique_ptr<Mesh>>& GLWidget::get_scene_meshes() const
{
    return scene_meshes;
}

void GLWidget::onHeightmapsChanged(QImage hm_sand, QImage hm_water, QImage hm_lava){
    if(scene_meshes.size() < 3){
        qDebug() << "Pas de plane";
    } else {
        scene_meshes[0]->heightmapImage = hm_sand;
        scene_meshes[1]->heightmapImage = hm_water;
        scene_meshes[2]->heightmapImage = hm_lava;

        int mesh_count = 0;
        for(auto &mptr : scene_meshes){
            QOpenGLTexture* texToUpdate = mptr->isInputA ? mptr->heightmapA : mptr->heightmapB;

            glBindTexture(GL_TEXTURE_2D,texToUpdate->textureId());
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mptr->heightmapImage.width(), mptr->heightmapImage.height(), GL_RED, GL_UNSIGNED_BYTE, mptr->heightmapImage.constBits());
            emit HeightmapChanged(mesh_count, mptr->heightmapImage);
            mesh_count++;
        }


        update();
    }
}
void GLWidget::onHeightmapChanged(int hm_index, QImage hm)
{
    if(scene_meshes.size() <= hm_index){
        qDebug() << "Ajoutez d'abord un terrain";
        return;
    }

    if (hm.isNull()) {
        qDebug() << "Image null reçue";
        return;
    }

    makeCurrent();

    Mesh* mesh = scene_meshes[hm_index].get();
    if (!mesh || !mesh->heightmapA || !mesh->heightmapB) {
        qDebug() << "Mesh ou textures invalides";
        doneCurrent();
        return;
    }

    QOpenGLTexture* texToUpdate = mesh->isInputA ? mesh->heightmapA : mesh->heightmapB;

    if (hm.width() != mesh->heightmapImage.width() ||
        hm.height() != mesh->heightmapImage.height()) {
        qDebug() << "ERREUR : Dimensions incompatibles!"
                 << "Attendu:" << mesh->heightmapImage.width() << "x" << mesh->heightmapImage.height()
                 << "Reçu:" << hm.width() << "x" << hm.height();
        doneCurrent();
        return;
    }

    QImage finalImage = hm.convertToFormat(QImage::Format_Grayscale8);
    mesh->heightmapImage = finalImage;

    glBindTexture(GL_TEXTURE_2D, texToUpdate->textureId());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                    finalImage.width(), finalImage.height(),
                    GL_RED, GL_UNSIGNED_BYTE,
                    finalImage.constBits());

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        qDebug() << "OpenGL Error après upload:" << err;
    }

    doneCurrent();
    update();
}

void GLWidget::pushUndoState(int meshIndex)
{
    if (meshIndex < 0 || meshIndex >= scene_meshes.size())
        return;

    UndoEntry entry;
    entry.meshIndex = meshIndex;
    entry.previousImage = scene_meshes[meshIndex]->heightmapImage.copy();

    undoStack.push_back(entry);

    if (undoStack.size() > 50)
        undoStack.erase(undoStack.begin());
}

void GLWidget::undoLastDraw()
{
    if (undoStack.empty())
        return;

    UndoEntry last = undoStack.back();
    undoStack.pop_back();

    int i = last.meshIndex;

    scene_meshes[i]->heightmapImage = last.previousImage;

    emit HeightmapChanged(i, last.previousImage);
    onHeightmapChanged(i, last.previousImage);

    update();
}

void GLWidget::clearAllMeshes()
{
    makeCurrent();

    // Nettoyer tous les meshes existants
    for (auto &mptr : scene_meshes) {
        if (!mptr) continue;

        if (mptr->has_heightmap) {
            delete mptr->heightmapA;
            delete mptr->heightmapB;
            mptr->heightmapA = nullptr;
            mptr->heightmapB = nullptr;
        }

        if (mptr->vao) mptr->vao->destroy();
        if (mptr->vbo_pos) mptr->vbo_pos->destroy();
        if (mptr->vbo_norm) mptr->vbo_norm->destroy();
        if (mptr->ebo) mptr->ebo->destroy();
        if (mptr->uv_buffer) mptr->uv_buffer->destroy();

        if (mptr->albedo) {
            delete mptr->albedo;
            mptr->albedo = nullptr;
        }
    }

    scene_meshes.clear();
    undoStack.clear();
    activeMeshIndex = 0;

    doneCurrent();
    update();
}

void GLWidget::setErosionEnabled(bool enabled)
{
    m_erosionEnabled = enabled;
    update();
}

void GLWidget::drawBrushPreview()
{
    if (!m_showBrushPreview)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red, 2));

    float sizeX = 10.0f;
    float sizeZ = 10.0f;

    Mesh *mesh = scene_meshes[activeMeshIndex].get();
    if (!mesh) return;

    QImage &img = mesh->heightmapImage;

    int x = ((m_brushPreviewPos.x() + sizeX / 2.0f) / sizeX) * img.width();
    int y = ((m_brushPreviewPos.z() + sizeZ / 2.0f) / sizeZ) * img.height();

    float sx = float(width())  / img.width();
    float sy = float(height()) / img.height();

    int r = m_brush_radius * sx;

    if (m_brushShape == "Circle") {
        painter.drawEllipse(QPoint(x * sx, y * sy), r, r);
    } else {
        painter.drawRect(x * sx - r, y * sy - r, 2*r, 2*r);
    }
}
