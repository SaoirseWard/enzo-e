// See LICENSE_ENZO file for license and copyright information

/***********************************************************************
/
/  GRID CLASS (COMPUTE THE PRESSURE FIELD AT THE GIVEN TIME)
/
/  written by: Greg Bryan
/  date:       November, 1994
/  modified1:
/
/  PURPOSE:
/
/  RETURNS:
/
************************************************************************/
 
// Compute the pressure at the requested time.  The pressure here is
//   just the ideal-gas equation-of-state.

#include "cello.hpp"

#include "enzo.hpp"
 
//----------------------------------------------------------------------
 
int EnzoBlock::ComputePressure(enzo_float time, enzo_float *pressure)
{

  /* declarations */
 
  int i, size = 1;
 
  /* Error Check */

  TRACE3 ("time: %20.14g %20.14g %20.14g\n",OldTime,time,Time());
  if (time < OldTime || time > Time()) {
    fprintf(stderr, "requested time is outside available range.\n");
    return ENZO_FAIL;
  }
 
  /* Compute interpolation coefficients. */
 
  enzo_float coef, coefold;
  if (Time() - OldTime > 0)
    coef    = (time - OldTime)/(Time() - OldTime);
  else
    coef    = 1;
 
  coefold = 1 - coef;
 
  /* Compute the size of the grid. */
 
  for (int dim = 0; dim < GridRank; dim++)
    size *= GridDimension[dim];
 
  /* Find fields: density, total energy, velocity1-3. */
 
  Field field = block()->field();

  
  enzo_float * density      = (enzo_float *) field.values("density");
  enzo_float * total_energy = (enzo_float *) field.values("total_energy");
  enzo_float * energy       = (enzo_float *) field.values("internal_energy");
  enzo_float * velocity_x   = (enzo_float *) field.values("velocity_x");
  enzo_float * velocity_y   = (enzo_float *) field.values("velocity_y");
  enzo_float * velocity_z   = (enzo_float *) field.values("velocity_z");

  /* Loop over the grid, compute the thermal energy, then the pressure,
     the timestep and finally the implied timestep. */
 
  /* special loop for no interpolate. */

  // WARNING: floating point comparison
  if (time == Time())

    for (i = 0; i < size; i++) {
 
      enzo_float te = total_energy[i];
      enzo_float d  = density[i];
      enzo_float vx = velocity_x[i];
      enzo_float vy = (rank() >= 2) ? velocity_y[i] : 0.0;
      enzo_float vz = (rank() >= 3) ? velocity_z[i] : 0.0;

      /* gas energy = E - 1/2 v^2. */
 
      energy[i] = te - 0.5*(vx*vx + vy*vy + vz*vz);

      pressure[i] = (Gamma - 1.0)*d*energy[i];

      if (pressure[i] < pressure_floor)
	pressure[i] = pressure_floor;

    } // end of loop
 
  else

    ERROR("EnzoBlock::ComputePressure()",
	    "Accessing OldBaryonField");
	    
    /* general case: */
 
    // for (i = 0; i < size; i++) {
 
    //   enzo_float te = coef * te  BaryonField[TENum][i] +
    // 	              coefold*OldBaryonField[TENum][i];
    //   density       = coef   *   BaryonField[DensNum][i] +
    //                   coefold*OldBaryonField[DensNum][i];
    //   vx     = coef   *   BaryonField[Vel1Num][i] +
    //                   coefold*OldBaryonField[Vel1Num][i];
 
    //   if (GridRank > 1)
    // 	vy   = coef   *   BaryonField[Vel2Num][i] +
    // 	              coefold*OldBaryonField[Vel2Num][i];
    //   if (GridRank > 2)
    // 	vz   = coef   *   BaryonField[Vel3Num][i] +
    // 	              coefold*OldBaryonField[Vel3Num][i];
 
    //   /* gas energy = E - 1/2 v^2. */
 
    //   gas_energy    = total_energy - 0.5*(vx*vx +
    // 					  vy*vy +
    // 					  vz*vz);
 
    //   pressure[i] = (Gamma - 1.0)*density*gas_energy;
 
    //   if (pressure[i] < pressure_floor)
    // 	pressure[i] = pressure_floor;
 
    // }
 
  /* Correct for Gamma from H2. */
 
  if (MultiSpecies > 1) {
 
    enzo_float TemperatureUnits = 1, number_density, nH2, GammaH2Inverse,
      GammaInverse = 1.0/(Gamma-1.0), x, Gamma1, temp;
    enzo_float DensityUnits, LengthUnits, VelocityUnits, TimeUnits;
 
    /* Find Multi-species fields. */
 
    int DeNum, HINum, HIINum, HeINum, HeIINum, HeIIINum, HMNum, H2INum,
        H2IINum, DINum, DIINum, HDINum;
    if (IdentifySpeciesFields(DeNum, HINum, HIINum, HeINum, HeIINum, HeIIINum,
		      HMNum, H2INum, H2IINum, DINum, DIINum, HDINum) == ENZO_FAIL) {
      fprintf(stderr, "Error in grid->IdentifySpeciesFields.\n");
      return ENZO_FAIL;
    }
 
    /* Find the temperature units if we are using comoving coordinates. */
 
    if (ComovingCoordinates)
      if (CosmologyGetUnits(&DensityUnits, &LengthUnits, &TemperatureUnits,
			    &TimeUnits, &VelocityUnits, Time()) == ENZO_FAIL) {
	fprintf(stderr, "Error in CosmologyGetUnits.\n");
	return ENZO_FAIL;
      }
 
    for (i = 0; i < size; i++) {
 
      number_density =
	0.25*(BaryonField[HeINum][i] + 
	      BaryonField[HeIINum][i] +
	      BaryonField[HeIIINum][i]) +
	BaryonField[HINum][i] + 
	BaryonField[HIINum][i] +
	BaryonField[DeNum][i];
 
      nH2 = 0.5*(BaryonField[H2INum][i] + BaryonField[H2IINum][i]);
 
      /* First, approximate temperature. */
 
      if (number_density == 0)
	number_density = number_density_floor;
      temp = MAX(TemperatureUnits*pressure[i]/(number_density + nH2), 
		 (enzo_float)(1.0));
 
      /* Only do full computation if there is a reasonable amount of H2.
	 The second term in GammaH2Inverse accounts for the vibrational
	 degrees of freedom. */
 
      GammaH2Inverse = 0.5*5.0;
      if (nH2/number_density > 1e-3) {
	x = temp/6100.0;
	if (x < 10.0)
	  GammaH2Inverse = 0.5*(5 + 2.0 * x*x * exp(x)/pow(exp(x)-1.0,2));
      }
 
      Gamma1 = 1.0 + (nH2 + number_density) /
	             (nH2*GammaH2Inverse + number_density * GammaInverse);
	
      /* Correct pressure with improved Gamma. */
 
      pressure[i] *= (Gamma1 - 1.0)/(Gamma - 1.0);
 
    } // end: loop over i
 
  } // end: if (MultiSpecies > 1)
 
  /* To emulate the opacity limit in turbulent star formation 
     simulations */
  
  enzo_float Gamma1 = Gamma;
  if ((ProblemType == 60 || ProblemType == 61) && GravityOn == TRUE)
    for (i=0; i<size; i++) {
      Gamma1 = MIN(Gamma + (log10(density[i])-8.0)*0.3999/2.5, 1.4);
      pressure[i] *= (Gamma1 - 1.0)/(Gamma - 1.0);
    }


  return ENZO_SUCCESS;
}
