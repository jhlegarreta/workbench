
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <limits>

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QImageWriter>

#define __IMAGE_CAPTURE_DIALOG__H__DECLARE__
#include "ImageCaptureDialog.h"
#undef __IMAGE_CAPTURE_DIALOG__H__DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CaretPreferences.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "SessionManager.h"
#include "CaretFileDialog.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::ImageCaptureDialog 
 * \brief Dialog for capturing images.
 *
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
ImageCaptureDialog::ImageCaptureDialog(BrainBrowserWindow* parent)
: WuQDialogNonModal("Image Capture",
                    parent)
{
    this->setDeleteWhenClosed(false);

    /*
     * Use Apply button for image capture
     */
    this->setApplyButtonText("Capture");

    /*
     * Image Source
     */
    QLabel* windowLabel = new QLabel("Workbench Window: ");
    this->windowSelectionSpinBox = new QSpinBox();
    this->windowSelectionSpinBox->setRange(1, 1000000);
    this->windowSelectionSpinBox->setSingleStep(1);
    this->windowSelectionSpinBox->setValue(parent->getBrowserWindowIndex() + 1);
    this->windowSelectionSpinBox->setFixedWidth(60);
    
    QGroupBox* imageSourceGroupBox = new QGroupBox("Image Source");
    QGridLayout* imageSourceLayout = new QGridLayout(imageSourceGroupBox);
    imageSourceLayout->addWidget(windowLabel, 0, 0);
    imageSourceLayout->addWidget(this->windowSelectionSpinBox, 0, 1);
    imageSourceLayout->setColumnStretch(0, 0);
    imageSourceLayout->setColumnStretch(1, 0);
    imageSourceLayout->setColumnStretch(1000, 100);
    
    /*
     * Image Size
     */
    this->imageSizeWindowRadioButton = new QRadioButton("Size of Window");
    this->imageSizeCustomRadioButton = new QRadioButton("Custom");
    QButtonGroup* sizeButtonGroup = new QButtonGroup(this);
    sizeButtonGroup->addButton(this->imageSizeWindowRadioButton);
    sizeButtonGroup->addButton(this->imageSizeCustomRadioButton);
    this->imageSizeWindowRadioButton->setChecked(true);
    this->imageSizeCustomXSpinBox = new QSpinBox();
    this->imageSizeCustomXSpinBox->setFixedWidth(80);
    this->imageSizeCustomXSpinBox->setRange(1, 1000000);
    this->imageSizeCustomXSpinBox->setSingleStep(1);
    this->imageSizeCustomXSpinBox->setValue(2560);
    this->imageSizeCustomYSpinBox = new QSpinBox();
    this->imageSizeCustomYSpinBox->setFixedWidth(80);
    this->imageSizeCustomYSpinBox->setRange(1, 1000000);
    this->imageSizeCustomYSpinBox->setSingleStep(1);
    this->imageSizeCustomYSpinBox->setValue(2048);
    QGroupBox* imageSizeGroupBox = new QGroupBox("Image Size");
    QGridLayout* imageSizeLayout = new QGridLayout(imageSizeGroupBox);
    imageSizeLayout->addWidget(this->imageSizeWindowRadioButton, 0, 0, 1, 3);
    imageSizeLayout->addWidget(this->imageSizeCustomRadioButton, 1, 0);
    imageSizeLayout->addWidget(this->imageSizeCustomXSpinBox, 1, 1);
    imageSizeLayout->addWidget(this->imageSizeCustomYSpinBox, 1, 2);
    imageSizeLayout->setColumnStretch(0, 0);
    imageSizeLayout->setColumnStretch(1, 0);
    imageSizeLayout->setColumnStretch(2, 0);
    imageSizeLayout->setColumnStretch(1000, 100);
    
    /*
     * Image Options
     */
    this->imageAutoCropCheckBox = new QCheckBox("Automatically Crop Image");
    QLabel* imageAutoCropMarginLabel = new QLabel("   Margin");
    this->imageAutoCropMarginSpinBox = new QSpinBox();
    this->imageAutoCropMarginSpinBox->setMinimum(0);
    this->imageAutoCropMarginSpinBox->setMaximum(100000);
    this->imageAutoCropMarginSpinBox->setSingleStep(1);
    this->imageAutoCropMarginSpinBox->setValue(10);
    this->imageAutoCropMarginSpinBox->setMaximumWidth(100);
    
    QHBoxLayout* cropMarginLayout = new QHBoxLayout();
    cropMarginLayout->addWidget(imageAutoCropMarginLabel);
    cropMarginLayout->addWidget(this->imageAutoCropMarginSpinBox);
    cropMarginLayout->addStretch();
    
    QGroupBox* imageOptionsGroupBox = new QGroupBox("Image Options");
    QVBoxLayout* imageOptionsLayout = new QVBoxLayout(imageOptionsGroupBox);
    imageOptionsLayout->addWidget(this->imageAutoCropCheckBox);
    imageOptionsLayout->addLayout(cropMarginLayout);
    
    /*
     * Image Destination
     */
    this->copyImageToClipboardCheckBox = new QCheckBox("Copy to Clipboard");
    this->copyImageToClipboardCheckBox->setChecked(true);
    this->saveImageToFileCheckBox = new QCheckBox("Save to File: " );
    this->imageFileNameLineEdit = new QLineEdit();
    this->imageFileNameLineEdit->setText("untitled.png");
    QPushButton* fileNameSelectionPushButton = new QPushButton("Choose File...");
    QObject::connect(fileNameSelectionPushButton, SIGNAL(clicked()),
                     this, SLOT(selectImagePushButtonPressed()));
    
    QGroupBox* imageDestinationGroupBox = new QGroupBox("Image Destination");
    QGridLayout* imageDestinationLayout = new QGridLayout(imageDestinationGroupBox);
    imageDestinationLayout->addWidget(this->copyImageToClipboardCheckBox, 0, 0, 1, 3);
    imageDestinationLayout->addWidget(this->saveImageToFileCheckBox, 1, 0);
    imageDestinationLayout->addWidget(this->imageFileNameLineEdit, 1, 1);
    imageDestinationLayout->addWidget(fileNameSelectionPushButton, 1, 2);
        
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    layout->addWidget(imageSourceGroupBox);
    layout->addWidget(imageSizeGroupBox);
    layout->addWidget(imageOptionsGroupBox);
    layout->addWidget(imageDestinationGroupBox);
    
    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
ImageCaptureDialog::~ImageCaptureDialog()
{
    
}

/**
 * May be called to update the dialog's content.
 */
void 
ImageCaptureDialog::updateDialog()
{
    
}

/**
 * Called when choose file pushbutton is pressed.
 */
void 
ImageCaptureDialog::selectImagePushButtonPressed()
{
    QString defaultFileName = this->imageFileNameLineEdit->text().trimmed();
    if (defaultFileName.isEmpty()) {
        defaultFileName = "untitled.png";
    }
    FileInformation fileInfo(this->imageFileNameLineEdit->text().trimmed());
    if (fileInfo.isRelative()) {
        FileInformation absFileInfo(GuiManager::get()->getBrain()->getCurrentDirectory(),
                                    this->imageFileNameLineEdit->text().trimmed());
        defaultFileName = absFileInfo.getFilePath();
    }
    
    std::vector<AString> imageFileFilters;
    AString defaultFileFilter;
    ImageFile::getImageFileFilters(imageFileFilters, 
                                   defaultFileFilter);
    QString filters;
    for (std::vector<AString>::iterator filterIterator = imageFileFilters.begin();
         filterIterator != imageFileFilters.end();
         filterIterator++) {
        if (filters.isEmpty() == false) {
            filters += ";;";
        }
        
        filters += *filterIterator;
    }
    
    AString name = CaretFileDialog::getSaveFileNameDialog(this,
                                                  "Choose File Name",
                                                  defaultFileName, //GuiManager::get()->getBrain()->getCurrentDirectory(),
                                                  filters,
                                                  &defaultFileFilter);
    if (name.isEmpty() == false) {
        this->imageFileNameLineEdit->setText(name.trimmed());
    }
}

/**
 * Called when the apply button is pressed.
 */
void ImageCaptureDialog::applyButtonPressed()
{
    const int browserWindowIndex = this->windowSelectionSpinBox->value() - 1;
    
    int32_t imageX = 0;
    int32_t imageY = 0;
    if (this->imageSizeCustomRadioButton->isChecked()) {
        imageX = this->imageSizeCustomXSpinBox->value();
        imageY = this->imageSizeCustomYSpinBox->value();
    }
    ImageFile imageFile;
    bool valid = GuiManager::get()->captureImageOfBrowserWindowGraphicsArea(browserWindowIndex,
                                                                            imageX,
                                                                            imageY,
                                                                            imageFile);
    
    if (valid == false) {
        WuQMessageBox::errorOk(this, 
                               "Invalid window selected");
        return;
    }
    
    if (this->imageAutoCropCheckBox->isChecked()) {
        const int marginSize = this->imageAutoCropMarginSpinBox->value();
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        uint8_t backgroundColor[3];
        prefs->getColorBackground(backgroundColor);
        imageFile.cropImageRemoveBackground(marginSize, backgroundColor);
    }
    
    if (this->copyImageToClipboardCheckBox->isChecked()) {
        QApplication::clipboard()->setImage(*imageFile.getAsQImage(), QClipboard::Clipboard);
    }

    if (this->saveImageToFileCheckBox->isChecked()) {
        std::vector<AString> imageFileExtensions;
        AString defaultFileExtension;
        ImageFile::getImageFileExtensions(imageFileExtensions, 
                                          defaultFileExtension);
        
        AString filename = this->imageFileNameLineEdit->text().trimmed();
        
        bool validExtension = false;
        for (std::vector<AString>::iterator extensionIterator = imageFileExtensions.begin();
             extensionIterator != imageFileExtensions.end();
             extensionIterator++) {
            if (filename.endsWith(*extensionIterator)) {
                validExtension = true;
            }
        }
        
        if (validExtension == false) {
            if (defaultFileExtension.isEmpty() == false) {
                filename += ("." + defaultFileExtension);
            }
        }
        
        try {
            imageFile.writeFile(filename);
        }
        catch (const DataFileException& /*e*/) {
            QString msg("Unable to save: " + filename);
            WuQMessageBox::errorOk(this, msg);
        }
    }
}


