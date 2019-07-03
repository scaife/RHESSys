/*--------------------------------------------------------------*/
/* 								*/
/*		update_gw_drainage					*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_gw_drainage -  					*/
/* 		drainage shallow subsurface saturation zone	*/
/*		from each patch to a regional (hillslope scale)	*/
/*		groundwater store				*/
/*		nitrogen is also drained using assumption 	*/
/*		of an exponential decay of N with depth		*/
/*								*/
/*	SYNOPSIS						*/
/*	int update_gw_drainage(					*/
/*			struct patch_object *			*/
/*			struct hillslope_object *		*/
/*			struct command_line_object *		*/
/*			struct date,				*/
/*			)					*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*	preset code just uses a user assigned loading rate	*/
/*	and all of it is nitrate				*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int update_gw_drainage(
				  struct  patch_object   *patch,
				  struct  hillslope_object *hillslope,
				  struct  zone_object *zone,
				  struct  command_line_object *command_line,
				  struct	date	current_date)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.			*/
	/*------------------------------------------------------*/
	
	double  compute_z_final(
		int,
		double,
		double,
		double,
		double,
		double);


	/*------------------------------------------------------*/
	/*	Local Variable Definition. 			*/
	/*------------------------------------------------------*/
	int ok = 1;
	double drainage,sat_store,N_loss;
	double preday_sat_deficit_z, add_field_capacity;
	double sat_to_gw_coeff;
	/*------------------------------------------------------*/
	/*		assume percent of incoming precip	*/
	/*------------------------------------------------------*/
	if (zone[0].hourly_rain_flag==1){
	  sat_to_gw_coeff = patch[0].soil_defaults[0][0].sat_to_gw_coeff / 24;
	}
	else sat_to_gw_coeff = patch[0].soil_defaults[0][0].sat_to_gw_coeff; 

	/*------------------------------------------------------*/
	/* multiply by Ksat vertical which is surface Ksat 	*/
	/* so that impervious areas don't infiltrate to deep gw */
	/*------------------------------------------------------*/
	sat_to_gw_coeff = sat_to_gw_coeff * patch[0].Ksat_vertical;

	drainage = sat_to_gw_coeff * patch[0].detention_store;
	patch[0].detention_store -= drainage;
	patch[0].gw_drainage = drainage;
	hillslope[0].gw.storage += (drainage * patch[0].area / hillslope[0].area);

	/*------------------------------------------------------*/
	/*	determine associated N leached			*/
	/*------------------------------------------------------*/
	if (patch[0].surface_DON > ZERO) {
		N_loss = sat_to_gw_coeff * patch[0].surface_DON;
		hillslope[0].gw.DON += (N_loss * patch[0].area / hillslope[0].area);
		patch[0].ndf.DON_to_gw += N_loss;
		patch[0].surface_DON -= N_loss;
		}
	if (patch[0].surface_DOC > ZERO) {
		N_loss = sat_to_gw_coeff * patch[0].surface_DOC;
		hillslope[0].gw.DOC += (N_loss * patch[0].area / hillslope[0].area);
		patch[0].cdf.DOC_to_gw += N_loss;
		patch[0].surface_DOC -= N_loss;
		}
	
	
	if (patch[0].surface_NH4 > ZERO) {
		N_loss = sat_to_gw_coeff * patch[0].surface_NH4;
		hillslope[0].gw.NH4 += (N_loss * patch[0].area / hillslope[0].area);
		patch[0].ndf.N_to_gw += N_loss;
		patch[0].surface_NH4 -= N_loss;
		}
	
	if (patch[0].surface_NO3 > ZERO) {
		N_loss = sat_to_gw_coeff * patch[0].surface_NO3;
		hillslope[0].gw.NO3 += (N_loss * patch[0].area / hillslope[0].area);
		patch[0].ndf.N_to_gw += N_loss;
		patch[0].surface_NO3 -= N_loss;
		}

	if (patch[0].soil_ns.nitrate > ZERO) {
		N_loss = sat_to_gw_coeff * patch[0].soil_ns.nitrate; 
		hillslope[0].gw.NO3 += (N_loss * patch[0].area / hillslope[0].area);
		patch[0].ndf.N_to_gw += N_loss;
		patch[0].soil_ns.nitrate -= N_loss;
		}


	return (!ok);
} /* end update_gw_drainage.c */
