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

#include "mainwindow.h"
#include "window.h"
#include <QMenuBar>
#include <QToolBar>
#include <QMenu>
#include <QSlider>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QCheckBox>
#include "glwidget.h"
#include "maillage.h"
#include "meshdialog.h"
#include "SimplexNoise.h"
#include <QRgb>

MainWindow::MainWindow()
{
    resize(1400, 1000);
    setMinimumSize(1400, 1000);

    QToolBar *toolbar1 = addToolBar("Mesh");
    toolbar1->setIconSize(QSize(24,24));

    QAction *newMeshAction = new QAction(QIcon(":/icons/open.png"), tr("Nouveau terrain"), this);
    connect(newMeshAction, &QAction::triggered, this, &MainWindow::openMeshDialog);
    toolbar1->addAction(newMeshAction);

    QAction *openFileAction = new QAction(QIcon(":/icons/open_file.png"), tr("Ouvrir un terrain"), this);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::loadTerrainBinary);
    toolbar1->addAction(openFileAction);

    QAction *saveAction = new QAction(QIcon(":/icons/save.png"), tr("Sauvegarder le terrain"), this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveTerrainBinary);
    toolbar1->addAction(saveAction);

    QAction *helpAction = new QAction(QIcon(":/icons/help.png"), tr("Informations"), this);
    connect(helpAction, &QAction::triggered, this, &MainWindow::infos);
    toolbar1->addAction(helpAction);

    addToolBarBreak();

    QToolBar *toolbar2 = new QToolBar("Draw Tools");
    toolbar2->setOrientation(Qt::Vertical);
    toolbar2->setIconSize(QSize(48,48));

    QActionGroup *drawGroup = new QActionGroup(this);
    drawGroup->setExclusive(true);

    sandAction = new QAction(QIcon(":/icons/sand.png"), tr("Sand"), this);
    waterAction = new QAction(QIcon(":/icons/water.png"), tr("Water"), this);
    dirtAction  = new QAction(QIcon(":/icons/dirt.png"), tr("Dirt"), this);

    sandAction->setCheckable(true);
    waterAction->setCheckable(true);
    dirtAction->setCheckable(true);

    drawGroup->addAction(sandAction);
    drawGroup->addAction(waterAction);
    drawGroup->addAction(dirtAction);

    sandAction->setChecked(true);

    toolbar2->addAction(sandAction);
    toolbar2->addAction(waterAction);
    toolbar2->addAction(dirtAction);

    addToolBar(Qt::LeftToolBarArea, toolbar2);

    toolbar2->addSeparator();

    QActionGroup *brushGroup = new QActionGroup(this);
    brushGroup->setExclusive(true);

    QAction *square = new QAction(QIcon(":/icons/square.png"), tr("Square"), this);
    QAction *circle = new QAction(QIcon(":/icons/circle.png"), tr("Circle"), this);

    square->setCheckable(true);
    circle->setCheckable(true);

    brushGroup->addAction(square);
    brushGroup->addAction(circle);

    square->setChecked(true);

    toolbar2->addAction(square);
    toolbar2->addAction(circle);

    toolbar2->addSeparator();

    QWidget *sliderWidget = new QWidget;
    QVBoxLayout *sliderLayout = new QVBoxLayout;
    sliderLayout->setContentsMargins(5,5,5,5);

    QLabel *radiusLabel = new QLabel("Brush Radius");
    QSlider *radiusSlider = new QSlider(Qt::Horizontal);
    radiusSlider->setRange(1, 100);
    radiusSlider->setValue(20);
    sliderLayout->addWidget(radiusLabel);
    sliderLayout->addWidget(radiusSlider);

    QLabel *strengthLabel = new QLabel("Brush Strength");
    QSlider *strengthSlider = new QSlider(Qt::Horizontal);
    strengthSlider->setRange(1, 50);
    strengthSlider->setValue(20);
    sliderLayout->addWidget(strengthLabel);
    sliderLayout->addWidget(strengthSlider);

    sliderWidget->setLayout(sliderLayout);
    toolbar2->addWidget(sliderWidget);

    toolbar2->addSeparator();

    QAction *undo = new QAction(QIcon(":/icons/undo.png"), tr("Undo"), this);
    undo->setShortcut(QKeySequence::Undo);
    toolbar2->addAction(undo);

    QCheckBox *erosion = new QCheckBox("Erosion");
    erosion->setChecked(true);
    toolbar2->addWidget(erosion);

    onAddNew();

    Window* w = qobject_cast<Window*>(centralWidget());
    if (w) {
        GLWidget* gl = w->get_glWidget();
        connect(radiusSlider, &QSlider::valueChanged, gl, &GLWidget::setBrushRadius);
        connect(strengthSlider, &QSlider::valueChanged, gl, &GLWidget::setBrushStrength);
        connect(radiusSlider, &QSlider::valueChanged, w, &Window::setBrushRadius);
        connect(strengthSlider, &QSlider::valueChanged, w, &Window::setBrushStrength);

        connect(sandAction, &QAction::triggered, this, [gl]() { gl->setActiveMesh(0); });
        connect(waterAction, &QAction::triggered, this, [gl]() { gl->setActiveMesh(1); });
        connect(dirtAction, &QAction::triggered, this, [gl]() { gl->setActiveMesh(2); });

        connect(square, &QAction::triggered, gl, &GLWidget::setBrushShapeSquare);
        connect(circle, &QAction::triggered, gl, &GLWidget::setBrushShapeCircle);
        connect(square, &QAction::triggered, w, &Window::setBrushShapeSquare);
        connect(circle, &QAction::triggered, w, &Window::setBrushShapeCircle);

        connect(undo, &QAction::triggered, this, [gl]() {
            gl->undoLastDraw();
        });

        connect(erosion, &QCheckBox::toggled, gl, &GLWidget::setErosionEnabled);
    }
}

