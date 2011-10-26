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

#include "CaretLogger.h"
#include "DescriptiveStatistics.h"
#include "ElapsedTimer.h"
#include "GiftiDataArray.h"
#include "GiftiFile.h"
#include "GiftiMetaData.h"
#include "GiftiTypeFile.h"
#include "GiftiMetaDataXmlElements.h"
#include "PaletteColorMapping.h"
#include "PaletteColorMappingSaxReader.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * Constructor.
 */
GiftiTypeFile::GiftiTypeFile(const DataFileTypeEnum::Enum dataFileType)
: CaretMappableDataFile(dataFileType)
{
    this->initializeMembersGiftiTypeFile();   
}

/**
 * Destructor.
 */
GiftiTypeFile::~GiftiTypeFile()
{
    if (this->giftiFile != NULL) {
        delete this->giftiFile;
        this->giftiFile = NULL;
    }
}

/**
 * Copy Constructor.
 *
 * @param gtf
 *    File that is copied.
 */
GiftiTypeFile::GiftiTypeFile(const GiftiTypeFile& gtf)
: CaretMappableDataFile(gtf)
{
    this->copyHelperGiftiTypeFile(gtf);
}

/**
 * Assignment operator.
 *
 * @param gtf
 *     File whose contents are copied to this file.
 */
GiftiTypeFile& 
GiftiTypeFile::operator=(const GiftiTypeFile& gtf)
{
    if (this != &gtf) {
        CaretMappableDataFile::operator=(gtf);
        this->copyHelperGiftiTypeFile(gtf);
    }
    return *this;
}

/**
 * Clear the contents of this file.
 */
void 
GiftiTypeFile::clear()
{
    DataFile::clear();
    this->giftiFile->clear(); 
}

/**
 * Clear modified status.
 */
void 
GiftiTypeFile::clearModified()
{
    DataFile::clearModified();
    this->giftiFile->clearModified();
}

/**
 * Is this file modified?
 *
 * @return true if modified, else false.
 */
bool 
GiftiTypeFile::isModified() const
{
    if (DataFile::isModified()) {
        return true;
    }
    if (this->giftiFile->isModified()) {
        return true;
    }
    return false;
}

/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
GiftiTypeFile::isEmpty() const
{
    return this->giftiFile->isEmpty();
}

/**
 * Read the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
GiftiTypeFile::readFile(const AString& filename) throw (DataFileException)
{
    ElapsedTimer et;
    et.start();
    this->giftiFile->readFile(filename);
    this->validateDataArraysAfterReading();
    this->clearModified();
    
    this->setFileName(filename);
    
    AString msg = ("Time to read " 
                   + filename 
                   + " was " 
                   + AString::number(et.getElapsedTimeSeconds())
                   + " seconds.");
    CaretLogInfo(msg);
}

/**
 * Write the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
GiftiTypeFile::writeFile(const AString& filename) throw (DataFileException)
{
    this->giftiFile->writeFile(filename);
    this->clearModified();
}
/**
 * Helps with file copying.
 * 
 * @param gtf
 *    File that is copied.
 */
void 
GiftiTypeFile::copyHelperGiftiTypeFile(const GiftiTypeFile& gtf)
{
    if (this->giftiFile != NULL) {
        delete this->giftiFile;
    }
    this->giftiFile = new GiftiFile(*gtf.giftiFile);
    this->validateDataArraysAfterReading();
}

/**
 * Initialize members of this class.
 */
void 
GiftiTypeFile::initializeMembersGiftiTypeFile()
{
    this->giftiFile = new GiftiFile();
}

/**
 * Get information about this file's contents.
 * @return
 *    Information about the file's contents.
 */
AString 
GiftiTypeFile::toString() const
{
    return this->giftiFile->toString();
}

StructureEnum::Enum 
GiftiTypeFile::getStructure() const
{
    AString structurePrimaryName;
    
    /*
     * Surface contains anatomical structure in pointset array.
     */
    const SurfaceFile* surfaceFile = dynamic_cast<const SurfaceFile*>(this);
    if (surfaceFile != NULL) {
        const GiftiDataArray* gda = this->giftiFile->getDataArrayWithIntent(NiftiIntentEnum::NIFTI_INTENT_POINTSET);
        const GiftiMetaData* metadata = gda->getMetaData();
        structurePrimaryName = metadata->get(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY);
    }
    else {
        const GiftiMetaData* metadata = this->giftiFile->getMetaData();
        structurePrimaryName = metadata->get(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY);
    }
    
    bool isValid = false;
    StructureEnum::Enum structure = StructureEnum::fromGuiName(structurePrimaryName, &isValid);
    return structure;
}

