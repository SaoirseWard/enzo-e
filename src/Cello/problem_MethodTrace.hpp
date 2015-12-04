// See LICENSE_CELLO file for license and copyright information

/// @file     problem_MethodTrace.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     2015-11-06 22:31:28
/// @brief    [\ref Problem] Declaration for the MethodTrace class

#ifndef PROBLEM_METHOD_TRACE_HPP
#define PROBLEM_METHOD_TRACE_HPP

class Refresh;
class Schedule;

class MethodTrace : public Method
{
  /// @class    MethodTrace
  /// @ingroup  MethodTrace
  /// @brief    [\ref MethodTrace] Test method for basic Particle methods

public: // interface

  /// Create a new MethodTrace
  MethodTrace (const FieldDescr * field_descr,
	       const ParticleDescr * particle_descr,
	       double courant) throw() ;

  /// Destructor
  virtual ~MethodTrace() throw()
  {};

  /// Charm++ PUP::able declarations
  PUPable_decl(MethodTrace);

  /// Charm++ PUP::able migration constructor
  MethodTrace (CkMigrateMessage *m) {}

  /// CHARM++ Pack / Unpack function
  void pup (PUP::er &p)
  {};

public: // virtual functions

  /// Apply the method to advance a block one timestep 

  virtual void compute ( Block * block) throw();

  /// Return the name of this MethodTrace
  virtual std::string name () throw ()
  { return "trace"; }

protected: // functions


protected: // attributes


};

#endif /* PROBLEM_METHOD_TRACE_HPP */