void MainWindow::onAddNew()
{
    if (!centralWidget())
        setCentralWidget(new Window(this));
    else
        QMessageBox::information(0, tr("Cannot add new window"), tr("Already occupied. Undock first."));
}

void MainWindow::loadMesh()
{
    Window *w = qobject_cast<Window*>(centralWidget());
    if (!w)
        return;

    std::cout<<"allo1"<<std::endl;
    auto p = std::make_unique<Plane>(10, 10, 32, 32);
    w->get_glWidget()->addMesh(std::move(p), false);
    std::cout<<"allo1"<<std::endl;
    auto p2 = std::make_unique<Plane>(10, 10, 32, 32);
    w->get_glWidget()->addMesh(std::move(p2), false);
    std::cout<<"allo2"<<std::endl;
    auto p3 = std::make_unique<Plane>(10, 10, 32, 32);
    w->get_glWidget()->addMesh(std::move(p3), false);
    std::cout<<"allo3"<<std::endl;
}

void MainWindow::loadFile()
{
    // old
    return;

    QString dir = QFileDialog::getExistingDirectory(this, "Charger un terrain");
    if (dir.isEmpty())
        return;

    Window* w = qobject_cast<Window*>(centralWidget());
    if (!w)
        return;

    GLWidget* gl = w->get_glWidget();
    const std::vector<std::unique_ptr<Mesh>>& meshes = gl->get_scene_meshes();

    if (meshes.empty())
        return;

    Mesh* m = meshes[0].get();

    if (!m->loadAllHeightmaps(dir)) {
        QMessageBox::warning(this, "Erreur", "Impossible de charger !");
        return;
    }

    gl->update();
    QMessageBox::information(this, "OK", "Terrain chargé !");
}

void MainWindow::saveFile()
{
    // old
    return;

    QString dir = QFileDialog::getExistingDirectory(this, "Sauvegarder le terrain");
    if (dir.isEmpty())
        return;

    Window* w = qobject_cast<Window*>(centralWidget());
    if (!w)
        return;

    GLWidget* gl = w->get_glWidget();
    const std::vector<std::unique_ptr<Mesh>>& meshes = gl->get_scene_meshes();

    if (meshes.empty())
        return;

    Mesh* m = meshes[0].get();

    if (!m->saveAllHeightmaps(dir)) {
        QMessageBox::warning(this, "Erreur", "Impossible de sauvegarder !");
        return;
    }

    QMessageBox::information(this, "OK", "Terrain sauvegardé !");
}

