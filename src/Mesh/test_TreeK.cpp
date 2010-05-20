// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file     test_TreeK.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2009-10-28
/// @brief    Test program for Tree2K and Tree3K classes

#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "cello.h"

#include "error.hpp"
#include "monitor.hpp"
#include "memory.hpp"
//#include "disk.hpp"
#include "mesh_Node2K.hpp"
#include "mesh_Node3K.hpp"
#include "mesh_Tree2K.hpp"
#include "mesh_Tree3K.hpp"

#include "image.h"

#define index(ix,iy,iz,n) ((ix) + (n)*((iy) + (n)*(iz)))

const bool debug    = false;
const bool geomview = false;

const int  cell_size = 1;
const int  line_width = 1;
const int  gray_threshold = 127;
const int sphere_size = 128;


//----------------------------------------------------------------------

int * create_level_array (int * n0, int * ny, int max_levels);
int * create_level_array3 (int * n3, int max_levels);
int * create_sphere (int n3, int max_levels);
void write_image(std::string filename, float * image, int nx, int ny, int nz=0);

void create_tree ( int * level_array, int nx, int ny, int nz, int k,  int d, 
		   std::string name, int max_level);
void print_usage(int, char**);
//----------------------------------------------------------------------

int main(int argc, char ** argv)
{

  // Required for Monitor

  Parallel * parallel = Parallel::instance();
  parallel->initialize(&argc,&argv);

  // Parse command line

  if (argc != 4) {
    print_usage(argc,argv);
  }

  // Check arguments

  int dimension  = atoi(argv[1]);
  int refinement = atoi(argv[2]);
  int max_level  = atoi(argv[3]);

  if (dimension != 2 && 
      dimension != 3) print_usage(argc,argv);
  
  if (refinement != 2 && 
      refinement != 4 &&
      refinement != 8 &&
      refinement != 16) print_usage(argc,argv);
  
  if (! (0 < max_level && max_level <= 12)) print_usage(argc,argv);

  char filename[80];
  sprintf (filename,"TreeK-D=%d-R=%d-L=%d",dimension,refinement,max_level);

  int nx,ny,nz;
  int * level_array;

  if (dimension == 2) {

    nx = 1;
    ny = 1;
    nz = 1;
    level_array = create_level_array(&nx,&ny,max_level);

  } else {

    nx = sphere_size;
    ny = sphere_size;
    nz = sphere_size;
    level_array = create_sphere(sphere_size,max_level);
  }

  create_tree (level_array, nx, ny, nz, refinement, dimension, filename,max_level);

  delete [] level_array;

  Memory::instance()->print();

  parallel->finalize();

}

//----------------------------------------------------------------------
void print_usage(int argc, char **argv)
{
  Parallel * parallel = Parallel::instance();

  fprintf (stderr,"\n");
  fprintf (stderr,"Usage: %s <dimension> <refinement> <levels>\n",argv[0]);
  fprintf (stderr,"\n");
  fprintf (stderr,"   where \n");
  fprintf (stderr,"\n");
  fprintf (stderr,"         <dimension>  = [2|3]\n");
  fprintf (stderr,"         <refinement> = [2|4|8|16]\n");
  fprintf (stderr,"\n");
  exit(1);
}
//----------------------------------------------------------------------

// read in the gimp-generated image data into a level array
// values are set to [0:max_levels)

int * create_level_array (int * nx, int * ny, int max_levels)
{

  int size = (width > height) ? width: height;

  int * level_array = new int [size*size];

  for (int i=0; i<size*size; i++) level_array[i] = 0;

  *nx = size;
  *ny = size;

  int pixel[3];
  char * data = header_data;

  int ix0 = (size - width) / 2;
  int iy0 = (size - height) / 2;

  int max = 0;
  for (size_t iy=0; iy<height; iy++) {
    for (size_t ix=0; ix<width; ix++) {
      HEADER_PIXEL(data,pixel);
      int i = (iy+iy0) + size*(ix+ix0);
      float r = 1.0*pixel[0]/256;
      float g = 1.0*pixel[1]/256;
      float b = 1.0*pixel[2]/256;
      level_array[i] = max_levels * (r + g + b) / 3;
      if (level_array[i] > max) max = level_array[i];
    }
  }
  return level_array;
}

