#ifndef __CHART_AXIS_CARTESIAN_H__
#define __CHART_AXIS_CARTESIAN_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
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


#include "ChartAxis.h"



namespace caret {

    class ChartModelCartesian;
    
    class ChartAxisCartesian : public ChartAxis {
        
    public:
        virtual ~ChartAxisCartesian();
        
        ChartAxisCartesian(const ChartAxisCartesian& obj);

        ChartAxisCartesian& operator=(const ChartAxisCartesian& obj);
        
        virtual ChartAxis* clone() const;
        
        float getMinimumValue() const;
        
        void setMinimumValue(const float minimumValue);
        
        float getMaximumValue() const;
        
        void setMaximumValue(const float maximumValue);
        
        float getStepValue() const;
        
        int32_t getDigitsRightOfDecimal() const;
        
        void getLabelsAndPositions(const float axisLengthInPixels,
                                   const float fontSizeInPixels,
                                   std::vector<float>& labelOffsetInPixelsOut,
                                   std::vector<AString>& labelTextOut);
        // ADD_NEW_METHODS_HERE
          
    protected:
        ChartAxisCartesian(const ChartAxisLocationEnum::Enum axisLocation);
        
        virtual void updateForAutoRangeScale();
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperChartAxisCartesian(const ChartAxisCartesian& obj);
        
        void initializeMembersChartAxisCartesian();

        SceneClassAssistant* m_sceneAssistant;

        mutable float m_maximumValue;
        
        mutable float m_minimumValue;
        
        mutable float m_stepValue;
        
        mutable int32_t m_digitsRightOfDecimal;
        
        // ADD_NEW_MEMBERS_HERE
        
    friend class ChartAxis;

    };
    
#ifdef __CHART_AXIS_CARTESIAN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_AXIS_CARTESIAN_DECLARE__

} // namespace
#endif  //__CHART_AXIS_CARTESIAN_H__