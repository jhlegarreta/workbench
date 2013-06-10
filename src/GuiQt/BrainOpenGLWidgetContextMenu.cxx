
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QActionGroup>

#define __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__
#include "BrainOpenGLWidgetContextMenu.h"
#undef __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__

#include "AlgorithmException.h"
#include "AlgorithmNodesInsideBorder.h"
#include "Border.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "ChartableInterface.h"
#include "ChartingDataManager.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUpdateTimeCourseDialog.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GuiManager.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "LabelFile.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Model.h"
#include "ProgressReportingDialog.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"
#include "TimeCourseDialog.h"
#include "UserInputModeFociWidget.h"
#include "VolumeFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLWidgetContextMenu 
 * \brief Context (pop-up) menu for BrainOpenGLWidget
 *
 * Displays a menu in the BrainOpenGLWidget.  Content of menu
 * is dependent upon data under the cursor.
 */
/**
 * Constructor.
 * @param identificationManager
 *    The identification manager, provides data under the cursor.
 * @param parent
 *    Parent on which the menu is displayed.
 */
BrainOpenGLWidgetContextMenu::BrainOpenGLWidgetContextMenu(SelectionManager* identificationManager,
                                                           BrowserTabContent* browserTabContent,
                                                           QWidget* parent)
: QMenu(parent)
{
    this->parentWidget = parent;
    this->identificationManager = identificationManager;
    this->browserTabContent = browserTabContent;
    
    /*
     * Accumlate identification actions
     */
    std::vector<QAction*> identificationActions;
    
    /*
     * Identify Border
     */
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    if (borderID->isValid()) {
        const QString text = ("Identify Border ("
                              + borderID->getBorder()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text, 
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifySurfaceBorderSelected())));
    }

    /*
     * Identify Surface Focus
     */
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    if (focusID->isValid()) {
        const QString text = ("Identify Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifySurfaceFocusSelected())));
    }
    
    /*
     * Identify Node
     */
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    if (surfaceID->isValid()) {
        const int32_t nodeIndex = surfaceID->getNodeNumber();
        const Surface* surface = surfaceID->getSurface();
        const QString text = ("Identify Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ") Under Mouse");

        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifySurfaceNodeSelected())));
    }
    
    /*
     * Identify Voxel
     */
    SelectionItemVoxel* idVoxel = this->identificationManager->getVoxelIdentification();
    if (idVoxel->isValid()) {
        int64_t ijk[3];
        idVoxel->getVoxelIJK(ijk);
        const AString text = ("Identify Voxel ("
                              + AString::fromNumbers(ijk, 3, ",")
                              + ")");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(identifyVoxelSelected())));
    }
    
    /*
     * Identify Volume Focus
     */
    SelectionItemFocusVolume* focusVolID = this->identificationManager->getVolumeFocusIdentification();
    if (focusVolID->isValid()) {
        const QString text = ("Identify Volume Focus ("
                              + focusVolID->getFocus()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifyVolumeFocusSelected())));
    }
    
    if (identificationActions.empty() == false) {
        this->addSeparator();
        
        for (std::vector<QAction*>::iterator idIter = identificationActions.begin();
             idIter != identificationActions.end();
             idIter++) {
            this->addAction(*idIter);
        }
    }
    
    
    std::vector<QAction*> borderConnectivityActions;
    
    /*static bool run = false;
    if(!run)
    {
        run = true;
        const AString actionName("Show Time series For Parcel ");
        QAction* action = connectivityActionGroup->addAction(actionName);
        dataSeriesActions.push_back(action);

    }*/

    if (borderID->isValid()) {
        Brain* brain = borderID->getBrain();
        std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
        bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
        
        /*
         * Connectivity actions for borders
         */
        if (hasCiftiConnectivity) {
            const QString text = ("Show CIFTI Connectivity for Nodes Inside Border "
                                  + borderID->getBorder()->getName());
            QAction* action = WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(borderCiftiConnectivitySelected()));
            borderConnectivityActions.push_back(action);
        }
        
        std::vector<ChartableInterface*> chartableFiles;
        brain->getAllChartableDataFiles(chartableFiles);
        
        if (chartableFiles.empty() == false) {
            const QString text = ("Show Charts for Nodes Inside Border "
                                  + borderID->getBorder()->getName());
            QAction* action = WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(borderDataSeriesSelected()));
            borderConnectivityActions.push_back(action);
        }
        
