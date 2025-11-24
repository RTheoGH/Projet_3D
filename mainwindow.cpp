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
#include "glwidget.h"
#include "maillage.h"
#include "meshdialog.h"

MainWindow::MainWindow()
{
    resize(1400, 1000);
    setMinimumSize(1400, 1000);
    //showMaximized();
    //QMenuBar *menuBar = new QMenuBar;
    //setMenuBar(menuBar);
    //QMenu *menuWindow = menuBar->addMenu(tr("&Window"));
    //QMenu *menuWindow = menuBar->addMenu(QIcon(":/icons/open.png"),"");


    //QMenu *fileMenu = menuBar->addMenu(tr("&Fichier"));
    //QAction *loadMeshAction = new QAction(tr("Charger un maillage"), this);
    //fileMenu->addAction(loadMeshAction);
    //connect(loadMeshAction, &QAction::triggered, this, &MainWindow::loadMesh);

    QToolBar *toolbar1 = addToolBar("Mesh");
    toolbar1->setIconSize(QSize(24,24));

    //QMenu *main = menuBar->addMenu("");

    QAction *newMeshAction = new QAction(QIcon(":/icons/open.png"), tr("Nouveau terrain"), this);
    connect(newMeshAction, &QAction::triggered, this, &MainWindow::openMeshDialog);
    toolbar1->addAction(newMeshAction);

    QAction *openFileAction = new QAction(QIcon(":/icons/open_file.png"), tr("Ouvrir un terrain"), this);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::loadFile);
    toolbar1->addAction(openFileAction);

    QAction *saveAction = new QAction(QIcon(":/icons/save.png"), tr("Sauvegarder le terrain"), this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
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

    QAction *sand = new QAction(QIcon(":/icons/sand.png"), tr("Sand"), this);
    QAction *water = new QAction(QIcon(":/icons/water.png"), tr("Water"), this);
    QAction *lava  = new QAction(QIcon(":/icons/lava.png"), tr("Lava"), this);

    sand->setCheckable(true);
    water->setCheckable(true);
    lava->setCheckable(true);

    drawGroup->addAction(sand);
    drawGroup->addAction(water);
    drawGroup->addAction(lava);

    sand->setChecked(true);

    toolbar2->addAction(sand);
    toolbar2->addAction(water);
    toolbar2->addAction(lava);

    addToolBar(Qt::LeftToolBarArea, toolbar2);

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

//    QMenu *draw = menuBar->addMenu("");

//    QAction *sand = new QAction(QIcon(":/icons/sand.png"), tr("sand"),this);
//    QAction *water = new QAction(QIcon(":/icons/water.png"), tr("water"),this);
//    QAction *lava = new QAction(QIcon(":/icons/lava.png"), tr("lava"),this);

//    draw->addAction(sand);
//    draw->addAction(water);
//    draw->addAction(lava);


    onAddNew();

    Window* w = qobject_cast<Window*>(centralWidget());
    if (w) {
        GLWidget* gl = w->get_glWidget();
        connect(radiusSlider, &QSlider::valueChanged, gl, &GLWidget::setBrushRadius);
        connect(strengthSlider, &QSlider::valueChanged, gl, &GLWidget::setBrushStrength);
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
    // QString fileName = QFileDialog::getOpenFileName(
    //     this,
    //     tr("Ouvrir un maillage"),
    //     "",
    //     tr("OFF Files (*.off);;All Files (*)")
    // );
    // if (fileName.isEmpty())
    //     return;

    Window *w = qobject_cast<Window*>(centralWidget());
    if (!w)
        return;

    // Mesh m;
    // if (!m.loadOFF(fileName)) {
    //     QMessageBox::warning(this, tr("Erreur"), tr("Impossible de charger le maillage."));
    //     return;
    // }

    auto p = std::make_unique<Plane>(10, 10, 32, 32);

    w->get_glWidget()->addMesh(std::move(p));
}

void MainWindow::loadFile()
{
    // ouvrir un fichier (3 heightmaps (png)) (si pas de plan affiché en créer un selon la taille des heightmaps? sinon remplacer les heightmaps du plan actuel)
    // QString fileName = QFileDialog::getOpenFileName(this,tr("Ouvrir un maillage"),"",tr("OFF Files (*.off);;All Files (*)"));
    // if (fileName.isEmpty())
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
    // sauvegarder les 3 heightmaps (png) dans un fichier ?;
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

        Window *w = qobject_cast<Window*>(centralWidget());
        if (!w)
            return;

        auto p = std::make_unique<Plane>(10, 10, size, size, perlin);
        w->get_glWidget()->addMesh(std::move(p));
    }
}

void MainWindow::infos()
{
    return;
}
