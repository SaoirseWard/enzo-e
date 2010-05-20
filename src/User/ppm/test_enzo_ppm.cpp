// $Id: test_enzo_method_ppm.cpp 1262 2010-03-03 15:44:05Z bordner $
// See LICENSE_ENZO file for license and copyright information

/// @file     test_enzo_method_ppm.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Thu Apr  1 16:19:18 PDT 2010
/// @brief    Unit tests for the EnzoMethodPpm class

#include <stdio.h>

#include "parallel.hpp"
#include "user.hpp"
#include "test.hpp"

int main (int argc, char ** argv)
{

  // Initialize parallelism

  Parallel * parallel = Parallel::instance();
  parallel->initialize(&argc,&argv);

  unit_init(parallel->process_rank(), parallel->process_count());

  unit_class ("MethodEnzoPpm");
  MethodEnzoPpm ppm;

  unit_func("initialize");
  ppm.initialize();
  unit_assert(true);

  unit_func("advance_block");
  ppm.advance_block();
  unit_assert(false);

  unit_func("refresh_face");
  ppm.refresh_face();
  unit_assert(false);

  unit_finalize();

}