//        bool hasTimeSeries = brain->getNumberOfConnectivityTimeSeriesFiles() > 0 ? true : false;
//        ConnectivityLoaderManager* clm = NULL;
//        if (hasTimeSeries) {
//            clm = brain->getConnectivityLoaderManager();
//        }        
//        if (clm != NULL) {
//            if(hasTimeSeries)
//            {
//                const QString text = ("Show Data Series Graph for Nodes Inside Border "
//                                      + borderID->getBorder()->getName());
//                QAction* action = WuQtUtilities::createAction(text,
//                                                              "",
//                                                              this,
//                                                              this,
//                                                              SLOT(borderDataSeriesSelected()));
//                borderConnectivityActions.push_back(action);
//            }   
//        }
    }
    
    std::vector<QAction*> ciftiConnectivityActions;
    QActionGroup* ciftiConnectivityActionGroup = new QActionGroup(this);
    QObject::connect(ciftiConnectivityActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelCiftiConnectivityActionSelected(QAction*)));
    
    std::vector<QAction*> dataSeriesActions;
    QActionGroup* dataSeriesActionGroup = new QActionGroup(this);
    QObject::connect(dataSeriesActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelDataSeriesActionSelected(QAction*)));
    if (surfaceID->isValid()) {
        /*
         * Connectivity actions for labels
         */
        Brain* brain = surfaceID->getBrain();
        Surface* surface = surfaceID->getSurface();
        const int32_t nodeNumber = surfaceID->getNodeNumber();
        
        CiftiConnectivityMatrixDataFileManager* connMatrixMan = brain->getCiftiConnectivityMatrixDataFileManager();
        std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
        bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
        
        std::vector<ChartableInterface*> chartableFiles;
        brain->getAllChartableDataFiles(chartableFiles);
        const bool haveChartableFiles = (chartableFiles.empty() == false);
        ChartingDataManager* chartingDataManager = brain->getChartingDataManager();
    
        Model* model = this->browserTabContent->getModelControllerForDisplay();
        if (model != NULL) {
            std::vector<CaretMappableDataFile*> allMappableLabelFiles;
            
            std::vector<CiftiBrainordinateLabelFile*> ciftiLabelFiles;
            brain->getConnectivityDenseLabelFiles(ciftiLabelFiles);
            allMappableLabelFiles.insert(allMappableLabelFiles.end(),
                                 ciftiLabelFiles.begin(),
                                 ciftiLabelFiles.end());
            
                std::vector<LabelFile*> brainStructureLabelFiles;
                surface->getBrainStructure()->getLabelFiles(brainStructureLabelFiles);
            allMappableLabelFiles.insert(allMappableLabelFiles.end(),
                                 brainStructureLabelFiles.begin(),
                                 brainStructureLabelFiles.end());
            
            const int32_t numberOfLabelFiles = static_cast<int32_t>(allMappableLabelFiles.size());
            for (int32_t ilf = 0; ilf < numberOfLabelFiles; ilf++) {
                CaretMappableDataFile* mappableLabelFile = allMappableLabelFiles[ilf];
                const int32_t numMaps = mappableLabelFile->getNumberOfMaps();
                for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                    
                    int32_t labelKey = -1;
                    AString labelName;
                    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
                    LabelFile* labelFile = dynamic_cast<LabelFile*>(mappableLabelFile);
                    
                    if (ciftiLabelFile != NULL) {
                        float nodeValue;
                        bool nodeValueValid = false;
                        AString stringValue;
                        if (ciftiLabelFile->getMapSurfaceNodeValue(mapIndex,
                                                                   surface->getStructure(),
                                                                   nodeNumber,
                                                                   surface->getNumberOfNodes(),
                                                                   nodeValue,
                                                                   nodeValueValid,
                                                                   stringValue)) {
                            if (nodeValueValid) {
                                labelKey = static_cast<int32_t>(nodeValue);
                                const GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
                                labelName =  labelTable->getLabelName(labelKey);
                            }
                        }
                    }
                    else if (labelFile != NULL) {
                        labelKey = labelFile->getLabelKey(nodeNumber,
                                                          mapIndex);
                        labelName = labelFile->getLabelName(nodeNumber,
                                                            mapIndex);
                    }
                    else {
                        CaretAssertMessage(0,
                                           "Should never get here, new or invalid label file type");
                    }
                    
                    const AString mapName = mappableLabelFile->getMapName(mapIndex);
                    if (labelName.isEmpty() == false) {
                        ParcelConnectivity* pc = new ParcelConnectivity(mappableLabelFile,
                                                                        mapIndex,
                                                                        labelKey,
                                                                        labelName,
                                                                        surface,
                                                                        nodeNumber,
                                                                        chartingDataManager,
                                                                        connMatrixMan);
                        this->parcelConnectivities.push_back(pc);
                        
                        if (hasCiftiConnectivity) {
                            const AString actionName("Show Cifti Connectivity For Parcel "
                                                     + labelName
                                                     + " in map "
                                                     + mapName);
                            QAction* action = ciftiConnectivityActionGroup->addAction(actionName);
                            action->setData(qVariantFromValue((void*)pc));
                            ciftiConnectivityActions.push_back(action);
                        }
                        
                        if (haveChartableFiles) {
                            const AString tsActionName("Show Data Series Graph For Parcel "
                                                       + labelName
                                                       + " in map "
                                                       + mapName);
                            QAction* tsAction = dataSeriesActionGroup->addAction(tsActionName);
                            tsAction->setData(qVariantFromValue((void*)pc));
                            dataSeriesActions.push_back(tsAction);
                        }
                    }
                    
                }
            }
        }
    }
    
    if (borderConnectivityActions.empty() == false) {
        this->addSeparator();
        for (std::vector<QAction*>::iterator borderIter = borderConnectivityActions.begin();
             borderIter != borderConnectivityActions.end();
             borderIter++) {
            this->addAction(*borderIter);
        }
    }
    
    if (ciftiConnectivityActions.empty() == false) {
        this->addSeparator();
        for (std::vector<QAction*>::iterator ciftiConnIter = ciftiConnectivityActions.begin();
             ciftiConnIter != ciftiConnectivityActions.end();
             ciftiConnIter++) {
            this->addAction(*ciftiConnIter);
        }
    }
    
    if(dataSeriesActions.empty() == false) {
        this->addSeparator();            
        for (std::vector<QAction*>::iterator tsIter = dataSeriesActions.begin();
             tsIter != dataSeriesActions.end();
             tsIter++) {
            this->addAction(*tsIter);
        }
    }
    
    std::vector<QAction*> createActions;
    
    const SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = identificationManager->getSurfaceNodeIdentificationSymbol();

    /*
     * Create focus at surface node or at ID symbol
     */
    if (surfaceID->isValid()
        && (focusID->isValid() == false)) {
        const int32_t nodeIndex = surfaceID->getNodeNumber();
        const Surface* surface = surfaceID->getSurface();
        const QString text = ("Create Focus at Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ")...");
        
        createActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createSurfaceFocusSelected())));
    }
    else if (idSymbol->isValid()
             && (focusID->isValid() == false)) {
        const int32_t nodeIndex = idSymbol->getNodeNumber();
        const Surface* surface = idSymbol->getSurface();
        const QString text = ("Create Focus at Selected Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ")...");
        
        createActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createSurfaceIDSymbolFocusSelected())));
    }

    /*
     * Create focus at voxel as long as there is no volume focus ID
     */
    if (idVoxel->isValid()
        && (focusVolID->isValid() == false)) {
        int64_t ijk[3];
        idVoxel->getVoxelIJK(ijk);
        float xyz[3];
        const VolumeMappableInterface* vf = idVoxel->getVolumeFile();
        vf->indexToSpace(ijk, xyz);
        
        const AString text = ("Create Focus at Voxel IJK ("
                              + AString::fromNumbers(ijk, 3, ",")
                              + ") XYZ ("
                              + AString::fromNumbers(xyz, 3, ",")
                              + ")...");
        createActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createVolumeFocusSelected())));
    }
    
    if (createActions.empty() == false) {
        if (this->actions().count() > 0) {
            this->addSeparator();
        }
        for (std::vector<QAction*>::iterator iter = createActions.begin();
             iter != createActions.end();
             iter++) {
            this->addAction(*iter);
        }
    }
    
    /*
     * Actions for editing
     */
    std::vector<QAction*> editActions;
    
    /*
     * Edit Surface Focus
     */
    if (focusID->isValid()) {
        const QString text = ("Edit Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ")");
        editActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editSurfaceFocusSelected())));
    }
    
    /*
     * Edit volume focus
     */
    if (focusVolID->isValid()) {
        const QString text = ("Edit Volume Focus ("
                              + focusVolID->getFocus()->getName()
                              + ")");
        editActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editVolumeFocusSelected())));
    }
    
    if (editActions.empty() == false) {
        if (this->actions().count() > 0) {
            this->addSeparator();
        }
        for (std::vector<QAction*>::iterator iter = editActions.begin();
             iter != editActions.end();
             iter++) {
            this->addAction(*iter);
        }
    }
    
    if (this->actions().count() > 0) {
        this->addSeparator();
    }
    this->addAction("Remove All Vertex Identification Symbols",
                    this,
                    SLOT(removeAllNodeIdentificationSymbolsSelected()));
    
    if (idSymbol->isValid()) {
        const AString text = ("Remove Identification of Vertices "
                              + AString::number(idSymbol->getNodeNumber()));
        
        this->addAction(WuQtUtilities::createAction(text,
                                                    "",
                                                    this,
                                                    this,
                                                    SLOT(removeNodeIdentificationSymbolSelected())));
    }
}

