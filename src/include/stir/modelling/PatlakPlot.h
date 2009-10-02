//
// $Id$
//
/*
    Copyright (C) 2006 - $Date$, Hammersmith Imanet Ltd
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
  \ingroup modelling
  \brief Implementation of functions of class stir::PatlakPlot

  \author Charalampos Tsoumpas

  $Date$
  $Revision$
*/


#ifndef __stir_modelling_PatlakPlot_H__
#define __stir_modelling_PatlakPlot_H__

#include "local/stir/modelling/KineticModel.h"
#include "local/stir/modelling/ModelMatrix.h"
#include "local/stir/modelling/BloodFrameData.h"
#include "local/stir/modelling/PlasmaData.h"
#include "stir/Succeeded.h"
#include "stir/RegisteredParsingObject.h"

START_NAMESPACE_STIR

//!
/*!
  \ingroup modelling
  \brief Implementation of functions of class stir::PatlakPlot

  \par Example .par file
  \verbatim
  Patlak Plot Parameters:=

  time frame definition filename := frames.txt
  starting frame := 23
  calibration factor := 9000
  blood data filename :=  blood_file.txt
  ; In seconds
  Time Shift := 0
  In total counts := 1

  end Patlak Plot Parameters:=
  \endverbatim

  \warning
  - The dynamic images will be calibrated only if the calibration factor is given. 
  - The [if_total_cnt] is set to true the Dynamic Image will have the total number of 
    counts while if set to false it will have the total_number_of_counts/get_duration(frame_num).
  - The dynamic images will always be in decaying counts.
  - The plasma data is assumed to be in decaying counts.

  \todo Should be derived from LinearModels, but when non-linear models will be introduced, as well.  
*/
class PatlakPlot : public RegisteredParsingObject<PatlakPlot, KineticModel> 
{
  public:
  //! Name which will be used when parsing a PatlakPlot object
  static const char * const registered_name; 

   PatlakPlot();   //!< Default constructor (calls set_defaults())
   ~PatlakPlot();   //!< default destructor
   //! \name Functions to get parameters @{
    //! Simply gets model matrix, if it has been already stored.
    ModelMatrix<2> get_model_matrix() const;
    //! Creates model matrix from plasma data (Must be already sorted in appropriate frames).
    ModelMatrix<2> get_model_matrix(const PlasmaData& plasma_data, 
				    const TimeFrameDefinitions& time_frame_definitions, 
				    const unsigned int starting_frame);
   //! Returns the frame that the PatlakPlot linearization is assumed to be valid.
    unsigned int
      get_starting_frame() const ;
    //! Returns the TimeFrameDefinitions that the PatlakPlot linearization is assumed to be valid: ChT::Check
    TimeFrameDefinitions 
      get_time_frame_definitions() const ;
#if 0
    /*! Creates model matrix from blood frame data.
    /todo It has to get re-implemented. At the moment it is inactivated using the preprocessor.    */
    ModelMatrix<2> get_model_matrix(const BloodFrameData& blood_frame_data, const unsigned int starting_frame);
#endif
    //!@}
    //! \name Functions to set parameters
    void set_model_matrix(ModelMatrix<2> model_matrix) ;     //!< Simply set model matrix 

    /*! Multiplies the dynamic image with the model gradient. \n
      For a linear model the model gradient is the transpose of the model matrix. \n
      So, the dynamic image is "projected" from time domain to the parameter domain.
    */  
    void
      multiply_dynamic_image_with_model_gradient(ParametricVoxelsOnCartesianGrid & parametric_image,
						 const DynamicDiscretisedDensity & dyn_image);

    //! Multiplies the parametric image with the model matrix to get the corresponding dynamic image.
    void
      get_dynamic_image_from_parametric_image(DynamicDiscretisedDensity & dyn_image,
					      const ParametricVoxelsOnCartesianGrid & par_image);

    //! This is the common method used to estimate the parametric images from the dynamic images. 
    void 
      apply_linear_regression(ParametricVoxelsOnCartesianGrid & par_image, const DynamicDiscretisedDensity & dyn_image);

    void set_defaults();

    Succeeded set_up(); // Could be in the post_processing() as well.

  bool _if_cardiac;   //!< Switches between cardiac and brain data
  unsigned int _starting_frame;   //!< Starting frame to apply the model
  float _cal_factor;   //!< Calibration Factor, maybe to be removed.
  float _time_shift;   //!< Shifts the time to fit the timing of Plasma Data with the Projection Data.
  bool _in_correct_scale; //!< Switch to scale or not the model_matrix to the correct scale, according to the appropriate scale factor.
  bool _in_total_cnt;   //!< Switch to choose the values of the model to be in total counts or in mean counts.
  string _blood_data_filename;   //!< Name of file in which the input function is stored
  BloodFrameData _blood_frame_data;   //!< Stores the blood frame data for cardiac studies
  PlasmaData _plasma_frame_data;    //!< Stores the plasma data into frames for brain studies
  string _time_frame_definition_filename;   //!< name of file to get frame definitions
  TimeFrameDefinitions _frame_defs;   //!< TimeFrameDefinitions
#if 0
  //! name of file to use as dynamic image
  string _dyn_image_filename;
  //! name of file to use as parametric image 
  string _par_image_filename;
  //! dynamic image pointer
  DynamicDiscretisedDensity _dyn_image_sptr;
  //! parametric image pointer
  ParametricVoxelsOnCartesianGrid _par_image_sptr;  
#endif

 private:
  void create_model_matrix();  //!< Creates model matrix from private members
  void initialise_keymap();
  bool post_processing();
  ModelMatrix<2> _model_matrix;
  bool _matrix_is_stored;
  typedef RegisteredParsingObject<PatlakPlot,KineticModel> base_type;
};

END_NAMESPACE_STIR

#endif //__stir_modelling_PatlakPlot_H__
