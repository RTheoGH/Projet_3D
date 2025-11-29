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
#include "window.h"
#include "mainwindow.h"
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>

Window::Window(MainWindow *mw)
    : mainWindow(mw)
{
    glWidget = new GLWidget;

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();


    //A completer, connecter les sliders de cette classe avec le glWidget pour mettre à jour la rotation
    // et inversement
    connect(xSlider, &QSlider::valueChanged, glWidget, &GLWidget::setXRotation);
    connect(ySlider, &QSlider::valueChanged, glWidget, &GLWidget::setYRotation);
    connect(zSlider, &QSlider::valueChanged, glWidget, &GLWidget::setZRotation);

    connect(glWidget, &GLWidget::xRotationChanged, xSlider, &QSlider::setValue);
    connect(glWidget, &GLWidget::yRotationChanged, ySlider, &QSlider::setValue);
    connect(glWidget, &GLWidget::zRotationChanged, zSlider, &QSlider::setValue);

    connect(glWidget, &GLWidget::HeightmapChanged, this, &Window::updateHeightmapLabel);
    connect(mw, &MainWindow::InitHeightmaps, this, &Window::updateHeightmapLabel);
    connect(mw, &MainWindow::SceneLoaded, glWidget, &GLWidget::onHeightmapsChanged);

    connect(this, &Window::onPixmapChanged, glWidget, &GLWidget::onHeightmapChanged);

    QHBoxLayout *mainLayout = new QHBoxLayout;

    QVBoxLayout *heightmaps = new QVBoxLayout;

    sand_h = new QLabel;
    water_h = new QLabel;
    lava_h = new QLabel;

    sand_h->setPixmap(QPixmap::fromImage(sandImage));
    sand_h->installEventFilter(this);

    water_h->setPixmap(QPixmap::fromImage(waterImage));
    water_h->installEventFilter(this);

    lava_h->setPixmap(QPixmap::fromImage(lavaImage));
    lava_h->installEventFilter(this);

    heightmaps->addWidget(sand_h);
    heightmaps->addWidget(water_h);
    heightmaps->addWidget(lava_h);

    mainLayout->addWidget(glWidget, 1);
    mainLayout->addLayout(heightmaps);

//    QHBoxLayout *container = new QHBoxLayout;
//    container->addWidget(glWidget);
///    container->addWidget(xSlider);
////    container->addWidget(ySlider);
////    container->addWidget(zSlider);

//    QWidget *plan = new QWidget;
//    plan->setLayout(container);
//    mainLayout->addWidget(plan);

//    dockBtn = new QPushButton(tr("Undock"), this);
//    connect(dockBtn, &QPushButton::clicked, this, &Window::dockUndock);
//    mainLayout->addWidget(dockBtn);

    setLayout(mainLayout);

//    xSlider->setValue(15 * 16);
//    ySlider->setValue(345 * 16);
//    zSlider->setValue(0 * 16);

    setWindowTitle(tr("Qt OpenGL"));
}

QSlider *Window::createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void Window::mouseDrawOnLabel(QMouseEvent *event, QLabel* label, QImage &img, int label_index)
{
    int x = event->x() * img.width() / label->width();
    int y = event->y() * img.height() / label->height();

    for (int i = -5; i <= 5; ++i) {
        for (int j = -5; j <= 5; ++j) {
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < img.width() && ny >= 0 && ny < img.height()) {
                QRgb value = img.pixel(nx, ny);
                value += 2;
                img.setPixel(nx, ny, qRgb(value, value, value));
            }
        }
    }

    emit onPixmapChanged(label_index, img);
    label->setPixmap(QPixmap::fromImage(img));
}

bool Window::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseMove) {

        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        if (obj == sand_h) {
            mouseDrawOnLabel(mouse, sand_h, sandImage, 0);
        }
        else if(obj == water_h){
            mouseDrawOnLabel(mouse, water_h, waterImage, 1);
        }
        else{
            mouseDrawOnLabel(mouse, lava_h, lavaImage, 2);
        }

        return true;
    }


    return QWidget::eventFilter(obj, event);
}

void Window::dockUndock()
{
    if (parent()) {
        setParent(0);
        setAttribute(Qt::WA_DeleteOnClose);
        move(QApplication::desktop()->width() / 2 - width() / 2,
             QApplication::desktop()->height() / 2 - height() / 2);
        dockBtn->setText(tr("Dock"));
        show();
    } else {
        if (!mainWindow->centralWidget()) {
            if (mainWindow->isVisible()) {
                setAttribute(Qt::WA_DeleteOnClose, false);
                dockBtn->setText(tr("Undock"));
                mainWindow->setCentralWidget(this);
            } else {
                QMessageBox::information(0, tr("Cannot dock"), tr("Main window already closed"));
            }
        } else {
            QMessageBox::information(0, tr("Cannot dock"), tr("Main window already occupied"));
        }
    }
}

void Window::updateHeightmapLabel(int material_index, QImage hm){
    switch (material_index) {
    case 0:
        sandImage = hm.scaled(250, 250, Qt::KeepAspectRatio);
        sand_h->setPixmap(QPixmap::fromImage(sandImage));
        break;
    case 1:
        waterImage = hm.scaled(250, 250, Qt::KeepAspectRatio);
        water_h->setPixmap(QPixmap::fromImage(hm.scaled(250, 250, Qt::KeepAspectRatio)));
        break;
    case 2:
        lavaImage = hm.scaled(250, 250, Qt::KeepAspectRatio);
        lava_h->setPixmap(QPixmap::fromImage(hm.scaled(250, 250, Qt::KeepAspectRatio)));
        break;
    default:
        break;
    }
}
