
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

#define __OVERLAY_SELECTION_CONTROL_LAYER_DECLARE__
#include "OverlaySelectionControlLayer.h"
#undef __OVERLAY_SELECTION_CONTROL_LAYER_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QToolButton>

#include "BrowserTabContent.h"
#include "CaretLogger.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventMapScalarDataColorMappingEditor.h"
#include "CaretMappableDataFile.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


/**
 * Contructs a single layer.
 * @param browserWindowIndex
 *    Index of browser window containing this
 *    Layer.
 * @param overlaySelectionControl
 *    Parent that contains this layer.
 * @param dataType
 *    Type of data for overlay selection.
 * @param layerIndex
 *    Index of this layer.
 */
OverlaySelectionControlLayer::OverlaySelectionControlLayer(const int32_t browserWindowIndex,
                                      OverlaySelectionControl* overlaySelectionControl,
                                      const int32_t layerIndex)
{
    const bool verticalFlag = (overlaySelectionControl->orientation == Qt::Vertical);
    
    this->overlaySelectionControl = overlaySelectionControl;
    this->browserWindowIndex = browserWindowIndex;
    this->layerIndex = layerIndex;
    
    this->enabledCheckBox = new QCheckBox("");
    this->enabledCheckBox->setVisible(true);
    QObject::connect(this->enabledCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(enableCheckBoxToggled(bool)));
    
    this->paletteDisplayCheckBox = new QCheckBox("");
    this->paletteDisplayCheckBox->setVisible(true);
    QObject::connect(this->paletteDisplayCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(paletteDisplayCheckBoxToggled(bool)));
    if (verticalFlag) {
        this->paletteDisplayCheckBox->setText("Show Palette");
    }
    
    //const int comboBoxWidth = 200;
    
    this->fileSelectionComboBox = new QComboBox();
    //this->fileSelectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    QObject::connect(this->fileSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(fileSelected(int)));
    //this->fileSelectionComboBox->setFixedWidth(comboBoxWidth);
    this->columnSelectionComboBox = new QComboBox();
    //this->columnSelectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QObject::connect(this->columnSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(columnSelected(int)));
    //this->columnSelectionComboBox->setFixedWidth(comboBoxWidth);
    
    this->settingsAction = WuQtUtilities::createAction("S",
                                                        "Show control for adjusting settings that controls data coloring",
                                                        this,
                                                        this,
                                                        SLOT(settingsToolButtonPressed()));
    this->settingsToolButton  = new QToolButton();
    this->settingsToolButton->setDefaultAction(this->settingsAction);
    if (verticalFlag) {
        //this->settingsAction->setText("Settings");
    }
    
    this->metadataAction = WuQtUtilities::createAction("M",
                                                        "Show metadata for the selected data",
                                                        this,
                                                        this,
                                                        SLOT(metadataToolButtonPressed()));
    this->metadataToolButton  = new QToolButton();
    this->metadataToolButton->setDefaultAction(this->metadataAction);
    if (verticalFlag) {
        //this->metadataAction->setText("Metadata");
    }
    
    this->opacityDoubleSpinBox = new QDoubleSpinBox();
    this->opacityDoubleSpinBox->setMinimum(0.0);
    this->opacityDoubleSpinBox->setMaximum(1.0);
    this->opacityDoubleSpinBox->setValue(1.0);
    this->opacityDoubleSpinBox->setSingleStep(0.1);
    this->opacityDoubleSpinBox->setToolTip("Adjust opacity, 0=>transparent, 1=opaque");
    this->opacityDoubleSpinBox->setFixedWidth(60);
    QObject::connect(this->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(opacityValueChanged(double)));
    
    this->removeAction = WuQtUtilities::createAction("X",
                                                        "Remove this layer",
                                                        this,
                                                        this,
                                                        SLOT(removeLayerToolButtonPressed()));
    this->deleteToolButton = new QToolButton();
    this->deleteToolButton->setDefaultAction(this->removeAction);
    
    this->moveUpAction = WuQtUtilities::createAction("",
                                                        "Move this layer up",
                                                        this,
                                                        this,
                                                        SLOT(moveLayerUpToolButtonPressed()));
    this->upArrowToolButton = new QToolButton();
    this->upArrowToolButton->setDefaultAction(this->moveUpAction);
    this->upArrowToolButton->setArrowType(Qt::UpArrow);
    
    this->moveDownAction = WuQtUtilities::createAction("",
                                                        "Move this layer down",
                                                        this,
                                                        this,
                                                        SLOT(moveLayerDownToolButtonPressed()));
    this->downArrowToolButton = new QToolButton();
    this->downArrowToolButton->setDefaultAction(this->moveDownAction);
    this->downArrowToolButton->setArrowType(Qt::DownArrow);
    
    this->widgetGroup = new WuQWidgetObjectGroup(overlaySelectionControl);
    this->widgetGroup->add(this->enabledCheckBox);
    this->widgetGroup->add(this->paletteDisplayCheckBox);
    this->widgetGroup->add(this->fileSelectionComboBox);
    this->widgetGroup->add(this->columnSelectionComboBox);
    this->widgetGroup->add(this->settingsToolButton);
    this->widgetGroup->add(this->metadataToolButton);
    this->widgetGroup->add(this->opacityDoubleSpinBox);            
    
    this->widgetGroup->add(this->deleteToolButton);
    this->widgetGroup->add(this->upArrowToolButton);
    this->widgetGroup->add(this->downArrowToolButton);
}