/**
 * Destructor.
 */
BrainOpenGLWidgetContextMenu::~BrainOpenGLWidgetContextMenu()
{
    for (std::vector<ParcelConnectivity*>::iterator parcelIter = this->parcelConnectivities.begin();
         parcelIter != this->parcelConnectivities.end();
         parcelIter++) {
        ParcelConnectivity* pc = *parcelIter;
        delete pc;
    }
}

/**
 * Called when a cifti connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void
BrainOpenGLWidgetContextMenu::parcelCiftiConnectivityActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    pc->getNodeIndices(nodeIndices);
    if (nodeIndices.empty()) {
        WuQMessageBox::errorOk(this,
                               "No vertices match label " + pc->labelName);
        return;
    }
    
    if (this->warnIfNetworkNodeCountIsLarge(pc->ciftiConnectivityManager,
                                            nodeIndices) == false) {
        return;
    }
    
    try {
        ProgressReportingDialog progressDialog("Connectivity Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        pc->ciftiConnectivityManager->loadAverageDataForSurfaceNodes(pc->surface,
                                                                      nodeIndices);
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when border cifti connectivity is selected.
 */
void
BrainOpenGLWidgetContextMenu::borderCiftiConnectivitySelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Border* border = borderID->getBorder();
    Surface* surface = borderID->getSurface();
    
    const int32_t numberOfNodes = surface->getNumberOfNodes();
    LabelFile labelFile;
    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
    const int32_t mapIndex = 0;
    
    try {
        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                         surface,
                                                         border,
                                                         false,
                                                         mapIndex,
                                                         labelKey,
                                                         &labelFile);
        std::vector<int32_t> nodeIndices;
        nodeIndices.reserve(numberOfNodes);
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
                nodeIndices.push_back(i);
            }
        }
        
        if (nodeIndices.empty()) {
            WuQMessageBox::errorOk(this,
                                   "No vertices found inside border " + border->getName());
            return;
        }
        
        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getChartingDataManager(),
                                                nodeIndices) == false) {
            return;
        }
        
        try {
            ProgressReportingDialog progressDialog("Connectivity Within Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);
            CiftiConnectivityMatrixDataFileManager* ciftiConnMann = borderID->getBrain()->getCiftiConnectivityMatrixDataFileManager();
            ciftiConnMann->loadAverageDataForSurfaceNodes(surface,
                                                          nodeIndices);
        }
        catch (const DataFileException& e) {
            WuQMessageBox::errorOk(this, e.whatString());
        }
        
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
}

