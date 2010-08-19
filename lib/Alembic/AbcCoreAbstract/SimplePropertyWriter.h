//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#ifndef _Alembic_AbcCoreAbstract_SimplePropertyWriter_h_
#define _Alembic_AbcCoreAbstract_SimplePropertyWriter_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/BasePropertyWriter.h>
#include <Alembic/AbcCoreAbstract/DataType.h>
#include <Alembic/AbcCoreAbstract/TimeSamplingType.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! The SimplePropertyWriter is the base class for the Scalar and Array
//! non-compound properties. The Scalar and Array properties are almost
//! entirely identical, differing only in how samples are set. They're also
//! somewhat semantically different, and worthy of a classifying separation,
//! but their functional interface is entirely the same, and hence this base
//! class.
class SimplePropertyWriter : public BasePropertyWriter
{
public:
    //! Virtual destructor
    //! ...
    virtual ~SimplePropertyWriter();

    //! Return the data type of the property
    //! All sampled properties, Scalar or Array, are characterized
    //! by the fact that every data element has the same "DataType".
    //! The DataType defines how the property's data is stored.
    //! This value is set upon construction and is an immutable
    //! characteristic of any property.
    virtual const DataType &getDataType() const = 0;

    //! Return the timeSamplingType. TimeSampling is one of four
    //! main types - Static (not time value on single sample),
    //! Uniform (time spacing between samples is always the same)
    //! Cyclic (a period variation of distances between samples)
    //! Acyclic (no pattern to how times associate with sample indices).
    //! The decision of which kind of timeSampling a property has is
    //! set upon construction and is an immutable
    //! characteristic of any property.
    virtual const TimeSamplingType &getTimeSamplingType() const = 0;

    //! Return the number of samples that have been written so far.
    //! This changes as samples are written.
    virtual size_t getNumSamples() = 0;

    //! Inherited from BasePropertyWriter
    //! No implementation herein
    //! virtual const std::string &getName() const = 0;
    
    //! Inherited from BasePropertyWriter
    //! There's actually no enum for kSimpleProperty, since
    //! this is just an intermediate type
    //! virtual PropertyType getPropertyType() const = 0;

    //! Inherited from BasePropertyWriter
    //! bool isScalar() const;

    //! Inherited from BasePropertyWriter
    //! bool isArray() const;

    //! Inherited from BasePropertyWriter
    //! bool isCompound() const;

    //! Inherited from BasePropertyWriter
    //! bool isSimple() const;

    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual const MetaData &getMetaData() const = 0;
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual ObjectWriterPtr getObject() = 0;

    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual CompoundPropertyWriterPtr getParent() = 0;
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual void appendMetaData( const MetaData &iAppend ) = 0;
    
    //! Inherited from BaseProeprtyWriter
    //! No implementation herein.
    //! virtual SimplePropertyWriterPtr asSimple();

    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual ScalarPropertyWriterPtr asScalar();
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual ArrayPropertyWriterPtr asArray();
    
    //! Inherited from BasePropertyWriter
    //! No implementation herein.
    //! virtual CompoundPropertyWriterPtr asCompound();
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif