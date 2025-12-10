#ifndef MESHDIALOG_H
#define MESHDIALOG_H

#pragma once
#include <QDialog>
#include <QDoubleSpinBox>

class QComboBox;
class QDialogButtonBox;
class QSpinBox;
class QCheckBox;

class MeshDialog : public QDialog
{
    Q_OBJECT
public:
    MeshDialog(QWidget *parent = nullptr);

    int meshSize() const;
    bool usePerlinNoise() const;
    int octaves() const;
    double frequency() const;

private:
    QComboBox *sizeBox;
    QCheckBox *m_perlin;
    QSpinBox *m_octaves;
    QDoubleSpinBox *m_frequency;
};


#endif // DIALOG_H