void MainWindow::openMeshDialog()
{
    MeshDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        int size = dlg.meshSize();
        bool perlin = dlg.usePerlinNoise();
        int octaves = dlg.octaves();
        double frequency = dlg.frequency();

        Window *w = qobject_cast<Window*>(centralWidget());
        if (!w)
            return;

        GLWidget *gl = w->get_glWidget();
        gl->clearAllMeshes();
        gl->setActiveMesh(0);
        sandAction->setChecked(true);

        auto p = std::make_unique<Plane>(10, 10, size, size, ":/textures/sand_texture.png");
        auto p2 = std::make_unique<Plane>(10, 10, size, size, ":/textures/water_texture.png");
        auto p3 = std::make_unique<Plane>(10, 10, size, size, ":/textures/dirt_texture.png");

        if(perlin){
            QImage perlin_image = loadPerlinNoiseImage(octaves, frequency);
            p->heightmapImage = perlin_image;
            p2->heightmapImage = perlin_image;
            p3->heightmapImage = perlin_image;
            emit InitHeightmaps(0, perlin_image);
            emit InitHeightmaps(1, perlin_image);
            emit InitHeightmaps(2, perlin_image);
        }
        else{
            QImage newHM = QImage(250, 250, QImage::Format_Grayscale8).scaled(250, 250, Qt::KeepAspectRatio);
            newHM.fill(0);
            emit InitHeightmaps(0, newHM);
            emit InitHeightmaps(1, newHM);
            emit InitHeightmaps(2, newHM);
        }

        w->get_glWidget()->addMesh(std::move(p), perlin);
        w->get_glWidget()->addMesh(std::move(p2), perlin);
        w->get_glWidget()->addMesh(std::move(p3), perlin);
    }
}

QImage MainWindow::loadPerlinNoiseImage(int octaves, double frequency)
{
    float scale     = 1.0f / frequency;
    float offset_x  = 0.0f; // 5.9f;
    float offset_y  = 0.0f; // 5.1f;
    float offset_z  = 0.0f; // 0.05f;
    float lacunarity    = 1.99f;
    float persistance   = 0.5f;

    const SimplexNoise simplex(0.1f/scale, 0.5f, lacunarity, persistance);

    QImage res_image(512, 512, QImage::Format_RGB32);

    for(int i=0 ; i<res_image.height(); i++){
        const float y = static_cast<float>(i - res_image.height()/2 + offset_y);
        for(int j=0 ; j<res_image.width(); j++){

            const float x = static_cast<float>(j - res_image.width()/2 + offset_x);
            const int noise = (simplex.fractal(octaves, x, y) + 1.0)/2.0 * 255.0;

            const QRgb pixel_value = qRgb(noise, noise, noise);
            res_image.setPixel(i, j, pixel_value);

        }
    }

    return res_image;
}

bool saveHeightmapsBinary(const QImage& img1, const QImage& img2, const QImage& img3, const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    out.writeRawData("HMAP", 4);
    out << quint32(1);
    out << quint32(3);

    auto writeImage = [&](const QImage& img)
    {
        QImage saved = img.convertToFormat(QImage::Format_Grayscale8);

        quint32 w = saved.width();
        quint32 h = saved.height();
        quint32 bpp = saved.bytesPerLine() / saved.width();
        quint32 dataSize = saved.sizeInBytes();

        out << w;
        out << h;
        out << bpp;
        out << dataSize;

        out.writeRawData(
            reinterpret_cast<const char*>(saved.bits()),
            dataSize
        );
    };

    writeImage(img1);
    writeImage(img2);
    writeImage(img3);

    file.close();
    return true;
}

bool loadHeightmapsBinary(QImage& img1, QImage& img2, QImage& img3, const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);

    char signature[4];
    in.readRawData(signature, 4);

    if (strncmp(signature, "HMAP", 4) != 0)
        return false;

    quint32 version, imageCount;
    in >> version;
    in >> imageCount;

    if (version != 1 || imageCount != 3)
        return false;

    auto readImage = [&](QImage& img)
    {
        quint32 w, h, bpp, dataSize;
        in >> w;
        in >> h;
        in >> bpp;
        in >> dataSize;

        img = QImage(w, h, QImage::Format_Grayscale8);

        in.readRawData(
            reinterpret_cast<char*>(img.bits()),
            dataSize
        );
    };

    readImage(img1);
    readImage(img2);
    readImage(img3);

    file.close();
    return true;
}

