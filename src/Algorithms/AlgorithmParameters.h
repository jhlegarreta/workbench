#ifndef __ALGORITHM_PARAMETERS_H__
#define __ALGORITHM_PARAMETERS_H__

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

#include "AString.h"
#include <vector>
#include "stdint.h"
#include "CaretPointer.h"

namespace caret {

    class SurfaceFile;
    class VolumeFile;
    class MetricFile;
    class LabelFile;
    class CiftiFile;
    
    class AlgorithmParserInterface;
    class AutoAlgorithmInterface;
    struct AbstractOutputParameter;
    struct OptionalParameter;
    
    struct AbstractParameter
    {
        enum parameterType {
            SURFACE,
            VOLUME,
            METRIC,
            LABEL,
            CIFTI,
            DOUBLE,
            INT,
            STRING,
            BOOL
        };
        int32_t m_key;//identifies this parameter uniquely for this algorithm
        AString m_shortName, m_description;
        virtual parameterType getType() = 0;
        AbstractParameter(int32_t key, const AString& shortName, const AString& description) :
        m_key(key),
        m_shortName(shortName),
        m_description(description)
        {
        };
        virtual ~AbstractParameter();
    };
    
    struct ParameterComponent
    {//sadly, inheriting from a friend class doesn't give you access to private members, so these are entirely public so parsers can use them
        std::vector<AbstractParameter*> m_paramList;//mandatory arguments
        std::vector<OptionalParameter*> m_optionList;//optional arguments
        
        ///constructor
        ParameterComponent();
        
        ///destructor
        virtual ~ParameterComponent();
        
        //convenience methods for algorithms to use to easily specify parameters
        ///add a parameter to get next item as a string
        void addStringParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a string with a key
        const AString& getString(const int32_t key);
        
        ///add a parameter to get next item as an int32
        void addIntParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get an integer with a key
        int64_t getInt(const int32_t key);
        
        ///add a parameter to get next item as a double
        void addDoubleParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a double with a key
        double getDouble(const int32_t key);
        
        ///add a parameter to get next item as a surface
        void addSurfaceParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a surface with a key
        SurfaceFile* getSurface(const int32_t key);
        
        ///add a parameter to get next item as a volume
        void addVolumeParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a volume with a key
        VolumeFile* getVolume(const int32_t key);
        
        ///add a parameter to get next item as a functional file (metric)
        void addMetricParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a metric with a key
        MetricFile* getMetric(const int32_t key);
        
        ///add a parameter to get next item as a label file
        void addLabelParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a label with a key
        LabelFile* getLabel(const int32_t key);
        
        ///add a parameter to get next item as a cifti file - TODO: make methods for different cifti types?
        void addCiftiParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a cifti with a key
        CiftiFile* getCifti(const int32_t key);
        
        ///add a preconstructed optional parameter
        void addOptionalParameter(OptionalParameter* param);
        
        ///return pointer to an input parameter
        AbstractParameter* getInputParameter(const int32_t key, const AbstractParameter::parameterType type);
        
        ///return pointer to an option
        OptionalParameter* getOptionalParameter(const int32_t key);
        
    };
    
    struct OptionalParameter : public ParameterComponent
    {
        int32_t m_key;//uniquely identifies this option
        AString m_optionSwitch, m_shortName, m_description;
        bool m_present;//to be filled by parser
        OptionalParameter(int32_t key, const AString& optionSwitch, const AString& shortName, const AString& description) :
        m_key(key),
        m_optionSwitch(optionSwitch),
        m_shortName(shortName),
        m_description(description)
        {
            m_present = false;
        }
    };
    
    struct AlgorithmParameters : public ParameterComponent
    {
        std::vector<AbstractParameter*> m_outputList;//should this be a different type? input and output parameters are very similar, just pointers to files
        AString m_helpText;//formatted by the parser object for display in terminal or modal window

        ///constructor
        AlgorithmParameters();
        