/**
 * Destroys this layer.
 */
OverlaySelectionControlLayer::~OverlaySelectionControlLayer()
{
    
}

/**
 * Called when palette display checkbox is toggled.
 * @param toggled
 *   New display status.
 */
void 
OverlaySelectionControlLayer::paletteDisplayCheckBoxToggled(bool toggled)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    Overlay* overlay = overlaySet->getOverlay(this->layerIndex);
    overlay->setPaletteDisplayEnabled(toggled);

    this->updateControl(browserTabContent);
    
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

/**
 * Called when enable checkbox is toggled.
 * @param toggled
 *   New enabled status.
 */
void 
OverlaySelectionControlLayer::enableCheckBoxToggled(bool toggled)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    browserTabContent->invalidateSurfaceColoring();
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    Overlay* overlay = overlaySet->getOverlay(this->layerIndex);
    overlay->setEnabled(toggled);
    
    this->updateControl(browserTabContent);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Move this layer up one level.
 */ 
void 
OverlaySelectionControlLayer::moveLayerUpToolButtonPressed()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    browserTabContent->invalidateSurfaceColoring();
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    overlaySet->moveDisplayedOverlayUp(this->layerIndex);
    
    this->updateControl(browserTabContent);
    
    this->overlaySelectionControl->updateControl();
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Move this layer down one level.
 */ 
void 
OverlaySelectionControlLayer::moveLayerDownToolButtonPressed()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    browserTabContent->invalidateSurfaceColoring();
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    overlaySet->moveDisplayedOverlayDown(this->layerIndex);
            
    this->updateControl(browserTabContent);
    
    this->overlaySelectionControl->updateControl();
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Remove this layer up one level.
 */ 
void 
OverlaySelectionControlLayer::removeLayerToolButtonPressed()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    browserTabContent->invalidateSurfaceColoring();
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    overlaySet->removeDisplayedOverlay(this->layerIndex);
    this->updateControl(browserTabContent);
    
    this->overlaySelectionControl->updateControl();
    
    emit controlRemoved();
    
    this->updateUserInterfaceAndGraphicsWindow();
}

void 
OverlaySelectionControlLayer::settingsToolButtonPressed()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    Overlay* overlay = overlaySet->getOverlay(this->layerIndex);
    CaretMappableDataFile* mapFile;
    int32_t mapIndex = -1;
    overlay->getSelectionData(browserTabContent, 
                              mapFile, 
                              mapIndex);
    if (mapFile != NULL) {
        if (mapFile->isMappedWithPalette()) {
            if (mapFile != NULL) {
                EventMapScalarDataColorMappingEditor pcme(this->browserWindowIndex,
                                                    mapFile,
                                                    mapIndex);
                EventManager::get()->sendEvent(pcme.getPointer());
            }
        }
        else if (mapFile->isMappedWithLabelTable()) {
            QMessageBox::information(this->overlaySelectionControl, "", "Edit Labels!");
        }
    }
}

void 
OverlaySelectionControlLayer::metadataToolButtonPressed()
{
    QMessageBox::information(this->overlaySelectionControl, "", "Metadata!");
}

/**
 * Called when opacity is changed.
 * @param value
 *    New value for opacity.
 */
void 
OverlaySelectionControlLayer::opacityValueChanged(double value)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    browserTabContent->invalidateSurfaceColoring();
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    Overlay* overlay = overlaySet->getOverlay(this->layerIndex);
    overlay->setOpacity(value);
    
    this->updateControl(browserTabContent);
    
    EventGraphicsUpdateOneWindow updateGraphics(this->browserWindowIndex);
    EventManager::get()->sendEvent(updateGraphics.getPointer());
}

/**
 * Called when a file selection is made.
 * @param fileIndex
 *    Index of selected file.
 */
void 
OverlaySelectionControlLayer::fileSelected(int fileIndex)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);

    browserTabContent->invalidateSurfaceColoring();
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    Overlay* overlay = overlaySet->getOverlay(this->layerIndex);
    
    void* pointer = this->fileSelectionComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    overlay->setSelectionData(file, 0);

    this->updateControl(browserTabContent);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when a column selection is made.
 * @param columnIndex
 *    Index of selected column.
 */