/**
 * Set the structure.
 * @param structure 
 *    New value for file's structure.
 */
void 
GiftiTypeFile::setStructure(const StructureEnum::Enum structure)
{
    const AString structureName = StructureEnum::toGuiName(structure);
    /*
     * Surface contains anatomical structure in pointset array.
     */
    SurfaceFile* surfaceFile = dynamic_cast<SurfaceFile*>(this);
    if (surfaceFile != NULL) {
        GiftiDataArray* gda = this->giftiFile->getDataArrayWithIntent(NiftiIntentEnum::NIFTI_INTENT_POINTSET);
        GiftiMetaData* metadata = gda->getMetaData();
        metadata->set(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY,
                      structureName);
    }
    else {
        GiftiMetaData* metadata = this->giftiFile->getMetaData();
        metadata->set(GiftiMetaDataXmlElements::METADATA_NAME_ANATOMICAL_STRUCTURE_PRIMARY,
                      structureName);
    }
    
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
GiftiTypeFile::getFileMetaData()
{
    return this->giftiFile->getMetaData();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
GiftiTypeFile::getFileMetaData() const
{
    return this->giftiFile->getMetaData();
}

/**
 * Verify that all of the data arrays have the same number of rows.
 * @throws DataFileException
 *    If there are data arrays that have a different number of rows.
 */
void 
GiftiTypeFile::verifyDataArraysHaveSameNumberOfRows(const int32_t minimumSecondDimension,
                                                    const int32_t maximumSecondDimension) const throw (DataFileException)
{
    const int32_t numberOfArrays = this->giftiFile->getNumberOfDataArrays();
    if (numberOfArrays > 1) {
        /*
         * Verify all arrays contain the same number of rows.
         */
        int64_t numberOfRows = this->giftiFile->getDataArray(0)->getNumberOfRows();        
        for (int32_t i = 1; i < numberOfArrays; i++) {
            const int32_t arrayNumberOfRows = this->giftiFile->getDataArray(i)->getNumberOfRows();
            if (numberOfRows != arrayNumberOfRows) {
                AString message = "All data arrays (columns) in the file must have the same number of rows.";
                message += "  The first array (column) contains " + AString::number(numberOfRows) + " rows.";
                message += "  Array " + AString::number(i + 1) + " contains " + AString::number(arrayNumberOfRows) + " rows.";
                DataFileException e(message);
                CaretLogThrowing(e);
                throw e;                                     
            }
        }
        
        /*
         * Verify that second dimensions is within valid range.
         */
        for (int32_t i = 0; i < numberOfArrays; i++) {
            const GiftiDataArray* gda = this->giftiFile->getDataArray(i);
            const int32_t numberOfDimensions = gda->getNumberOfDimensions();
            if (numberOfDimensions > 2) {
                DataFileException e("Data array "
                                    + AString::number(i + 1)
                                    + " contains "
                                    + AString::number(numberOfDimensions)
                                    + " dimensions.  Two is the maximum allowed.");
                CaretLogThrowing(e);
                throw e;
            }
            
            int32_t secondDimension = 0;
            if (numberOfDimensions > 1) {
                secondDimension = gda->getDimension(1);
                if (secondDimension == 1) {
                    secondDimension = 0;
                }
            }
            
            if ((secondDimension < minimumSecondDimension) 
                || (secondDimension > maximumSecondDimension)) {
                DataFileException e("Data array "
                                    + AString::number(i + 1)
                                    + " second dimension is "
                                    + AString::number(numberOfDimensions)
                                    + ".  Minimum allowed is "
                                    + AString::number(minimumSecondDimension)
                                    + ".  Maximum allowed is "
                                    + AString::number(maximumSecondDimension));
                CaretLogThrowing(e);
                throw e;
            }
        }
    }
}
   
/**
 * Get the name of a file column.
 * @param columnIndex
 *    Index of column.
 * @return
 *    Name of column.
 */
AString 
GiftiTypeFile::getColumnName(const int columnIndex) const
{
    return this->giftiFile->getDataArrayName(columnIndex);
}

/**
 * Find the first column with the given column name.
 * @param columnName
 *     Name of column.
 * @return
 *     Index of column with name or negative if no match.
 */
int32_t 
GiftiTypeFile::getColumnIndexFromColumnName(const AString& columnName) const
{
    return this->giftiFile->getDataArrayWithNameIndex(columnName);
}

/**
 * Set the name of a column.
 * @param columnIndex
 *    Index of column.
 * @param columnName
 *    New name for column.
 */
void 
GiftiTypeFile::setColumnName(const int32_t columnIndex,
                             const AString& columnName)
{
    this->giftiFile->setDataArrayName(columnIndex, columnName);
}

/**
 * @return The palette color mapping for a data column.
 */
PaletteColorMapping* 
GiftiTypeFile::getPaletteColorMapping(const int32_t columnIndex)
{
    GiftiDataArray* gda = this->giftiFile->getDataArray(columnIndex);
    return gda->getPaletteColorMapping();
}




/**
 * @return Is the data mappable to a surface?
 */
bool 
GiftiTypeFile::isSurfaceMappable() const
{
    return true;
}

/**
 * @return Is the data mappable to a volume?
 */
bool 
GiftiTypeFile::isVolumeMappable() const
{
    return false;
}

/**
 * @return The number of maps in the file.  
 * Note: Caret5 used the term 'columns'.
 */
int32_t 
GiftiTypeFile::getNumberOfMaps() const
{
    return this->giftiFile->getNumberOfDataArrays();
}

/**
 * Get the name of the map at the given index.
 * 
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString 
GiftiTypeFile::getMapName(const int32_t mapIndex) const
{
    return this->giftiFile->getDataArrayName(mapIndex);
}

/**
 * Find the index of the map that uses the given name.
 * 
 * @param mapName
 *    Name of the desired map.
 * @return
 *    Index of the map using the given name.  If there is more
 *    than one map with the given name, this method is likely
 *    to return the index of the first map with the name.
 */
int32_t 
GiftiTypeFile::getMapIndexFromName(const AString& mapName)
{
    return this->giftiFile->getDataArrayWithNameIndex(mapName);
}

/**
 * Set the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mapName
 *    New name for the map.
 */
void 
GiftiTypeFile::setMapName(const int32_t mapIndex,
                        const AString& mapName)
{
    this->giftiFile->setDataArrayName(mapIndex, mapName);
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map (const value).
 */         
const GiftiMetaData* 
GiftiTypeFile::getMapMetaData(const int32_t mapIndex) const
{
    return this->giftiFile->getDataArray(mapIndex)->getMetaData();
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map.
 */         
GiftiMetaData* 
GiftiTypeFile::getMapMetaData(const int32_t mapIndex)
{
    return this->giftiFile->getDataArray(mapIndex)->getMetaData();
}

/**
 * Get statistics describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */         
const DescriptiveStatistics* 
GiftiTypeFile::getMapStatistics(const int32_t mapIndex)
{
    DescriptiveStatistics* stats = new DescriptiveStatistics();
    return stats;
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool 
GiftiTypeFile::isMappedWithPalette() const
{
    if (this->getDataFileType() == DataFileTypeEnum::METRIC) {
        return true;
    }
    return false;
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (will be NULL for data
 *    not mapped using a palette).
 */         
PaletteColorMapping* 
GiftiTypeFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    GiftiDataArray* gda = this->giftiFile->getDataArray(mapIndex);
    return gda->getPaletteColorMapping();    
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (constant) (will be NULL for data
 *    not mapped using a palette).
 */         
const PaletteColorMapping* 
GiftiTypeFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
    const GiftiDataArray* gda = this->giftiFile->getDataArray(mapIndex);
    return gda->getPaletteColorMapping();
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool 
GiftiTypeFile::isMappedWithLabelTable() const
{
    if (this->getDataFileType() == DataFileTypeEnum::LABEL) {
        return true;
    }
    return false;
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (will be NULL for data
 *    not mapped using a label table).
 */         
GiftiLabelTable* 
GiftiTypeFile::getMapLabelTable(const int32_t /*mapIndex*/)
{
    /*
     * Use file's label table since GIFTI uses one
     * label table for all data arrays.
     */
    return this->giftiFile->getLabelTable();
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (constant) (will be NULL for data
 *    not mapped using a label table).
 */         
const GiftiLabelTable* 
GiftiTypeFile::getMapLabelTable(const int32_t /*mapIndex*/) const
{
    /*
     * Use file's label table since GIFTI uses one
     * label table for all data arrays.
     */
    return this->giftiFile->getLabelTable();
}