        ///destructor
        ~AlgorithmParameters();
        
        ///set the help text of the algorithm - you DO NOT need to add newlines within paragraphs or list the parameters, or give a description of each parameter! describe ONLY what it does, plus any quirks
        void setHelpText(const AString& textIn);
        
        ///get the unformatted help text, without command or arguments descriptions, to be formatted by the argument parser
        const AString& getHelpText();
        
        //convenience methods for algorithms to use to easily specify parameters
        ///add a parameter to get next item as a string
        void addStringOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a string with a key
        AString* getOutputString(const int32_t key);
        
        ///add a parameter to get next item as an int32
        void addIntOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get an integer with a key
        int64_t* getOutputInt(const int32_t key);
        
        ///add a parameter to get next item as a double
        void addDoubleOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a double with a key
        double* getOutputDouble(const int32_t key);
        
        ///add a parameter to get next item as a surface
        void addSurfaceOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a surface with a key
        SurfaceFile* getOutputSurface(const int32_t key);
        
        ///add a parameter to get next item as a volume
        void addVolumeOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a volume with a key
        VolumeFile* getOutputVolume(const int32_t key);
        
        ///add a parameter to get next item as a functional file (metric)
        void addMetricOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a metric with a key
        MetricFile* getOutputMetric(const int32_t key);
        
        ///add a parameter to get next item as a label file
        void addLabelOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a label with a key
        LabelFile* getOutputLabel(const int32_t key);
        
        ///add a parameter to get next item as a cifti file - TODO: make methods for different cifti types?
        void addCiftiOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a cifti with a key
        CiftiFile* getOutputCifti(const int32_t key);
        
        ///return pointer to an output
        AbstractParameter* getOutputParameter(const int32_t key, const AbstractParameter::parameterType type);
        
    };

    //templates for the common cases
    template<typename T, AbstractParameter::parameterType TYPE>
    struct PointerTemplateParameter : public AbstractParameter
    {
        virtual parameterType getType() { return TYPE; };
        CaretPointer<T> m_parameter;//so the GUI parser and the commandline parser don't need to do different things to delete the parameter info
        PointerTemplateParameter(const int32_t key, const AString& shortName, const AString& description) : AbstractParameter(key, shortName, description)
        {
            m_parameter = NULL;
        };
    };
    
    template<typename T, AbstractParameter::parameterType TYPE>
    struct PrimitiveTemplateParameter : public AbstractParameter
    {
        virtual parameterType getType() { return TYPE; };
        T m_parameter;
        PrimitiveTemplateParameter(const int32_t key, const AString& shortName, const AString& description) : AbstractParameter(key, shortName, description)
        {
            m_parameter = 0;
        };
    };
    
    //some friendlier names
    typedef PointerTemplateParameter<SurfaceFile, AbstractParameter::SURFACE> SurfaceParameter;
    typedef PointerTemplateParameter<VolumeFile, AbstractParameter::VOLUME> VolumeParameter;
    typedef PointerTemplateParameter<MetricFile, AbstractParameter::METRIC> MetricParameter;
    typedef PointerTemplateParameter<LabelFile, AbstractParameter::LABEL> LabelParameter;
    typedef PointerTemplateParameter<CiftiFile, AbstractParameter::CIFTI> CiftiParameter;
    typedef PrimitiveTemplateParameter<double, AbstractParameter::DOUBLE> DoubleParameter;
    typedef PrimitiveTemplateParameter<int64_t, AbstractParameter::INT> IntParameter;
    typedef PrimitiveTemplateParameter<AString, AbstractParameter::STRING> StringParameter;
    typedef PrimitiveTemplateParameter<bool, AbstractParameter::BOOL> BooleanParameter;
    
    class AlgParamParserInterface
    {
    public:
        AlgParamParserInterface(AutoAlgorithmInterface* myAutoAlg);
    };
}

#endif //__ALGORITHM_PARAMETERS_H__
