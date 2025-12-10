#include "meshdialog.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGroupBox>

MeshDialog::MeshDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Choisir la taille du maillage");

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(400, 300);

    sizeBox = new QComboBox;
    sizeBox->addItems({"64", "128", "256", "512"});
    sizeBox->setCurrentText("128");

    auto *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Taille du maillage :"));
    layout->addWidget(sizeBox);

    QGroupBox *perlinGroup = new QGroupBox("Bruit de Perlin");
    QVBoxLayout *perlinLayout = new QVBoxLayout;

    m_perlin = new QCheckBox("Activer le bruit de Perlin");
    perlinLayout->addWidget(m_perlin);

    QLabel *octavesLabel = new QLabel("Nombre d'octaves :");
    m_octaves = new QSpinBox;
    m_octaves->setRange(1, 10);
    m_octaves->setValue(5);
    m_octaves->setEnabled(false);
    perlinLayout->addWidget(octavesLabel);
    perlinLayout->addWidget(m_octaves);

    QLabel *freqLabel = new QLabel("Fréquence (échelle) :");
    m_frequency = new QDoubleSpinBox;
    m_frequency->setRange(0.001, 1.0);
    m_frequency->setSingleStep(0.001);
    m_frequency->setDecimals(4);
    m_frequency->setValue(0.005);
    m_frequency->setEnabled(false);
    perlinLayout->addWidget(freqLabel);
    perlinLayout->addWidget(m_frequency);

    perlinGroup->setLayout(perlinLayout);
    layout->addWidget(perlinGroup);

    connect(m_perlin, &QCheckBox::toggled, m_octaves, &QSpinBox::setEnabled);
    connect(m_perlin, &QCheckBox::toggled, m_frequency, &QDoubleSpinBox::setEnabled);
    connect(m_perlin, &QCheckBox::toggled, octavesLabel, &QLabel::setEnabled);
    connect(m_perlin, &QCheckBox::toggled, freqLabel, &QLabel::setEnabled);

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

int MeshDialog::octaves() const
{
    return m_octaves->value();
}

double MeshDialog::frequency() const
{
    return m_frequency->value();
}