void MainWindow::saveTerrainBinary()
{
    QString filename = QFileDialog::getSaveFileName(
        this,"Sauvegarder le terrain","","Heightmap (*.hmap)"
    );

    if (filename.isEmpty())
        return;

    Window* w = qobject_cast<Window*>(centralWidget());
    if (!w) return;

    GLWidget* gl = w->get_glWidget();
    const auto& meshes = gl->get_scene_meshes();

    if (meshes.size() < 3) {
        QMessageBox::warning(this, "Erreur", "Il faut 3 terrains !");
        return;
    }

    QImage& img1 = meshes[0]->heightmapImage;
    QImage& img2 = meshes[1]->heightmapImage;
    QImage& img3 = meshes[2]->heightmapImage;

    if (!saveHeightmapsBinary(img1, img2, img3, filename)) {
        QMessageBox::critical(this, "Erreur", "Échec sauvegarde !");
        return;
    }

    QMessageBox::information(this, "OK", "Terrain sauvegardé !");
}

void MainWindow::loadTerrainBinary()
{
    QString filename = QFileDialog::getOpenFileName(
        this,"Charger un terrain","","Heightmap (*.hmap)"
    );

    if (filename.isEmpty())
        return;

    Window* w = qobject_cast<Window*>(centralWidget());
    if (!w) return;

    GLWidget* gl = w->get_glWidget();
    const auto& meshes = gl->get_scene_meshes();

    if (meshes.size() < 3) {
        QMessageBox::warning(this, "Erreur", "Il faut 3 terrains !");
        return;
    }

    if (!loadHeightmapsBinary(meshes[0]->heightmapImage,
                              meshes[1]->heightmapImage,
                              meshes[2]->heightmapImage,
                              filename)) {
        QMessageBox::critical(this, "Erreur", "Échec du chargement !");
        return;
    }

    // qDebug() << meshes[0]->heightmapImage.size();
    // qDebug() << meshes[1]->heightmapImage.size();
    // qDebug() << meshes[2]->heightmapImage.size();

    emit SceneLoaded(meshes[0]->heightmapImage, meshes[1]->heightmapImage, meshes[2]->heightmapImage);

    gl->update();
    QMessageBox::information(this, "OK", "Terrain chargé !");
}

void MainWindow::infos()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("À propos - Éditeur de Terrain");
    aboutBox.setIconPixmap(QPixmap(":/icons/help.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QString infoText =
        "<h2>Éditeur de Terrain 3D</h2>"
        "<p><b>Version:</b> 1.0</p>"
        "<p><b>Date:</b> Décembre 2025</p>"
        "<hr>"
        "<h3>Fonctionnalités:</h3>"
        "<ul>"
        "<li>Création de terrains (sans/avec bruit de Perlin)</li>"
        "<li>3 types de matériaux (Sable, Eau, Terre)</li>"
        "<li>Pinceau circulaire ou carré</li>"
        "<li>Système d'érosion</li>"
        "<li>Sauvegarde/Chargement de terrains</li>"
        "</ul>"
        "<hr>"
        "<h3>Contrôles:</h3>"
        "<ul>"
        "<li><b>Clic gauche:</b> Dessiner sur le terrain</li>"
        "<li><b>Clic droit:</b> Effacer sur le terrain</li>"
        "<li><b>Clic gauche + glisser:</b> Rotation de la caméra</li>"
        "<li><b>Clic droit + glisser:</b> Déplacement de la caméra</li>"
        "<li><b>Molette:</b> Zoom avant/arrière</li>"
        "<li><b>Ctrl+Z:</b> Annuler la dernière action</li>"
        "</ul>"
        "<hr>"
        "<p><i>Développé avec Qt et OpenGL</i></p>"
        "<p>© VIGUIER Killian & REYNIER Théo</p>";

    aboutBox.setText(infoText);
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}
