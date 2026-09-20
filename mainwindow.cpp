#include "mainwindow.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_label(new QLabel(this))
{
    auto *central = new QWidget(this);
    auto *outer = new QVBoxLayout(central);

    // Top row: action buttons
    auto *topRow = new QHBoxLayout;
    topRow->addWidget(new QPushButton("Add Images", this));
    topRow->addWidget(new QPushButton("Remove", this));
    topRow->addWidget(new QPushButton("Clear", this));
    topRow->addStretch();
    outer->addLayout(topRow);

    // Middle row: file table | preview | settings
    auto *middleRow = new QHBoxLayout;

    auto *tablePlaceholder = new QLabel("table goes here", this);
    tablePlaceholder->setAlignment(Qt::AlignCenter);
    tablePlaceholder->setFixedWidth(220);
    middleRow->addWidget(tablePlaceholder);

    m_label->setAlignment(Qt::AlignCenter);
    m_label->setMinimumSize(400, 300);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    middleRow->addWidget(m_label, 1);

    // Settings panel
    auto *panel = new QWidget(this);
    auto *form = new QFormLayout(panel);

    m_textEdit = new QLineEdit(this);
    m_textEdit->setText("My Watermark");
    form->addRow("Text:", m_textEdit);

    m_hAlignBox = new QComboBox(this);
    m_hAlignBox->addItems({"Left", "Center", "Right"});   // same order as enum HAlign
    m_hAlignBox->setCurrentIndex(2);                      // Right
    form->addRow("Horizontal:", m_hAlignBox);

    m_vAlignBox = new QComboBox(this);
    m_vAlignBox->addItems({"Top", "Center", "Bottom"});   // same order as enum VAlign
    m_vAlignBox->setCurrentIndex(2);                      // Bottom
    form->addRow("Vertical:", m_vAlignBox);

    panel->setFixedWidth(220);
    middleRow->addWidget(panel);

    outer->addLayout(middleRow, 1);

    // Bottom row: output folder, process button, progress
    auto *bottomRow = new QHBoxLayout;
    auto *outputEdit = new QLineEdit(this);
    outputEdit->setPlaceholderText("Output folder");
    bottomRow->addWidget(outputEdit, 1);
    bottomRow->addWidget(new QPushButton("Browse", this));
    bottomRow->addWidget(new QPushButton("Process All", this));
    bottomRow->addWidget(new QProgressBar(this));
    outer->addLayout(bottomRow);

    setCentralWidget(central);

    // Any change in a control refreshes the preview
    connect(m_textEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_hAlignBox, &QComboBox::currentIndexChanged, this, &MainWindow::updatePreview);
    connect(m_vAlignBox, &QComboBox::currentIndexChanged, this, &MainWindow::updatePreview);
}

void MainWindow::setImage(const QImage &image)
{
    m_original = image;
    updatePreview();
}

void MainWindow::updatePreview()
{
    if (m_original.isNull())
        return;

    m_settings.text = m_textEdit->text();
    m_settings.hAlign = static_cast<HAlign>(m_hAlignBox->currentIndex());
    m_settings.vAlign = static_cast<VAlign>(m_vAlignBox->currentIndex());

    const QImage result = applyWatermark(m_original, m_settings);
    const QPixmap pixmap = QPixmap::fromImage(result);
    m_label->setPixmap(pixmap.scaled(800, 600, Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
}