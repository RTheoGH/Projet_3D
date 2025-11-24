#include "meshdialog.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>

MeshDialog::MeshDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Choisir la taille du maillage");

    sizeBox = new QComboBox;
    sizeBox->addItems({"256", "512", "1024", "2048"});

    auto *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Taille du maillage :"));
    layout->addWidget(sizeBox);

    m_perlin = new QCheckBox("Ajouter du bruit de Perlin");
    layout->addWidget(m_perlin);

    QDialogButtonBox *buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);
    setLayout(layout);
}

int MeshDialog::meshSize() const
{
    return sizeBox->currentText().toInt();
}

bool MeshDialog::usePerlinNoise() const
{
    return m_perlin->isChecked();
}