//----------------------------------------------------------------------
// read in the gimp-generated image data into a level array
// values are set to [0:max_levels)

int * create_level_array3 (int * n3, int max_levels)
{

  if (width != height) {
    printf ("%s:%d width = %d  height = %d\n",__FILE__,__LINE__,width, height);
    exit(1);
  }

  *n3 = width;
  int n = *n3;

  int * level_array = new int [n*n*n];

  for (int i=0; i<n*n*n; i++) level_array[i] = 0;

  int pixel[3];
  char * data = header_data;
 
  float r = 0.125;  // width of the 2D image in the 3D cube
  int nxm = n*(1.0-r)/2;
  int nxp = n*(1.0+r)/2;

  for (int iz=0; iz<n; iz++) {
    for (int iy=0; iy<n; iy++) {
      HEADER_PIXEL(data,pixel);
      for (int ix=0; ix<nxm; ix++) {
	level_array[index(iz,iy,ix,n)] = 0;
      }
      for (int ix=nxm; ix<nxp; ix++) {
	float r = 1.0*pixel[0]/256;
	float g = 1.0*pixel[1]/256;
	float b = 1.0*pixel[2]/256;
	int value = max_levels * (r + g + b) / 3;
	level_array[index(iz,iy,ix,n)] = value;
      }
      for (int ix=nxp; ix<n; ix++) {
	level_array[index(iz,iy,ix,n)] = 0;
      }
    }
  }
  return level_array;
}

//----------------------------------------------------------------------

int * create_sphere (int n3, int max_levels)
{

  int * level_array = new int [n3*n3*n3];

  for (int i=0; i<n3*n3*n3; i++) level_array[i] = 0;

  const double R = 0.3;  // radius
  double R2 = R*R;
  
  double x,y,z;

  for (int iz=0; iz<n3/2; iz++) {
    z = double(iz) / n3 - 0.5;
    for (int iy=0; iy<n3/2; iy++) {
      y = double(iy) / n3 - 0.5;
      for (int ix=0; ix<n3/2; ix++) {
	x = double(ix) / n3 - 0.5;
	double r2 = x*x + y*y + z*z;
	double v = r2 < R2 ? max_levels : 0;

	level_array[index(     ix,     iy,     iz,n3)] = v;
	level_array[index(n3-ix-1,     iy,     iz,n3)] = v;
	level_array[index(     ix,n3-iy-1,     iz,n3)] = v;
	level_array[index(n3-ix-1,n3-iy-1,     iz,n3)] = v;
	level_array[index(     ix,     iy,n3-iz-1,n3)] = v;
	level_array[index(n3-ix-1,     iy,n3-iz-1,n3)] = v;
	level_array[index(     ix,n3-iy-1,n3-iz-1,n3)] = v;
	level_array[index(n3-ix-1,n3-iy-1,n3-iz-1,n3)] = v;
      }
    }
  }
  return level_array;
}

//----------------------------------------------------------------------

void write_image(std::string filename, float * image, int nx, int ny, int nz)
{
  if (nx > 8192 || ny > 8192 || nz > 8192) {
    printf ("%s:%d (nx,ny,nz) = (%d,%d,%d)\n",__FILE__,__LINE__,nx,ny,nz);
    exit(1);
  }
  // Write HDF5 file

  // Hdf5 hdf5;
  // hdf5.file_open((filename+".hdf5").c_str(),"w");
  // printf ("write_image %d %d %d\n",nx,ny,nz);
  // hdf5.dataset_open_write ("tree_image",nx,ny,nz);
  // hdf5.write(image);
  // hdf5.dataset_close ();
  // hdf5.file_close();

  // Write PNG image

  Monitor * monitor = Monitor::instance();
  float min=image[0];
  float max=image[0];
  for (int i=0; i<nx*ny*nz; i++) {
    if (min > image[i]) min = image[i];
    if (max < image[i]) max = image[i];
  }
  double color_map[] = {0,0,0,1,1,1};
  monitor->image ((filename+".png").c_str(),image,nx,ny,1,0,0,0,nx,ny,1,2,reduce_sum,
		  min,max,color_map, 2);

}