///**
// * Called when border connectivity is selected.
// */
//void 
//BrainOpenGLWidgetContextMenu::borderConnectivitySelected()
//{
//    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
//    Border* border = borderID->getBorder();
//    Surface* surface = borderID->getSurface();
//    
//    const int32_t numberOfNodes = surface->getNumberOfNodes();
//    LabelFile labelFile;
//    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
//    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
//    const int32_t mapIndex = 0;
//    
//    try {
//        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
//                                                         surface,
//                                                         border,
//                                                         false,
//                                                         mapIndex,
//                                                         labelKey,
//                                                         &labelFile);
//        std::vector<int32_t> nodeIndices;
//        nodeIndices.reserve(numberOfNodes);
//        for (int32_t i = 0; i < numberOfNodes; i++) {
//            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
//                nodeIndices.push_back(i);
//            }
//        }
//        
//        if (nodeIndices.empty()) {
//            WuQMessageBox::errorOk(this,
//                                   "No vertices found inside border " + border->getName());
//            return;
//        }
//        
//        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getChartingDataManager(),
//                                                nodeIndices) == false) {
//            return;
//        }
//        
//        try {
//            ProgressReportingDialog progressDialog("Connectivity Within Border",
//                                                    "",
//                                                    this);
//            progressDialog.setValue(0);
//            const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
//            
//            QList<TimeLine> timeLines;
//            ChartingDataManager* chartingDataManger = borderID->getBrain()->getChartingDataManager();
//            chartingDataManger->loadAverageChartForSurfaceNodes(surface,
//                                                                 nodeIndices,
//                                                                true,
//                                                                 timeLines);
//            if (showAllGraphs) {
//                displayAllDataSeriesGraphs();
//            }
//            
//            GuiManager::get()->addTimeLines(timeLines);
//            EventUpdateTimeCourseDialog e;
//            EventManager::get()->sendEvent(e.getPointer());
//            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//        }
//        catch (const DataFileException& e) {
//            WuQMessageBox::errorOk(this, e.whatString());
//        }
//        
//        
//        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());    
//    }
//    catch (const AlgorithmException& e) {
//        WuQMessageBox::errorOk(this, e.whatString());
//    }
//}

