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
#include <QFileDialog>
#include <QHeaderView>
#include <QTableView>
#include <QItemSelectionModel>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_label(new QLabel(this))
{
    auto *central = new QWidget(this);
    auto *outer = new QVBoxLayout(central);

    // Top row: action buttons
    auto *topRow = new QHBoxLayout;
    m_addButton = new QPushButton("Add Images", this);
    topRow->addWidget(m_addButton);
    topRow->addWidget(new QPushButton("Remove", this));
    topRow->addWidget(new QPushButton("Clear", this));
    topRow->addStretch();
    outer->addLayout(topRow);

    // Middle row: file table | preview | settings
    auto *middleRow = new QHBoxLayout;

    m_model = new JobListModel(this);
    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->hide();
    m_table->setFixedWidth(260);
    middleRow->addWidget(m_table);

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

    m_opacitySlider = new QSlider(Qt::Horizontal, this);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);
    m_sizeSlider = new QSlider(Qt::Horizontal, this);
    m_sizeSlider->setRange(0, 20);
    m_sizeSlider->setValue(5);
    m_marginSlider = new QSlider(Qt::Horizontal, this);
    m_marginSlider->setRange(0, 10);
    m_marginSlider->setValue(3);

    form->addRow("Opacity:", m_opacitySlider);
    form->addRow("Size:", m_sizeSlider);
    form->addRow("Margin:", m_marginSlider);


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
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddImages);
    connect(m_table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::onSelectionChanged);
    connect(m_opacitySlider, &QSlider::valueChanged, this, &MainWindow::updatePreview);
    connect(m_sizeSlider,    &QSlider::valueChanged, this, &MainWindow::updatePreview);
    connect(m_marginSlider,  &QSlider::valueChanged, this, &MainWindow::updatePreview);
}

void MainWindow::setImage(const QImage &image)
{
    m_original = image;
    updatePreview();
}

void MainWindow::onSelectionChanged(const QModelIndex &current)
{
    if (!current.isValid())
        return;

    const QImage image = loadImage(m_model->pathAt(current.row()));
    if (image.isNull())
        return;

    setImage(image);
}

void MainWindow::onAddImages()
{
    const QStringList paths = QFileDialog::getOpenFileNames(
        this, "Select images", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp)");

    if (paths.isEmpty())
        return;

    m_model->addFiles(paths);
    m_table->setCurrentIndex(m_model->index(m_model->rowCount() - 1, 0));
}

void MainWindow::updatePreview()
{
    if (m_original.isNull())
        return;

    m_settings.text = m_textEdit->text();
    m_settings.hAlign = static_cast<HAlign>(m_hAlignBox->currentIndex());
    m_settings.vAlign = static_cast<VAlign>(m_vAlignBox->currentIndex());
    m_settings.opacity = m_opacitySlider->value() / 100.0;
    m_settings.sizeFraction = m_sizeSlider->value() / 100.0;
    m_settings.marginFraction = m_marginSlider->value() / 100.0;

    const QImage result = applyWatermark(m_original, m_settings);
    const QPixmap pixmap = QPixmap::fromImage(result);
    m_label->setPixmap(pixmap.scaled(800, 600, Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
}