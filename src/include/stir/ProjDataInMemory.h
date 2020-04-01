/*
    Copyright (C) 2002 - 2011-02-23, Hammersmith Imanet Ltd
    Copyright (C) 2019, UCL
    This file is part of STIR.

    This file is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    See STIR/LICENSE.txt for details
*/
/*!
  \file
  \ingroup projdata
  \brief Declaration of class stir::ProjDataInMemory

  \author Kris Thielemans
*/

#ifndef __stir_ProjDataInMemory_H__
#define __stir_ProjDataInMemory_H__

#include "stir/ProjDataFromStream.h" 
#include "boost/shared_array.hpp"
#include <string>

/* Implementation note (KT)
   
   I first used the std::stringstream class (when available).
   However, this class currently has a problem that you cannot preallocate
   a buffer size. This means that when we write to the stringstream, it will
   grow piece by piece. For some implementations (i.e. those that keep the memory
   contiguous), this might mean multiple reallocations and copying of data.
   Of course, for 'smart' implementations of stringstream, this wouldn't happen.

   So, we now use boost::interprocess::bufferstream instead. You could use
   use old style strstream instead, but that's now very deprecated, so that's not recommended.
*/
//#define STIR_USE_OLD_STRSTREAM

#if defined(BOOST_NO_STRINGSTREAM) && !defined(STIR_USE_OLD_STRSTREAM)
#define STIR_USE_OLD_STRSTREAM 
#endif

START_NAMESPACE_STIR

class Succeeded;

/*!
  \ingroup projdata
  \brief A class which reads/writes projection data from/to memory.

  Mainly useful for temporary storage of projection data.

*/
class ProjDataInMemory : public ProjDataFromStream
{
public: 
    
  //! constructor with only info, but no data
  /*! 
    \param proj_data_info_ptr object specifying all sizes etc.
      The ProjDataInfo object pointed to will not be modified.
    \param initialise_with_0 specifies if the data should be set to 0. 
        If \c false, the data is undefined until you set it yourself.
  */
  ProjDataInMemory (shared_ptr<ExamInfo> const& exam_info_sptr,
		    shared_ptr<const ProjDataInfo> const& proj_data_info_ptr,
                    const bool initialise_with_0 = true);

  //! constructor that copies data from another ProjData
  ProjDataInMemory (const ProjData& proj_data);

  //! Copy constructor
  ProjDataInMemory (const ProjDataInMemory& proj_data);

  //! destructor deallocates all memory the object owns
  virtual ~ProjDataInMemory();
 
  //! Returns a  value of a bin
  float get_bin_value(Bin& bin);
    
private:
  // an auto_ptr doesn't work in gcc 2.95.2 because of assignment problems, so we use shared_array
  // note however that the buffer is not shared. we just use it such that its memory gets 
  // deallocated automatically.
  boost::shared_array<char> buffer;
  
  size_t get_size_of_buffer() const;

  //! allocates buffer for storing the data. Has to be called by constructors before create_stream()
  char * create_buffer(const bool initialise_with_0 = false) const;

  //! Create a new stream
  shared_ptr<std::iostream> create_stream() const;
};

END_NAMESPACE_STIR


#endif