/**
 * Called when a connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void 
BrainOpenGLWidgetContextMenu::parcelDataSeriesActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    pc->getNodeIndices(nodeIndices);
    if (nodeIndices.empty()) {
        WuQMessageBox::errorOk(this,
                               "No vertices match label " + pc->labelName);
        return;
    }
    
    if (this->warnIfNetworkNodeCountIsLarge(pc->chartingDataManager,
                                            nodeIndices) == false) {
        return;
    }
    
    try {
        ProgressReportingDialog progressDialog("Data Series Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        
        const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
        QList<TimeLine> timeLines;
        pc->chartingDataManager->loadAverageChartForSurfaceNodes(pc->surface,
                                                            nodeIndices,
                                                                 true,  // only files with charting enabled
                                                            timeLines);
        if(timeLines.size()!=0)
        {
            for (int i = 0; i < timeLines.size(); i++) {
                TimeLine &tl = timeLines[i];
                for(int i=0;i<3;i++) tl.point[i] = 0.0;
                tl.parcelName = pc->labelName;
                tl.structureName = StructureEnum::toGuiName(pc->surface->getStructure());
                tl.label = tl.structureName + ":" + tl.parcelName;
            }
            
            GuiManager::get()->addTimeLines(timeLines);
            
            if (showAllGraphs) {
                displayAllDataSeriesGraphs();
            }
        }
        EventUpdateTimeCourseDialog e;
        EventManager::get()->sendEvent(e.getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    catch (const DataFileException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }   
  
}

/**
 * Called when border timeseries is selected.
 */
