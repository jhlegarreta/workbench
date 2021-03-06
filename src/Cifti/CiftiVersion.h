#ifndef __CIFTI_VERSION_H__
#define __CIFTI_VERSION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <QString>

#include "stdint.h"

namespace caret
{
    class CiftiVersion
    {
        int16_t m_major, m_minor;
    public:
        int16_t getMajor() const { return m_major; }
        int16_t getMinor() const { return m_minor; }
        
        CiftiVersion();
        CiftiVersion(const int16_t& major, const int16_t& minor);
        CiftiVersion(const QString& versionString);
        QString toString() const;
        bool operator<(const CiftiVersion& rhs) const;
        bool operator>(const CiftiVersion& rhs) const;
        bool operator==(const CiftiVersion& rhs) const;
        bool operator!=(const CiftiVersion& rhs) const;
        bool operator<=(const CiftiVersion& rhs) const;
        bool operator>=(const CiftiVersion& rhs) const;
        ///quirk tests
        bool hasReversedFirstDims() const;
    };
}

#endif //__CIFTI_VERSION_H__
