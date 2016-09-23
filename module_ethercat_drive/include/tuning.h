/*
 * tuning.h
 *
 *  Created on: Jul 13, 2015
 *      Author: Synapticon GmbH
 */


#pragma once

#include <platform.h>
#include <motor_control_interfaces.h>
#include <refclk.h>
#include <adc_service.h>
#include <position_ctrl_service.h>
#include <profile_control.h>
#include <ethercat_service.h>
#include <pdo_handler.h>
#include <position_feedback_service.h>
#include <stdint.h>

#include <xscope.h>
#include <mc_internal_constants.h>

//FixMe Profiler initialization should be done in main. The user_config thus will be excluded from here!
//#include <user_config_speedy_A1.h>
//#include <user_config.h>

interface PositionLimiterInterface {
    void set_limit(int limit);
    int get_limit();
};

void position_limiter(int position_limit, interface PositionLimiterInterface server i_position_limiter, client interface MotorcontrolInterface i_motorcontrol);

int tuning_handler(
        /* input */  uint16_t controlword, uint32_t control_extension, uint32_t target_position,
        /* output */ uint16_t &status_mux, uint32_t &tuning_result,
        ProfilerConfig          &profiler_config,
        MotorcontrolConfig      &motorcontrol_config,
        UpstreamControlData     send_to_master,
        DownstreamControlData   send_to_control,
        interface MotorcontrolInterface client i_motorcontrol,
        interface PositionVelocityCtrlInterface client i_position_control,
        client interface PositionFeedbackInterface ?i_position_feedback,
        client interface PositionLimiterInterface ?i_position_limiter
    );
