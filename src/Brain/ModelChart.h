#ifndef __MODEL_CHART_H__
#define __MODEL_CHART_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include <set>

#include "ChartDataTypeEnum.h"
#include "DataFileException.h"
#include "EventListenerInterface.h"
#include "Model.h"

namespace caret {

    class ChartData;
    class ChartModel;
    class ChartModelDataSeries;
    class OverlaySetArray;
    class SurfaceFile;
    
    /// Controls the display of a chart.
    class ModelChart : public Model, public EventListenerInterface  {
        
    public:
        ModelChart(Brain* brain);
        
        virtual ~ModelChart();
        
        void initializeOverlays();
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
        void loadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                         const int32_t surfaceNumberOfNodes,
                                         const int32_t nodeIndex) throw (DataFileException);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        virtual void receiveEvent(Event* event);
        
        ChartDataTypeEnum::Enum getSelectedChartDataType(const int32_t tabIndex) const;
        
        void setSelectedChartDataType(const int32_t tabIndex,
                                      const ChartDataTypeEnum::Enum dataType);
        
        ChartModel* getSelectedChartModel(const int32_t tabIndex);
        
        const ChartModel* getSelectedChartModel(const int32_t tabIndex) const;
        
        virtual void getDescriptionOfContent(const int32_t tabIndex,
                                             PlainTextStringBuilder& descriptionOut) const;
        
        virtual void copyTabContent(const int32_t sourceTabIndex,
                                    const int32_t destinationTabIndex);
        
        void reset();
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
        
    private:
        ModelChart(const ModelChart&);
        
        ModelChart& operator=(const ModelChart&);
        
        void initializeCharts();
        
        void removeAllCharts();
        
        const ChartModel* getSelectedChartModelHelper(const int32_t tabIndex) const;
        
        void saveChartModelsToScene(const SceneAttributes* sceneAttributes,
                                    SceneClass* sceneClass,
                                    const std::vector<int32_t>& tabIndices,
                                    std::set<AString>& validChartDataIDsOut);
        
        void restoreChartModelsFromScene(const SceneAttributes* sceneAttributes,
                                         const SceneClass* sceneClass);
        
        /** Overlays sets for this model and for each tab */
        OverlaySetArray* m_overlaySetArray;
        
        mutable ChartDataTypeEnum::Enum m_selectedChartDataType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Chart model for data-series data */
        ChartModelDataSeries* m_chartModelDataSeries[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Chart model for time-series data */
        ChartModelDataSeries* m_chartModelTimeSeries[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];


        SceneClassAssistant* m_sceneAssistant;
    };

} // namespace

#endif // __MODEL_CHART_H__