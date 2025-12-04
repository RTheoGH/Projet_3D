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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_3_Core>
#include <QMatrix4x4>
#include "maillage.h"
#include <memory>
#include <vector>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    static bool isTransparent() { return m_transparent; }
    static void setTransparent(bool t) { m_transparent = t; }

    void addMesh(std::unique_ptr<Mesh> mesh, bool perlin);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    const std::vector<std::unique_ptr<Mesh>>& get_scene_meshes() const;

public slots:
    //Completer : ajouter des slots pour signaler appliquer le changement de rotation
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void cleanup();
    void setBrushRadius(int radius) { m_brush_radius = radius; }
    void setBrushStrength(int strength) { m_brush_strength = strength; }
    void setActiveMesh(int index);
    void onHeightmapsChanged(QImage hm_sand, QImage hm_water, QImage hm_lava);
    void onHeightmapChanged(int hm_index, QImage hm);

    void setBrushShapeSquare() { m_brushShape = "Square"; }
    void setBrushShapeCircle() { m_brushShape = "Circle"; }
    void timerEvent(QTimerEvent*) override;

signals:

    //Completer : ajouter des signaux pour signaler des changement de rotation
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void HeightmapChanged(int material_index, QImage hm);

protected:
    void initializeGL() override;
    void uploadMeshToGPU(Mesh& m);
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void drawOnHeightmap(const QVector3D &point, bool invert);
    QVector3D screenPosToPlane(const QPoint &pos);
    bool rayIntersectsTriangle(
        const QVector3D &orig, const QVector3D &dir,
        const QVector3D &v0, const QVector3D &v1, const QVector3D &v2,
        float &t, float &u, float &v);
    void drawBrushPreview();
private:
    // void setupVertexAttribs();

    bool m_core;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    QPoint m_last_position;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *m_compute = nullptr;
    QOpenGLContext *m_context = nullptr;
    QOpenGLFunctions_4_3_Core *f = nullptr;
    bool runCompute = true;
    int m_mvp_matrix_loc;
    int m_model_matrix_loc;
    int m_normal_matrix_loc;
    int m_light_pos_loc;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;
    static bool m_transparent;

    GLint textureUniform = -1;

    std::vector<std::unique_ptr<Mesh>> scene_meshes;

    float m_tx = 0.0f;
    float m_ty = 0.0f;
    float m_zoom = -20.0f;

    QPoint m_last_rot_position;
    QPoint m_las_mouse_pos;
    int m_brush_radius = 20;
    int m_brush_strength = 20;
    bool m_drawing;

    int activeMeshIndex = 0;

    QString m_brushShape = "Square";
    QVector3D m_brushPreviewPos;
    bool m_showBrushPreview = false;
};

#endif
