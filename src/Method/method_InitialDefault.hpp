// $Id: method_InitialDefault.hpp 1877 2010-11-30 01:20:27Z bordner $
// See LICENSE_CELLO file for license and copyright information

#ifndef METHOD_INITIAL_DEFAULT_HPP
#define METHOD_INITIAL_DEFAULT_HPP

/// @file     method_InitialDefault.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Tue Jan  4 19:26:38 PST 2011
/// @brief    [\ref Method] Default initialization method

class InitialDefault : public Initial {

  /// @class    InitialDefault
  /// @ingroup  Method
  /// @brief    [\ref Method] Default initialization method

public: // interface

  /// Constructor
  InitialDefault(Parameters * parameters) throw();

  /// Read initialization from Initial group in parameter file

  virtual void compute (DataBlock * data_block) throw();

private: // functions
  
  void allocate_xyzt_(DataBlock * data_block,
		      int index_field,
		      int * mx, int * my, int * mz,
		      double ** value, double ** vdeflt,
		      bool ** region, bool ** rdeflt,
		      double ** x, double ** y, double ** z,
		      double ** t) throw();

  void copy_values_ (
		     FieldBlock * field_block,
		     double * value, bool * region,
		     int index_field,
		     int nx, int ny, int nz) throw();

  void evaluate_scalar_ (FieldBlock * field_block, int index_field, 
			 std::string field_name,
			 int n, double * value, double * vdeflt,
			 double * x, double * y, double * z, double * t) throw();

  void evaluate_logical_ (FieldBlock * field_block, int index_field, 
			 std::string field_name,
			 int n, bool * value, bool * vdeflt,
			 double * x, double * y, double * z, double * t) throw();


private: // attributes

  Parameters * parameters_;

};

#endif /* METHOD_METHOD_INITIAL_DEFAULT_HPP */
