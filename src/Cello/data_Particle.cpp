// See LICENSE_CELLO file for license and copyright information

/// @file     data_Particle.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2016-03-31
/// @brief    

#include "data.hpp"
#include "charm_simulation.hpp"

//----------------------------------------------------------------------

int Particle::insert_particles (int it, int np)
  
{
  if (proxy_simulation.ckLocalBranch()) {
    proxy_simulation.ckLocalBranch()->monitor_insert_particles(np);
  }

  return particle_data_->insert_particles (particle_descr_, it, np); 
}

//----------------------------------------------------------------------

void Particle::delete_particles (int it, int ib, const bool * m)
{
  // count number of particles npd to delete

  const int np = num_particles(it,ib);
  int npd = 0;
  if (m == NULL) {
    npd = np;
  } else {
    for (int ip=0; ip<np; ip++) npd += m[ip]?1:0;
  }

  if (proxy_simulation.ckLocalBranch()) {
    proxy_simulation.ckLocalBranch()->monitor_insert_particles(-npd);
  }

  particle_data_->delete_particles (particle_descr_,it,ib,m); 
}

//----------------------------------------------------------------------

Particle::~Particle() throw()
{
}