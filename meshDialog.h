#ifndef MESHDIALOG_H
#define MESHDIALOG_H

#pragma once
#include <QDialog>

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

private:
    QComboBox *sizeBox;
    QCheckBox *m_perlin;
};


#endif // DIALOG_H
