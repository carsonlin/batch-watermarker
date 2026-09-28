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
#include <QElapsedTimer>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_label(new QLabel(this))
{
    auto *central = new QWidget(this);
    auto *outer = new QVBoxLayout(central);

    // Top row: action buttons
    auto *topRow = new QHBoxLayout;
    m_addButton = new QPushButton("Add Images", this);
    topRow->addWidget(m_addButton);
    m_removeButton = new QPushButton("Remove", this);
    topRow->addWidget(m_removeButton);
    m_clearButton = new QPushButton("Clear", this);
    topRow->addWidget(m_clearButton);
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
    m_hAlignBox->addItems({"Left", "Center", "Right"});
    m_hAlignBox->setCurrentIndex(2);
    form->addRow("Horizontal:", m_hAlignBox);

    m_vAlignBox = new QComboBox(this);
    m_vAlignBox->addItems({"Top", "Center", "Bottom"});
    m_vAlignBox->setCurrentIndex(2);
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

    m_browseButton = new QPushButton("Choose Output Folder", this);
    bottomRow->addWidget(m_browseButton);

    m_outputLabel = new QLabel("No folder selected", this);
    bottomRow->addWidget(m_outputLabel, 1);

    m_processButton = new QPushButton("Process All", this);
    bottomRow->addWidget(m_processButton);

    m_progressBar = new QProgressBar(this);
    bottomRow->addWidget(m_progressBar);

    outer->addLayout(bottomRow);

    setCentralWidget(central);

    // Any change in a control refreshes the preview
    connect(m_textEdit, &QLineEdit::textChanged, this, &MainWindow::updatePreview);
    connect(m_hAlignBox, &QComboBox::currentIndexChanged, this, &MainWindow::updatePreview);
    connect(m_vAlignBox, &QComboBox::currentIndexChanged, this, &MainWindow::updatePreview);
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddImages);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::onRemove);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::onClear);
    connect(m_table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MainWindow::onSelectionChanged);
    connect(m_opacitySlider, &QSlider::valueChanged, this, &MainWindow::updatePreview);
    connect(m_sizeSlider,    &QSlider::valueChanged, this, &MainWindow::updatePreview);
    connect(m_marginSlider,  &QSlider::valueChanged, this, &MainWindow::updatePreview);
    connect(m_browseButton, &QPushButton::clicked, this, &MainWindow::onBrowseFolder);
    connect(m_processButton, &QPushButton::clicked, this, &MainWindow::onProcessAll);
    auto resetProgress = [this]() { m_progressBar->setValue(0); };
    connect(m_model, &QAbstractItemModel::rowsInserted, this, resetProgress);
    connect(m_model, &QAbstractItemModel::rowsRemoved,  this, resetProgress);
    connect(m_model, &QAbstractItemModel::modelReset,   this, resetProgress);
}

void MainWindow::onProcessAll()
{
    if (m_outputFolder.isEmpty()) {
        QMessageBox::warning(this, "No output folder", "Choose an output folder first.");
        return;
    }

    const int total = m_model->rowCount();
    if (total == 0)
        return;

    readSettings();
    m_progressBar->setRange(0, total);
    m_progressBar->setValue(0);

    QElapsedTimer timer;
    timer.start();
    int succeeded = 0;

    for (int row = 0; row < total; ++row) {
        m_model->setStatus(row, JobStatus::Processing);

        const QString sourcePath = m_model->pathAt(row);
        const QImage image = loadImage(sourcePath);
        if (image.isNull()) {
            m_model->setStatus(row, JobStatus::Failed);
            m_progressBar->setValue(row + 1);
            continue;                       // skip this file, keep going
        }

        const QImage result = applyWatermark(image, m_settings);
        const QFileInfo info(sourcePath);
        const QString newName = QString("%1_watermarked_%2.%3")
                                    .arg(info.completeBaseName())
                                    .arg(row + 1)
                                    .arg(info.suffix());
        const QString outputPath = QDir(m_outputFolder).filePath(newName);

        if (result.save(outputPath)) {
            m_model->setStatus(row, JobStatus::Done);
            ++succeeded;
        } else {
            m_model->setStatus(row, JobStatus::Failed);
        }
        m_progressBar->setValue(row + 1);
    }

    qDebug() << "Batch of" << total << "took" << timer.elapsed() << "ms";
    QMessageBox::information(this, "Finished",
                             QString("%1 of %2 images processed.").arg(succeeded).arg(total));
}

void MainWindow::setImage(const QImage &image)
{
    m_original = image;
    m_previewSource = image.scaledToWidth(800, Qt::SmoothTransformation);
    updatePreview();
}

void MainWindow::onSelectionChanged(const QModelIndex &current)
{
    if (!current.isValid()){
        clearPreview();
        return;
    }

    showRow(current.row());
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

void MainWindow::onBrowseFolder()
{
    const QString folder = QFileDialog::getExistingDirectory(
        this, "Select output folder", QString());

    if (folder.isEmpty())
        return;

    m_outputFolder = folder;
    m_outputLabel->setText(folder);

}

void MainWindow::updatePreview()
{
    if (m_original.isNull())
        return;

    readSettings();

    const QImage result = applyWatermark(m_previewSource, m_settings);
    const QPixmap pixmap = QPixmap::fromImage(result);
    m_label->setPixmap(pixmap.scaled(800, 600, Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
}

void MainWindow::readSettings()
{
    m_settings.text = m_textEdit->text();
    m_settings.hAlign = static_cast<HAlign>(m_hAlignBox->currentIndex());
    m_settings.vAlign = static_cast<VAlign>(m_vAlignBox->currentIndex());
    m_settings.opacity = m_opacitySlider->value() / 100.0;
    m_settings.sizeFraction = m_sizeSlider->value() / 100.0;
    m_settings.marginFraction = m_marginSlider->value() / 100.0;
}

void MainWindow::onRemove()
{
    const QModelIndex index = m_table->currentIndex();
    if (!index.isValid())
        return;
    const int row = index.row();
    m_model->removeAt(row);

    if (m_model->rowCount() == 0){
        clearPreview();
        return;
    }

    const int newRow = qMin(row, m_model->rowCount() - 1);
    m_table->setCurrentIndex(m_model->index(newRow, 0));
    showRow(newRow);
}

void MainWindow::onClear()
{
    m_model->clear();
    clearPreview();
}

void MainWindow::clearPreview()
{
    m_original = QImage();
    m_previewSource = QImage();
    m_label->clear();
}

void MainWindow::showRow(int row)
{
    const QImage image = loadImage(m_model->pathAt(row));
    if (image.isNull())
        return;

    setImage(image);
}