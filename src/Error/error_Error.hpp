// $Id: error_error.hpp 1258 2010-03-02 01:07:36Z bordner $
// See LICENSE_CELLO file for license and copyright information

#ifndef ERROR_ERROR_HPP
#define ERROR_ERROR_HPP

/// @file     error_Error.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @todo     Add Parallel support
/// @bug      exit() is called instead of MPI_Abort(), etc.
/// @date     Thu Feb 25 16:20:17 PST 2010
/// @brief    Declaration of the Error class

/// @def      ERROR_MESSAGE_LENGTH
/// @brief    Maximum length of error and warning messages
#define ERROR_MESSAGE_LENGTH 255

/// @def      WARNING_MESSAGE
/// @brief    Handle a (non-lethal) warning message
#define WARNING_MESSAGE(FUNCTION,MESSAGE) \
  Error::instance()->warning_(__FILE__,__LINE__,FUNCTION,MESSAGE)

/// @def      ERROR_MESSAGE
/// @brief    Handle a (lethal) error message
#define ERROR_MESSAGE(FUNCTION,MESSAGE) \
  Error::instance()->error_(__FILE__,__LINE__,FUNCTION,MESSAGE)

/// @def      INCOMPLETE_MESSAGE
/// @brief    Placeholder for code that is incomplete
#define INCOMPLETE_MESSAGE(FUNCTION,MESSAGE) \
  Error::instance()->incomplete_(__FILE__,__LINE__,FUNCTION,MESSAGE)

/// @def      TRACE
/// @brief    Trace file name and location to stdout
#define TRACE_MESSAGE					\
  Error::instance()->trace_(__FILE__,__LINE__)

/// @def      ASSERT
/// @brief    Equivalent to assert()
#define ASSERT(FUNCTION,MESSAGE,ASSERTION) \
  Error::instance()->assert_(__FILE__,__LINE__,FUNCTION,MESSAGE,ASSERTION)

//----------------------------------------------------------------------

class Error {

  /// @class    Error
  /// @ingroup  Error
  /// @brief    Singleton class for reporting errors and warnings

public: // functions

  /// Get single instance of the Error object
  static Error * instance() throw ()
  { return & instance_; };

  /// Set whether to trace on this processor
  void set_traces_active (bool traces_active) 
  { traces_active_ = traces_active; };

  /// Return whether to trace on this processor
  bool traces_active ()
  { return traces_active_; };

  /// Set whether to trace on this processor
  void set_warnings_active (bool warnings_active) 
  { warnings_active_ = warnings_active; };

  /// Return whether to trace on this processor
  bool warnings_active ()
  { return warnings_active_; };

  /// Set whether to trace on this processor
  void set_errors_active (bool errors_active) 
  { errors_active_ = errors_active; };

  /// Return whether to trace on this processor
  bool errors_active ()
  { return errors_active_; };

  /// Set whether to trace on this processor
  void set_incompletes_active (bool incompletes_active) 
  { incompletes_active_ = incompletes_active; };

  /// Return whether to trace on this processor
  bool incompletes_active ()
  { return incompletes_active_; };

public: // functions

  //----------------------------------------------------------------------
  /// Warning message
  void warning_ (const char * file,
		 int          line,
		 const char * function,
		 const char * message)
  {
    if (warnings_active_) {
      message_(stdout,"WARNING",file,line,function,message);
    }
  };

  //----------------------------------------------------------------------
  /// Incomplete message
  void incomplete_ (const char * file,
		    int          line,
		    const char * function,
		    const char * message)
  {
    if (incompletes_active_) {
      message_(stdout,"INCOMPLETE",file,line,function,message);
    }
  };

  //----------------------------------------------------------------------
  /// Error message
  void error_ (const char * file,
	       int          line,
	       const char * function,
	       const char * message)
  {
    if (errors_active_) {
      message_(stderr,"ERROR",file,line,function,message);
      exit(1);
    }
  };

  //----------------------------------------------------------------------
  void trace_ (const char * file,
	       int          line)
  {
    if (traces_active_) {
      printf ("TRACE %s:%d\n",file,line); 
      fflush(stdout);
    }
  };

  //----------------------------------------------------------------------
  void assert_ (const char * file,
		int          line,
		const char * function,
		const char * message,
		bool         assertion)
  {
    if (!assertion) {
      printf(file,line,"ASSERT",file,line,function,message);
      exit(1);
    }
  };

private: // functions

  //----------------------------------------------------------------------
  /// Initialize the Error object (singleton design pattern)
  Error() 
    : errors_active_(true),
      incompletes_active_(true),
      traces_active_(true),
      warnings_active_(true)
      
  {};

private: // functions

  void message_  
  (
   FILE *       fp,
   const char * type,
   const char * file,
   int          line,
   const char * function,
   const char * message)
  {
    fprintf (fp,"\n");
    fprintf (fp,"     %s File:     %s:%d\n",type,file,line);
    fprintf (fp,"     %s Function: %s()\n", type,function);
    fprintf (fp,"     %s Message:  %s\n",   type,message);
    fprintf (fp,"\n");
  };

private: // attributes

  /// Single instance of the Error object (singleton design pattern)
  static Error instance_;

  /// Whether to display error messages
  bool errors_active_;

  /// Whether to display "incomplete" messages
  bool incompletes_active_;

  /// Whether tracing is on or off
  bool traces_active_;

  /// Whether to display warning messages
  bool warnings_active_;

};


#endif /* ERROR_ERROR_HPP */