void 
OverlaySelectionControlLayer::columnSelected(int columnIndex)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(this->browserWindowIndex, false);
    
    OverlaySet* overlaySet = browserTabContent->getOverlaySet();
    Overlay* overlay = overlaySet->getOverlay(this->layerIndex);
    
    const int32_t fileIndex = this->fileSelectionComboBox->currentIndex();
    void* pointer = this->fileSelectionComboBox->itemData(fileIndex).value<void*>();
    CaretMappableDataFile* file = (CaretMappableDataFile*)pointer;
    overlay->setSelectionData(file, columnIndex);

    browserTabContent->invalidateSurfaceColoring();
    
    
    this->updateControl(browserTabContent);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Update this control.
 * @param browserTabContent
 *     Content in the current browser tab.
 */
void 
OverlaySelectionControlLayer::updateControl(BrowserTabContent* browserTabContent)
{
    int numberOfDisplayedLayers = 3;
    this->widgetGroup->blockAllSignals(true);
    this->updateOverlayControl(browserTabContent);
    numberOfDisplayedLayers = browserTabContent->getOverlaySet()->getNumberOfDisplayedOverlays();
 
    if (numberOfDisplayedLayers < 3) {
        numberOfDisplayedLayers = 3;
    }
    
    int32_t lastDisplayedIndex = numberOfDisplayedLayers - 1;
    bool isMoveUpEnabled = false;
    bool isMoveDownEnabled = false;
    bool isRemoveEnabled = false;
    
    if (this->layerIndex > 0) {
        isMoveUpEnabled = true;
    }
    if (this->layerIndex < lastDisplayedIndex) {
        isMoveDownEnabled = true;
    }
    if (numberOfDisplayedLayers > 3) {
        isRemoveEnabled = true;
    }
    
    this->moveDownAction->setEnabled(isMoveDownEnabled);
    this->moveUpAction->setEnabled(isMoveUpEnabled);
    this->removeAction->setEnabled(isRemoveEnabled);
    
    this->widgetGroup->blockAllSignals(false);
}

/**
 * Update this overlay control.
 * @param browserTabContent
 *     Content in the current browser tab.
 */
void 
OverlaySelectionControlLayer::updateOverlayControl(BrowserTabContent* browserTabContent)
{
    Overlay* so = browserTabContent->getOverlaySet()->getOverlay(this->layerIndex);
    
    this->fileSelectionComboBox->clear();
    this->columnSelectionComboBox->clear();
    
    /*
     * Get the selection information for the overlay.
     */
    std::vector<CaretMappableDataFile*> dataFiles;
    CaretMappableDataFile* selectedFile = NULL;
    AString selectedMapUniqueID = "";
    int32_t selectedMapIndex = -1;
    so->getSelectionData(browserTabContent,
                         dataFiles, 
                         selectedFile, 
                         selectedMapUniqueID, 
                         selectedMapIndex);
    
    /*
     * Load the file selection combo box.
     */
    int32_t selectedFileIndex = -1;
    const int32_t numFiles = static_cast<int32_t>(dataFiles.size());
    for (int32_t i = 0; i < numFiles; i++) {
        CaretMappableDataFile* dataFile = dataFiles[i];
        
        AString dataTypeName = DataFileTypeEnum::toName(dataFile->getDataFileType());
        switch (dataFile->getDataFileType()) {
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                dataTypeName = "CONNECTIVITY";
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                dataTypeName = "TIME_SERIES";
                break;
            default:
                break;
        }
        AString name = dataTypeName
        + " "
        + dataFile->getFileNameNoPath();
        this->fileSelectionComboBox->addItem(name,
                                             qVariantFromValue((void*)dataFile));
        if (dataFile == selectedFile) {
            selectedFileIndex = i;
        }
    }
    if (selectedFileIndex >= 0) {
        this->fileSelectionComboBox->setCurrentIndex(selectedFileIndex);
    }
     
    /*
     * Load the column selection combo box.
     */
    if (selectedFile != NULL) {
        const int32_t numMaps = selectedFile->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            this->columnSelectionComboBox->addItem(selectedFile->getMapName(i));
        }
        this->columnSelectionComboBox->setCurrentIndex(selectedMapIndex);
    }
    
    this->paletteDisplayCheckBox->setChecked(so->isPaletteDisplayEnabled());
    this->enabledCheckBox->setChecked(so->isEnabled());
    this->opacityDoubleSpinBox->setValue(so->getOpacity());
}

/**
 * Add a widget to a widget group so that they can be
 * hidden/displayed as a group.
 * @param w
 *   Widget that is added.
 */
void 
OverlaySelectionControlLayer::addWidget(QWidget* w)
{
    this->widgetGroup->add(w);
}

/**
 * @return Is this layer visible?
 */
bool
OverlaySelectionControlLayer::isVisible() const 
{
    return this->enabledCheckBox->isVisible();    
}

/**
 * Set the visibility of this layer.
 * @param visible
 *    New visibility status.
 */
void
OverlaySelectionControlLayer::setVisible(const bool visible)
{
    this->widgetGroup->setVisible(visible);
}

/**
 * Update the user-interface and graphics windows for the selected tab.
 */
void 
OverlaySelectionControlLayer::updateUserInterfaceAndGraphicsWindow()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(this->browserWindowIndex).getPointer());
}

