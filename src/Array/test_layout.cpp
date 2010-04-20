// $Id: test_block.cpp 1369 2010-04-08 01:38:06Z bordner $
// See LICENSE_CELLO file for license and copyright information

/// @file     test_layout.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2010-04-19
/// @brief    Unit tests for the Layout class

#include "cello.h"

#include "error.hpp"
#include "test.hpp"
#include "parallel.hpp"
#include "array.hpp"

int main(int argc, char ** argv)
{

  Parallel * parallel = Parallel::instance();
  
  parallel->initialize(&argc, &argv);

  int np = parallel->process_count();
  int ip = parallel->process_rank();

  unit_class ("Layout");

  // Problem dimension dim = 3
  int dim = 3;            // dimensionality

  // Array size n3[]

  int n3[] = {15,7,24};

  //----------------------------------------------------------------------
  // TEST 1:  One (P=1,T=1) [Serial]
  //----------------------------------------------------------------------

  unit_func("Layout");
  Layout * layout_serial = new Layout (dim);
  unit_assert (true);

  unit_func("set_array");
  layout_serial -> set_array (dim, n3);

  // test array size

  int n3_serial[3];
  unit_func("array_size");
  layout_serial->array_size(3,n3_serial);
  unit_assert (n3[0]==n3_serial[0] &&
	       n3[1]==n3_serial[1] &&
	       n3[2]==n3_serial[2]);
	       
  // test (P,T) ranges

  // (default: 1 process starting at 0)
  unit_func("set_processes");
  layout_serial->set_processes(0,5);

  unit_func("process_first");
  unit_assert (layout_serial -> process_first() == 0);
  unit_func("process_count");
  unit_assert (layout_serial -> process_count() == 1);

  // (default: 1 thread starting at 0)
  unit_func("set_threads");
  layout_serial->set_threads(0,1);   

  unit_func("thread_first");
  unit_assert (layout_serial -> thread_first() == 0);
  unit_func("thread_count");
  unit_assert (layout_serial -> thread_count() == 1);

  // test process block size and count

  int np3[3] = {4,2,3};
  layout_serial -> set_process_blocks (3,np3);
  unit_func("process_block_count");
  unit_assert (layout_serial -> process_block_count() == 1);
  //  unit_assert (layout_serial -> thread_block_count() == 1);

  delete layout_serial;

  //----------------------------------------------------------------------
  // TEST 2:  One (P=8,T=1) [MPI parallel]
  //----------------------------------------------------------------------

  //----------------------------------------------------------------------
  // TEST 3:  One (P=4,T=2) [MPI parallel, OpenMP threading]
  //----------------------------------------------------------------------

  //----------------------------------------------------------------------
  // TEST 4:  Two (P=2,T=2) [Multiple layouts]
  //----------------------------------------------------------------------

  unit_assert (false);
  
  parallel->finalize();

}