
/**
 *
 * \file main.c
 *
 * \brief Example Master App for Cyclic Synchronous Position (on PC)
 *
 *
 *
 * Copyright (c) 2013, Synapticon GmbH
 * All rights reserved.
 * Author: Pavan Kanajar <pkanajar@synapticon.com> & Christian Holl <choll@synapticon.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Execution of this software or parts of it exclusively takes place on hardware
 *    produced by Synapticon GmbH.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Synapticon GmbH.
 *
 */

#include <ctrlproto_m.h>
#include <ecrt.h>
#include "ethercat_setup.h"
#include <stdio.h>
#include <stdbool.h>
#include "profile.h"
#include "drive_function.h"
#include <motor_define.h>
#include <sys/time.h>
#include <time.h>


int main()
{
	int flag = 0;

	int acceleration = 350;				// rpm/s
	int deceleration = 350;   			// rpm/s
	int velocity = 350;					// rpm
	float actual_position = 0.0f;		// degree
	float target_position = 350.0f;		// degree
	int actual_velocity;
	int steps = 0;
	int i = 1;
	int position_ramp = 0;

	int slave_number = 0;

	init_master(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	init_nodes(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	set_operation_mode(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	initialize_position_profile_limits(slave_number, slv_handles);


	i = 0;
	while(1)
	{

		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		if(master_setup.op_flag)	//Check if the master is active
		{
			/* Read Actual Position from the node */
			if(flag == 0)
			{
			 	 actual_position = get_position_actual_degree(slave_number, slv_handles);
			 	 i = i+1;
			 	 if(i>3)
			 	 {
			 	 	 target_position =  actual_position + 200.0f;
			 	 	 if(target_position > 350.0f)
			 	 	 	 target_position = 300.0f;
			 	 	 steps = init_position_profile_params(target_position, actual_position, velocity, acceleration, \
								deceleration, slave_number, slv_handles);
			 	 	 flag = 1;
			 	 	 i = 1;
			 	 	 printf("steps %d target %f actual %f\n", steps, target_position, actual_position);
			 	 }
			}

			if(i<steps && flag == 1)
			{
				position_ramp =  generate_profile_position(i, slave_number, slv_handles);
				//printf(" position_ramp %d\n", position_ramp);
				set_position_degree(position_ramp, slave_number, slv_handles);
				i = i+1;
			}
			if(i >= steps && flag == 1)
			{
				break;
			}

			/*if(i<steps - steps/2&& flag == 1)
			{
				position_ramp = generate_profile_position(i, slave_number, slv_handles);
				set_position_degree(position_ramp, slave_number, slv_handles);
				i = i+1;
			}
			else if(flag == 1 && i >=steps-steps/2)
			{
				break;
			}*/

			/*if(i>=steps && flag == 0)
			{
				actual_position = get_position_actual_degree(slave_number, slv_handles);
				target_position = 50.0f;
				velocity = 350;
				acceleration = 350;
				deceleration = 350;
				steps = init_position_profile_params(target_position, actual_position, velocity, acceleration, \
							deceleration, slave_number, slv_handles);
				i = 1;
				flag = 1;
			}*/
			actual_position = get_position_actual_degree(slave_number, slv_handles);
			actual_velocity = get_velocity_actual_rpm(slave_number, slv_handles);
			printf("actual position %f actual velocity %d\n", actual_position, actual_velocity);
		}
	}

	quick_stop_position(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	renable_ctrl_quick_stop(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES); //after quick-stop

	set_operation_mode(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	enable_operation(slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

	shutdown_operation(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);





//*/

	/*while(1)
	{
		pdo_handle_ecat(&master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);

		printf("actual position %f\n", get_position_actual_degree(slave_number, slv_handles));
	}*/
//	shutdown_operation(CSP, slave_number, &master_setup, slv_handles, TOTAL_NUM_OF_SLAVES);


	return 0;
}

