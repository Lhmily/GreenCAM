/*------------------------------------------------------------
 * UC Santa Barbara ArchLab
 * Ternary CAM (TCAM) Power and Delay Model - Release 2.0
 *
 * Copyright (c) 2006 The Regents of the University of California.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research and non-profit purposes,
 * without fee, and without a written agreement is hereby granted,
 * provided that the above copyright notice, this paragraph and the
 * following three paragraphs appear in all copies.
 *
 * Permission to incorporate this software into commercial products may
 * be obtained by contacting the University of California. For
 * information about obtaining such a license contact:
 * Banit Agrawal <banit@cs.ucsb.edu>
 * Tim Sherwood  <sherwood@cs.ucsb.edu>
 *
 * This software program and documentation are copyrighted by The Regents
 * of the University of California. The software program and
 * documentation are supplied "as is", without any accompanying services
 * from The Regents. The Regents does not warrant that the operation of
 * the program will be uninterrupted or error-free. The end-user
 * understands that the program was developed for research purposes and
 * is advised not to rely exclusively on the program for any reason.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND
 * ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. THE UNIVERSITY OF
 * CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *------------------------------------------------------------*/

#ifndef _ITRS_INTERCONNECT_H_
#define _ITRS_INTERCONNECT_H_

/*
   Generic implementation:
     Capacitance and resistance of CNT and Cu wires

*/

typedef struct 
{
   float tech_node;		/* tech node (feature size) in nm */

   float local_wire_pitch;		/* wire width in nm */
   float local_wire_aspect_ratio;	/* aspect ration of wire: height/width */
   float local_wire_resistivity;	/* resistivity in ohm-nm */

   float inter_wire_pitch;		/* wire width in nm */
   float inter_wire_aspect_ratio;	/* aspect ration of wire: height/width */
   float inter_wire_resistivity;	/* resistivity in onm/nm */

   float global_wire_pitch;		/* wire width in nm */
   float global_wire_aspect_ratio;	/* aspect ration of wire: height/width */
   float global_wire_resistivity;	/* resistivity in onm/nm */
  
   float effective_permittivity;
   float bulk_permittivity;

} ITRS_roadmap_data; 


extern int num_tech_node;
extern ITRS_roadmap_data itrs_predicted_data[20];

extern float permittivity0;	/* permittivity in F/nm */
extern float tech;

#endif  /* _ITRS_INTERCONNECT_H_ */