void 
BrainOpenGLWidgetContextMenu::borderDataSeriesSelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Border* border = borderID->getBorder();
    Surface* surface = borderID->getSurface();
    
    const int32_t numberOfNodes = surface->getNumberOfNodes();
    LabelFile labelFile;
    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
    const int32_t mapIndex = 0;
    
    try {
        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                         surface,
                                                         border,
                                                         false,
                                                         mapIndex,
                                                         labelKey,
                                                         &labelFile);
        std::vector<int32_t> nodeIndices;
        nodeIndices.reserve(numberOfNodes);
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
                nodeIndices.push_back(i);
            }
        }
        
        if (nodeIndices.empty()) {
            WuQMessageBox::errorOk(this,
                                   "No vertices found inside border " + border->getName());
            return;
        }
        
        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getChartingDataManager(),
                                                nodeIndices) == false) {
            return;
        }
        
        try {
            ProgressReportingDialog progressDialog("Data Series Within Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);
//            TimeLine tl;
//            for(int i=0;i<3;i++) tl.point[i] = 0.0;
//            tl.borderClassName = border->getClassName();
//            tl.borderName = border->getName();
//            tl.structureName = StructureEnum::toGuiName(border->getStructure());
//            tl.label =  tl.structureName + ":" + tl.borderClassName + ":" + tl.borderName;
            
            const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
            ChartingDataManager* chartingDataManager = borderID->getBrain()->getChartingDataManager();
            QList<TimeLine> timeLines;
            chartingDataManager->loadAverageChartForSurfaceNodes(surface,
                                                                 nodeIndices,
                                                                 true,  // only files with charting enabled
                                                                 timeLines);
            if (timeLines.empty() == false) {
                const int numTimelines = timeLines.size();
                for (int itl = 0; itl < numTimelines; itl++) {
                    for(int i=0;i<3;i++) {
                        timeLines[itl].point[i] = 0.0;
                    }
                    timeLines[itl].borderClassName = border->getClassName();
                    timeLines[itl].borderName = border->getName();
                    timeLines[itl].structureName = StructureEnum::toGuiName(border->getStructure());
                    timeLines[itl].label =  timeLines[itl].structureName + ":" + timeLines[itl].borderClassName + ":" + timeLines[itl].borderName;
                }
                
                if (showAllGraphs) {
                    displayAllDataSeriesGraphs();
                }
                
                GuiManager::get()->addTimeLines(timeLines);
                EventUpdateTimeCourseDialog e;
                EventManager::get()->sendEvent(e.getPointer());
                EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
            }
            
//            ConnectivityLoaderManager* connectivityLoaderManager = borderID->getBrain()->getConnectivityLoaderManager();
//            const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
//            connectivityLoaderManager->loadAverageTimeSeriesForSurfaceNodes(surface,
//                                                                          nodeIndices,tl);
//            QList <TimeLine> tlV;
//            connectivityLoaderManager->getSurfaceTimeLines(tlV);
//            if(tlV.size()!=0)
//            {
//                if (showAllGraphs) {
//                    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//                    displayAllDataSeriesGraphs();
//                }
//                GuiManager::get()->addTimeLines(tlV);
//            }
//            EventUpdateTimeCourseDialog e;
//            EventManager::get()->sendEvent(e.getPointer());

        }
        catch (const DataFileException& e) {
            WuQMessageBox::errorOk(this, e.whatString());
        }   
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }

}

/**
 * @return If no data series graphs are enabled, enable all of them and return 
 * true.  Else, return false.
 */
bool
BrainOpenGLWidgetContextMenu::enableDataSeriesGraphsIfNoneEnabled()
{
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<ChartableInterface*> chartFiles;
    brain->getAllChartableDataFiles(chartFiles);
    if (chartFiles.empty()) {
        return false;
    }
    
    /*
     * Exit if any data series graph is enabled.
     */
    for (std::vector<ChartableInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableInterface* chartFile = *iter;
        if (chartFile->isChartingEnabled()) {
            return false;
        }
    }
    
    /*
     * Enable and display all data series graphs.
     */
    for (std::vector<ChartableInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableInterface* chartFile = *iter;
        chartFile->setChartingEnabled(true);
    }

    return true;
}

/**
 * Display all data-series graphs.
 */