//----------------------------------------------------------------------

void create_tree 
(
 int * level_array, 
 int nx, int ny, int nz,
 int k,  int d, 
 std::string name,
 int max_level
 )
{

  TreeK * tree = 0;
  if (d==2) tree = new Tree2K(k);
  if (d==3) tree = new Tree3K(k);

  float mem_per_node;

  Memory * memory = Memory::instance();

  memory->reset();

  printf ("--------------------------------------------------\n");
  printf ("k=%d d=%d\n",k,d);
  printf ("--------------------------------------------------\n");

  int full_nodes = true;

  //--------------------------------------------------
  // Refine the tree
  //--------------------------------------------------

  printf ("\nINITIAL TREE\n");

  memory->set_active(true);
  tree->refine(level_array,nx,ny,nz,max_level,full_nodes);
  memory->print();
  memory->set_active(false);


  if (geomview) tree->geomview(name + "-0.gv");

  mem_per_node = (float) memory->bytes(0) / tree->num_nodes();
  printf ("nodes      = %d\n",tree->num_nodes());
  printf ("levels     = %d\n",tree->levels());
  printf ("bytes/node = %g\n",mem_per_node);

  //--------------------------------------------------
  // Balance the tree
  //--------------------------------------------------

  // Determine image size
  float * image;
  int image_size = cell_size + 2*line_width;
  for (int i=0; i<tree->levels(); i++) {
    image_size = 2*image_size - line_width;
  }

  printf ("\nBALANCED TREE\n");

  memory->set_active(true);
  tree->balance(full_nodes);
  memory->print();
  memory->set_active(false);

  if (d==2) {
    image = tree->create_image(image_size,line_width);
    write_image(name + "-0",image,image_size,image_size,1);
    delete [] image;
  } else {
    image = tree->create_image(image_size,line_width,0);
    write_image(name + "-x" + "-0",image,image_size,image_size,1);
    delete [] image;
    image = tree->create_image(image_size,line_width,1);
    write_image(name + "-y" + "-0",image,image_size,image_size,1);
    delete [] image;
    image = tree->create_image(image_size,line_width,2);
    write_image(name + "-z" + "-0",image,image_size,image_size,1);
    delete [] image;
  }

  if (geomview) tree->geomview(name + "-0" + "-1.gv");

  mem_per_node = (float) memory->bytes(0) / tree->num_nodes();
  printf ("nodes      = %d\n",tree->num_nodes());
  printf ("levels     = %d\n",tree->levels());
  printf ("bytes/node = %g\n",mem_per_node);

  //--------------------------------------------------
  // Coalesce Patches In the tree
  //--------------------------------------------------

  printf ("\nCOALESCED TREE\n");

  memory->set_active(true);
  tree->optimize();
  memory->print();
  memory->set_active(false);

  if (d==2) {
    image = tree->create_image(image_size,line_width);
    write_image(name + "-1",image,image_size,image_size,1);
    delete [] image;
  } else {
    image = tree->create_image(image_size,line_width,0);
    write_image(name + "-x" + "-1",image,image_size,image_size,1);
    delete [] image;
    image = tree->create_image(image_size,line_width,1);
    write_image(name + "-y" + "-1",image,image_size,image_size,1);
    delete [] image;
    image = tree->create_image(image_size,line_width,2);
    write_image(name + "-z" + "-1",image,image_size,image_size,1);
    delete [] image;
  }

  if (geomview) tree->geomview(name + "1" + "-2.gv");

  printf ("nodes      = %d\n",tree->num_nodes());
  printf ("levels     = %d\n",tree->levels());
  printf ("bytes/node = %g\n",mem_per_node);

  delete tree;

}