void
BrainOpenGLWidgetContextMenu::displayAllDataSeriesGraphs()
{
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<ChartableInterface*> chartFiles;
    brain->getAllChartableDataFiles(chartFiles);
    for (std::vector<ChartableInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableInterface* chartFile = *iter;
        chartFile->setChartingEnabled(true);
        TimeCourseDialog* tcd = GuiManager::get()->getTimeCourseDialog(chartFile);
        tcd->setTimeSeriesGraphEnabled(true);
        tcd->show();
    }    
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceBorderSelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Brain* brain = borderID->getBrain();
    this->identificationManager->clearOtherSelectedItems(borderID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to create a focus at a node location
 */
void
BrainOpenGLWidgetContextMenu::createSurfaceFocusSelected()
{
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    const Surface* surface = surfaceID->getSurface();
    const int32_t nodeIndex = surfaceID->getNodeNumber();
    const float* xyz = surface->getCoordinate(nodeIndex);
    
    const AString focusName = (StructureEnum::toGuiName(surface->getStructure())
                               + " Vertex "
                               + AString::number(nodeIndex));
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}


/**
 * Called to create a focus at a node location
 */
void
BrainOpenGLWidgetContextMenu::createSurfaceIDSymbolFocusSelected()
{
    SelectionItemSurfaceNodeIdentificationSymbol* nodeSymbolID =
        this->identificationManager->getSurfaceNodeIdentificationSymbol();
    
    const Surface* surface = nodeSymbolID->getSurface();
    const int32_t nodeIndex = nodeSymbolID->getNodeNumber();
    const float* xyz = surface->getCoordinate(nodeIndex);
    
    const AString focusName = (StructureEnum::toGuiName(surface->getStructure())
                               + " Vertex "
                               + AString::number(nodeIndex));
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}
/**
 * Called to create a focus at a voxel location
 */
void
BrainOpenGLWidgetContextMenu::createVolumeFocusSelected()
{
    SelectionItemVoxel* voxelID = this->identificationManager->getVoxelIdentification();
    const VolumeMappableInterface* vf = voxelID->getVolumeFile();
    int64_t ijk[3];
    voxelID->getVoxelIJK(ijk);
    float xyz[3];
    vf->indexToSpace(ijk, xyz);
    
    const CaretMappableDataFile* cmdf = dynamic_cast<const CaretMappableDataFile*>(vf);
    const AString focusName = (cmdf->getFileNameNoPath()
                               + " IJK ("
                               + AString::fromNumbers(ijk, 3, ",")
                               + ")");
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}


/**
 * Called to display identication information on the surface focus.
 */
void
BrainOpenGLWidgetContextMenu::identifySurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    Brain* brain = focusID->getBrain();
    this->identificationManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to display identication information on the volume focus.
 */
void
BrainOpenGLWidgetContextMenu::identifyVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->identificationManager->getVolumeFocusIdentification();
    Brain* brain = focusID->getBrain();
    this->identificationManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to edit the surface focus.
 */
void
BrainOpenGLWidgetContextMenu::editSurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentWidget);
}

/**
 * Called to edit the volume focus.
 */
void
BrainOpenGLWidgetContextMenu::editVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->identificationManager->getVolumeFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentWidget);
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceNodeSelected()
{    
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    Brain* brain = surfaceID->getBrain();
    this->identificationManager->clearOtherSelectedItems(surfaceID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    Surface* surface = surfaceID->getSurface();
    const StructureEnum::Enum structure = surface->getStructure();
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItemNode(idMessage,
                                                        structure,
                                                        surface->getNumberOfNodes(),
                                                        surfaceID->getNodeNumber()));
    
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifyVoxelSelected()
{
    SelectionItemVoxel* voxelID = this->identificationManager->getVoxelIdentification();
    Brain* brain = voxelID->getBrain();
    this->identificationManager->clearOtherSelectedItems(voxelID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to remove all node identification symbols.
 */
void 
BrainOpenGLWidgetContextMenu::removeAllNodeIdentificationSymbolsSelected()
{
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    idManager->removeAllIdentifiedItems();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called to remove node identification symbol from node.
 */
void
BrainOpenGLWidgetContextMenu::removeNodeIdentificationSymbolSelected()
{
   SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = identificationManager->getSurfaceNodeIdentificationSymbol();
    if (idSymbol->isValid()) {
        Surface* surface = idSymbol->getSurface();
        
        IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
        idManager->removeIdentifiedNodeItem(surface->getStructure(),
                                            surface->getNumberOfNodes(),
                                            idSymbol->getNodeNumber());

        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}
/**
 * If any enabled connectivity files retrieve data from the network
 * and the number of nodes is large, warn the user since this will
 * be a very slow operation.
 *
 * @param clm
 *    The connectivity manager.
 * @param nodeIndices
 *    Indices of nodes that will have connectivity data retrieved.
 * @return
 *    true if process should continue, false if user cancels.
 */
bool
BrainOpenGLWidgetContextMenu::warnIfNetworkNodeCountIsLarge(const CiftiConnectivityMatrixDataFileManager* cmdf,
                                                            const std::vector<int32_t>& nodeIndices)
{
    const int32_t numNodes = static_cast<int32_t>(nodeIndices.size());
    if (numNodes < 200) {
        return true;
    }
    
    if (cmdf->hasNetworkFiles() == false) {
        return true;
    }
    
    const QString msg = ("There are "
                         + QString::number(numNodes)
                         + " vertices in the selected region.  Loading data from the network for "
                         + "this quantity of vertices may take a very long time.");
    const bool result = WuQMessageBox::warningYesNo(this,
                                                    "Do you want to continue?",
                                                    msg);
    return result;
}


/**
 * If any enabled connectivity files retrieve data from the network
 * and the number of nodes is large, warn the user since this will
 * be a very slow operation.
 *
 * @param chartingDataManager
 *    The charting data manager.
 * @param nodeIndices
 *    Indices of nodes that will have connectivity data retrieved.
 * @return 
 *    true if process should continue, false if user cancels.
 */
bool 
BrainOpenGLWidgetContextMenu::warnIfNetworkNodeCountIsLarge(const ChartingDataManager* chartingDataManager,
                                                            const std::vector<int32_t>& nodeIndices)
{
    const int32_t numNodes = static_cast<int32_t>(nodeIndices.size());
    if (numNodes < 200) {
        return true;
    }
    
    if (chartingDataManager->hasNetworkFiles() == false) {
        return true;
    }
    
    const QString msg = ("There are "
                         + QString::number(numNodes)
                         + " vertices in the selected region.  Loading data for "
                         + "this quantity of vertices may take a very long time.");
    const bool result = WuQMessageBox::warningYesNo(this,
                                                    "Do you want to continue?",
                                                    msg);
    return result;
}

/* ------------------------------------------------------------------------- */
/**
 * Constructor.
 */
BrainOpenGLWidgetContextMenu::ParcelConnectivity::ParcelConnectivity(CaretMappableDataFile* mappableLabelFile,
                   const int32_t labelFileMapIndex,
                   const int32_t labelKey,
                   const QString& labelName,
                   Surface* surface,
                   const int32_t nodeNumber,
                   ChartingDataManager* chartingDataManager,
                   CiftiConnectivityMatrixDataFileManager* ciftiConnectivityManager) {
    this->mappableLabelFile = mappableLabelFile;
    this->labelFileMapIndex = labelFileMapIndex;
    this->labelKey = labelKey;
    this->labelName = labelName;
    this->surface = surface;
    this->nodeNumber = nodeNumber;
    this->chartingDataManager = chartingDataManager;
    this->ciftiConnectivityManager = ciftiConnectivityManager;
}

/**
 * Destructor.
 */
BrainOpenGLWidgetContextMenu::ParcelConnectivity::~ParcelConnectivity()
{
    
}

/**
 * Get the indices inside the parcel.
 *
 * @param nodeIndicesOut
 *    Contains node indices upon exit.
 */
void
BrainOpenGLWidgetContextMenu::ParcelConnectivity::getNodeIndices(std::vector<int32_t>& nodeIndicesOut) const
{
    nodeIndicesOut.clear();
    
    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
    LabelFile* labelFile = dynamic_cast<LabelFile*>(mappableLabelFile);
    if (ciftiLabelFile != NULL) {
        ciftiLabelFile->getNodeIndicesWithLabelKey(surface->getStructure(),
                                                   surface->getNumberOfNodes(),
                                                   labelFileMapIndex,
                                                   labelKey,
                                                   nodeIndicesOut);
    }
    else if (labelFile != NULL) {
        labelFile->getNodeIndicesWithLabelKey(labelFileMapIndex,
                                              labelKey,
                                              nodeIndicesOut);
    }
    else {
        CaretAssertMessage(0,
                           "Should never get here, new or invalid label file type");
    }
    
}

/* ------------------------------------------------------------------------- */
/**
 * Constructor.
 */
BrainOpenGLWidgetContextMenu::VolumeParcelConnectivity::VolumeParcelConnectivity(CaretMappableDataFile* mappableLabelFile,
                                                                                 const int32_t labelFileMapIndex,
                                                                                 const int32_t labelKey,
                                                                                 const QString& labelName,
                                                                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                                                 const float sliceCoordinate,
                                                                                 ChartingDataManager* chartingDataManager,
                                                                                 CiftiConnectivityMatrixDataFileManager* ciftiConnectivityManager) {
    this->mappableLabelFile = mappableLabelFile;
    this->labelFileMapIndex = labelFileMapIndex;
    this->labelKey = labelKey;
    this->labelName = labelName;
    this->slicePlane = slicePlane;
    this->sliceCoordinate = sliceCoordinate;
    this->chartingDataManager = chartingDataManager;
    this->ciftiConnectivityManager = ciftiConnectivityManager;
}

/**
 * Destructor.
 */
BrainOpenGLWidgetContextMenu::VolumeParcelConnectivity::~VolumeParcelConnectivity()
{
    
}


