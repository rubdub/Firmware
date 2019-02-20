/****************************************************************************
 *
 *   Copyright (c) 2012-2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mixer_multirotor.cpp
 *
 * Multi-rotor mixers.
 */
#include <px4_config.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <math.h>

#include <mathlib/math/Limits.hpp>
#include <drivers/drv_pwm_output.h>

#include "mixer.h"

// This file is generated by the multi_tables script which is invoked during the build process
#include "mixer_multirotor.generated.h"

#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <math.h>

#include <uORB/uORB.h>
// #include "../../../../build_px4fmu-v2_default/src/modules/uORB/topics/sensor_combined.h"
// #include "../../../../build_px4fmu-v2_default/src/modules/uORB/topics/vehicle_attitude.h"

// start of copy
#include <px4_config.h>
#include <px4_defines.h>
#include <px4_tasks.h>
#include <px4_posix.h>

#include <drivers/drv_hrt.h>
#include <ecl/attitude_fw/ecl_pitch_controller.h>
#include <ecl/attitude_fw/ecl_roll_controller.h>
#include <ecl/attitude_fw/ecl_wheel_controller.h>
#include <ecl/attitude_fw/ecl_yaw_controller.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>
#include <systemlib/param/param.h>
#include <systemlib/perf_counter.h>
#include <uORB/topics/actuator_controls.h>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/control_state.h>
// #include "../../../../build_px4fmu-v2_default/src/modules/uORB/topics/manual_control_setpoint.h"
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_land_detected.h>
#include <uORB/topics/vehicle_rates_setpoint.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/uORB.h>
#include <vtol_att_control/vtol_type.h>
#include <uORB/uORB.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/vehicle_attitude.h>

//end of copy

#include <conversion/rotation.h>
#include <drivers/drv_hrt.h>
#include <lib/geo/geo.h>
#include <lib/mathlib/mathlib.h>
#include <lib/tailsitter_recovery/tailsitter_recovery.h>
#include <px4_config.h>
#include <px4_defines.h>
#include <px4_posix.h>
#include <px4_tasks.h>
#include <systemlib/circuit_breaker.h>
#include <systemlib/err.h>
#include <systemlib/param/param.h>
#include <systemlib/perf_counter.h>
#include <systemlib/systemlib.h>
#include <uORB/topics/actuator_armed.h>
#include <uORB/topics/actuator_controls.h>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/control_state.h>
// #include "../../../build_px4fmu-v2_default/src/modules/uORB/topics/manual_control_setpoint.h"
#include <uORB/topics/mc_att_ctrl_status.h>
#include <uORB/topics/multirotor_motor_limits.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_correction.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_rates_setpoint.h>
#include <uORB/topics/vehicle_status.h>



//int channels = orb_subscribe(ORB_ID(rc_channels));
//int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_combined));
//orb_set_interval(sensor_sub_fd, 200);
//
//px4_pollfd_struct_t fds[] = {
//        { .fd = sensor_sub_fd,   .events = POLLIN },
//};
//
//if (fds[0].revents & POLLIN) {
//    struct sensor_combined_s raw;
//    /* copy sensors raw data into local buffer */
//    orb_copy(ORB_ID(sensor_combined), sensor_sub_fd, &raw);
//}

#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer] " fmt "\n", ##args); } while(0)
//#include <debug.h>
//#define debug(fmt, args...)	syslog(fmt "\n", ##args)

/*
 * Clockwise: 1
 * Counter-clockwise: -1
 */

MultirotorMixer::MultirotorMixer(ControlCallback control_cb,
				 uintptr_t cb_handle,
				 MultirotorGeometry geometry,
				 float roll_scale,
				 float pitch_scale,
				 float yaw_scale,
				 float idle_speed) :
	Mixer(control_cb, cb_handle),
	_roll_scale(roll_scale),
	_pitch_scale(pitch_scale),
	_yaw_scale(yaw_scale),
	_idle_speed(-1.0f + idle_speed * 2.0f),	/* shift to output range here to avoid runtime calculation */
	_delta_out_max(0.0f),
	_thrust_factor(0.0f),
	_limits_pub(),
	_rotor_count(_config_rotor_count[(MultirotorGeometryUnderlyingType)geometry]),
	_rotors(_config_index[(MultirotorGeometryUnderlyingType)geometry]),
	_outputs_prev(new float[_rotor_count])
{
	memset(_outputs_prev, _idle_speed, _rotor_count * sizeof(float));
}

MultirotorMixer::~MultirotorMixer()
{
	if (_outputs_prev != nullptr) {
		delete[] _outputs_prev;
	}
}

MultirotorMixer *
MultirotorMixer::from_text(Mixer::ControlCallback control_cb, uintptr_t cb_handle, const char *buf, unsigned &buflen)
{
	MultirotorGeometry geometry;
	char geomname[8];
	int s[4];
	int used;

//    _manual_control_sp_sub = orb_subscribe(ORB_ID(manual_control_setpoint));
//    bool updated;
//
//    /* get pilots inputs */
//    orb_check(_manual_control_sp_sub, &updated);
//
//    if (updated) {
//        orb_copy(ORB_ID(manual_control_setpoint), _manual_control_sp_sub, &_manual_control_sp);
//    }

//    //Code to read uORB messages
//    struct manual_control_setpoint_s		_manual;		/**< r/c channel data */
//    int		_manual_sub;			/**< notification of manual control updates */
////    bool manual_updated;
//    _manual = {}; //Initialization of struct
//    _manual_sub = orb_subscribe(ORB_ID(manual_control_setpoint));
//
////    orb_check(_manual_sub, &manual_updated);
////    if (manual_updated) {
////        orb_copy(ORB_ID(manual_control_setpoint), _manual_sub, &_manual);
////    }

//    int sensor_sub_fd = orb_subscribe(ORB_ID(manual_control_setpoint));
//    orb_set_interval(sensor_sub_fd, 200);
//    px4_pollfd_struct_t fds = {};
//    fds.events = POLLIN;
//    fds.fd = sensor_sub_fd;
//
//
//    int error_counter = 0;
//    for (int i = 0; i < 5; i++) {
//        /* wait for sensor update of 1 file descriptor for 1000 ms (1 second) */
//        int poll_ret = px4_poll(&fds, 1, 1000);
//
//        /* handle the poll result */
//        if (poll_ret == 0) {
//            /* this means none of our providers is giving us data */
//            PX4_ERR("Got no data within a second");
//
//        } else if (poll_ret < 0) {
//            /* this is seriously bad - should be an emergency */
//            if (error_counter < 10 || error_counter % 50 == 0) {
//                /* use a counter to prevent flooding (and slowing us down) */
//                PX4_ERR("ERROR return value from poll(): %d", poll_ret);
//            }
//
//            error_counter++;
//
//        } else {
//
//            if (fds.revents & POLLIN) {
//                /* obtained data for the first file descriptor */
//                struct sensor_combined_s raw;
//                /* copy sensors raw data into local buffer */
//                orb_copy(ORB_ID(sensor_combined), sensor_sub_fd, &raw);
//                PX4_INFO("Accelerometer:\t%8.4f\t%8.4f\t%8.4f",
//                         (double) raw.accelerometer_m_s2[0],
//                         (double) raw.accelerometer_m_s2[1],
//                         (double) raw.accelerometer_m_s2[2]);
//
////                /* set att and publish this information for other apps
////                 the following does not have any meaning, it's just an example
////                */
////                att.q[0] = raw.accelerometer_m_s2[0];
////                att.q[1] = raw.accelerometer_m_s2[1];
////                att.q[2] = raw.accelerometer_m_s2[2];
////
////                orb_publish(ORB_ID(vehicle_attitude), att_pub, &att);
//            }
//        }
//    }





    /* enforce that the mixer ends with a new line */
	if (!string_well_formed(buf, buflen)) {
		return nullptr;
	}

	if (sscanf(buf, "R: %7s %d %d %d %d%n", geomname, &s[0], &s[1], &s[2], &s[3], &used) != 5) {
		debug("multirotor parse failed on '%s'", buf);
		return nullptr;
	}

	if (used > (int)buflen) {
		debug("OVERFLOW: multirotor spec used %d of %u", used, buflen);
		return nullptr;
	}

	buf = skipline(buf, buflen);

	if (buf == nullptr) {
		debug("no line ending, line is incomplete");
		return nullptr;
	}

	debug("remaining in buf: %d, first char: %c", buflen, buf[0]);

	if (!strcmp(geomname, "4+")) {
		geometry = MultirotorGeometry::QUAD_PLUS;

	} else if (!strcmp(geomname, "4x")) {
		geometry = MultirotorGeometry::QUAD_X;

	} else if (!strcmp(geomname, "4h")) {
		geometry = MultirotorGeometry::QUAD_H;

	} else if (!strcmp(geomname, "4v")) {
		geometry = MultirotorGeometry::QUAD_V;

	} else if (!strcmp(geomname, "4w")) {
		geometry = MultirotorGeometry::QUAD_WIDE;

	} else if (!strcmp(geomname, "4s")) {
		geometry = MultirotorGeometry::QUAD_S250AQ;

	} else if (!strcmp(geomname, "4dc")) {
		geometry = MultirotorGeometry::QUAD_DEADCAT;

	} else if (!strcmp(geomname, "6+")) {
		geometry = MultirotorGeometry::HEX_PLUS;

	} else if (!strcmp(geomname, "6x")) {
		geometry = MultirotorGeometry::HEX_X;

	} else if (!strcmp(geomname, "6c")) {
		geometry = MultirotorGeometry::HEX_COX;

	} else if (!strcmp(geomname, "6t")) {
		geometry = MultirotorGeometry::HEX_T;

	} else if (!strcmp(geomname, "8+")) {
		geometry = MultirotorGeometry::OCTA_PLUS;

	} else if (!strcmp(geomname, "8x")) {
		geometry = MultirotorGeometry::OCTA_X;

	} else if (!strcmp(geomname, "8c")) {
		geometry = MultirotorGeometry::OCTA_COX;

	} else if (!strcmp(geomname, "6m")) {
		geometry = MultirotorGeometry::DODECA_TOP_COX;

	} else if (!strcmp(geomname, "6a")) {
		geometry = MultirotorGeometry::DODECA_BOTTOM_COX;


#if 0

	} else if (!strcmp(geomname, "8cw")) {
		geometry = MultirotorGeometry::OCTA_COX_WIDE;
#endif

	} else if (!strcmp(geomname, "2-")) {
		geometry = MultirotorGeometry::TWIN_ENGINE;

	} else if (!strcmp(geomname, "3y")) {
		geometry = MultirotorGeometry::TRI_Y;

	} else {
		debug("unrecognised geometry '%s'", geomname);
		return nullptr;
	}

	debug("adding multirotor mixer '%s'", geomname);

//    float trigger = math::constrain(get_control(0, 3), 0.0f, 1.0f);

	return new MultirotorMixer(
		       control_cb,
		       cb_handle,
		       geometry,
		       s[0] / 10000.0f,
		       s[1] / 10000.0f,
		       s[2] / 10000.0f,
		       s[3] / 10000.0f);
}

unsigned
MultirotorMixer::mix(float *outputs, unsigned space, uint16_t *status_reg)
{
	/* Summary of mixing strategy:
	1) mix roll, pitch and thrust without yaw.
	2) if some outputs violate range [0,1] then try to shift all outputs to minimize violation ->
		increase or decrease total thrust (boost). The total increase or decrease of thrust is limited
		(max_thrust_diff). If after the shift some outputs still violate the bounds then scale roll & pitch.
		In case there is violation at the lower and upper bound then try to shift such that violation is equal
		on both sides.
	3) mix in yaw and scale if it leads to limit violation.
	4) scale all outputs to range [idle_speed,1]
	*/

	float		roll    = math::constrain(get_control(0, 0) * _roll_scale, -1.0f, 1.0f);
	float		pitch   = math::constrain(get_control(0, 1) * _pitch_scale, -1.0f, 1.0f);
	float		yaw     = math::constrain(get_control(0, 2) * _yaw_scale, -1.0f, 1.0f);
	float		thrust  = math::constrain(get_control(0, 3), 0.0f, 1.0f);
	float		min_out = 1.0f;
	float		max_out = 0.0f;

    //--------------------RD, Start of conditional geometry
	bool enable_transformation = true;
    float       frame_state = math::constrain(get_control(3, 5), -1.0f, 1.0f);
//    float       elevon_state = math::constrain(get_control(3, 6), -1.0f, 1.0f);


    const Rotor quad_plus[] = {
            { -1.000000,  0.000000,  1.000000,  1.000000 },
            {  1.000000,  0.000000,  1.000000,  1.000000 },
            {  0.000000,  1.000000, -1.000000,  1.000000 },
            { -0.000000, -1.000000, -1.000000,  1.000000 },
            { -0.000000, -0.000000, -0.000000,  1.000000 },
            { -0.000000, -0.000000, -0.000000,  1.000000 },
            { -0.000000, -0.000000, -0.000000,  1.000000 },
            { -0.000000, -0.000000, -0.000000,  1.000000 },
    };

    const Rotor config_twin_engine[] = {
            { -1.000000,  0.000000,  0.000000,  1.000000 },
            {  1.000000,  0.000000,  0.000000,  1.000000 },
            {  1.000000,  0.000000,  0.000000,  1.000000 },
            { -1.000000,  0.000000,  0.000000,  1.000000 },
            {  0.000000,  0.000000, -0.000000,  0.000000 },
            {  0.000000,  0.000000, -0.000000,  0.000000 },
            { -0.000000,  0.000000, -0.000000,  0.000000 },
            { -0.000000, -0.000000, -0.000000,  0.000000 },
    };


	if (enable_transformation == true){
		if (frame_state >= 0.1f){
			_rotor_count = 8;
			_rotors = quad_plus;
		}
		else {
			_rotor_count = 4;
			_rotors = config_twin_engine;
		}
	}

/////////////////////////////Begin throttle Lock/////////////////////////////////// 
	if (fabsf(frame_state-prior_frame_state) >= 0.2f && triggerflag == false){
		prior_frame_state = frame_state;
		start_transition_time = hrt_absolute_time();
		triggerflag = true;
	}

	if (triggerflag == true){
		roll = 0.0f;
		pitch = 0.0f;
		yaw = 0.0f;
	}		
	
	current_time = hrt_absolute_time();
	
	//Transformation ROLL, PITCH, YAW LOCK Delay in us (750000 = .75seconds)
	if (triggerflag == true && (current_time - start_transition_time) > 500000.0f){	
		triggerflag = false;
	}
//////////////////////////////End Throttle Lock///////////////////////////////////

	else{}

    //-------------------End of Conditional Geometry

	// clean out class variable used to capture saturation
	_saturation_status.value = 0;

	// thrust boost parameters
	float thrust_increase_factor = 1.5f;
	float thrust_decrease_factor = 0.6f;

	/* perform initial mix pass yielding unbounded outputs, ignore yaw */
	for (unsigned i = 0; i < _rotor_count; i++) {
		float out = roll * _rotors[i].roll_scale +
			    pitch * _rotors[i].pitch_scale +
			    thrust;

		out *= _rotors[i].out_scale;

		/* calculate min and max output values */
		if (out < min_out) {
			min_out = out;
		}

		if (out > max_out) {
			max_out = out;
		}

		outputs[i] = out;
	}

	float boost = 0.0f;		// value added to demanded thrust (can also be negative)
	float roll_pitch_scale = 1.0f;	// scale for demanded roll and pitch

	if (min_out < 0.0f && max_out < 1.0f && -min_out <= 1.0f - max_out) {
		float max_thrust_diff = thrust * thrust_increase_factor - thrust;

		if (max_thrust_diff >= -min_out) {
			boost = -min_out;

		} else {
			boost = max_thrust_diff;
			roll_pitch_scale = (thrust + boost) / (thrust - min_out);
		}

	} else if (max_out > 1.0f && min_out > 0.0f && min_out >= max_out - 1.0f) {
		float max_thrust_diff = thrust - thrust_decrease_factor * thrust;

		if (max_thrust_diff >= max_out - 1.0f) {
			boost = -(max_out - 1.0f);

		} else {
			boost = -max_thrust_diff;
			roll_pitch_scale = (1 - (thrust + boost)) / (max_out - thrust);
		}

	} else if (min_out < 0.0f && max_out < 1.0f && -min_out > 1.0f - max_out) {
		float max_thrust_diff = thrust * thrust_increase_factor - thrust;
		boost = math::constrain(-min_out - (1.0f - max_out) / 2.0f, 0.0f, max_thrust_diff);
		roll_pitch_scale = (thrust + boost) / (thrust - min_out);

	} else if (max_out > 1.0f && min_out > 0.0f && min_out < max_out - 1.0f) {
		float max_thrust_diff = thrust - thrust_decrease_factor * thrust;
		boost = math::constrain(-(max_out - 1.0f - min_out) / 2.0f, -max_thrust_diff, 0.0f);
		roll_pitch_scale = (1 - (thrust + boost)) / (max_out - thrust);

	} else if (min_out < 0.0f && max_out > 1.0f) {
		boost = math::constrain(-(max_out - 1.0f + min_out) / 2.0f, thrust_decrease_factor * thrust - thrust,
					thrust_increase_factor * thrust - thrust);
		roll_pitch_scale = (thrust + boost) / (thrust - min_out);
	}

	// capture saturation
	if (min_out < 0.0f) {
		_saturation_status.flags.motor_neg = true;
	}

	if (max_out > 1.0f) {
		_saturation_status.flags.motor_pos = true;
	}

	// Thrust reduction is used to reduce the collective thrust if we hit
	// the upper throttle limit
	float thrust_reduction = 0.0f;

	// mix again but now with thrust boost, scale roll/pitch and also add yaw
	for (unsigned i = 0; i < _rotor_count; i++) {
		float out = (roll * _rotors[i].roll_scale +
			     pitch * _rotors[i].pitch_scale) * roll_pitch_scale +
			    yaw * _rotors[i].yaw_scale +
			    thrust + boost;

		out *= _rotors[i].out_scale;

		// scale yaw if it violates limits. inform about yaw limit reached
		if (out < 0.0f) {
			if (fabsf(_rotors[i].yaw_scale) <= FLT_EPSILON) {
				yaw = 0.0f;

			} else {
				yaw = -((roll * _rotors[i].roll_scale + pitch * _rotors[i].pitch_scale) *
					roll_pitch_scale + thrust + boost) / _rotors[i].yaw_scale;
			}

		} else if (out > 1.0f) {
			// allow to reduce thrust to get some yaw response
			float prop_reduction = fminf(0.15f, out - 1.0f);
			// keep the maximum requested reduction
			thrust_reduction = fmaxf(thrust_reduction, prop_reduction);

			if (fabsf(_rotors[i].yaw_scale) <= FLT_EPSILON) {
				yaw = 0.0f;

			} else {
				yaw = (1.0f - ((roll * _rotors[i].roll_scale + pitch * _rotors[i].pitch_scale) *
					       roll_pitch_scale + (thrust - thrust_reduction) + boost)) / _rotors[i].yaw_scale;
			}
		}
	}

	// Apply collective thrust reduction, the maximum for one prop
	thrust -= thrust_reduction;

	// add yaw and scale outputs to range idle_speed...1
	for (unsigned i = 0; i < _rotor_count; i++) {
		outputs[i] = (roll * _rotors[i].roll_scale +
			      pitch * _rotors[i].pitch_scale) * roll_pitch_scale +
			     yaw * _rotors[i].yaw_scale +
			     thrust + boost;

		/*
			implement simple model for static relationship between applied motor pwm and motor thrust
			model: thrust = (1 - _thrust_factor) * PWM + _thrust_factor * PWM^2
			this model assumes normalized input / output in the range [0,1] so this is the right place
			to do it as at this stage the outputs are in that range.
		 */
		if (_thrust_factor > 0.0f) {
			outputs[i] = -(1.0f - _thrust_factor) / (2.0f * _thrust_factor) + sqrtf((1.0f - _thrust_factor) *
					(1.0f - _thrust_factor) / (4.0f * _thrust_factor * _thrust_factor) + (outputs[i] < 0.0f ? 0.0f : outputs[i] /
							_thrust_factor));
		}

		/*
			Motor 1 and 3 pulse to counter pitching down moment when opening up (frame_state == fixed_wing)
			and not when closing together. 

			7/17/18 - +.25 for the NomQ is wayyyy too high, resulted in flipping the craft during transformation
		*/



		outputs[i] = math::constrain(_idle_speed + (outputs[i] * (1.0f - _idle_speed)), _idle_speed, 1.0f);

	}

	/* slew rate limiting and saturation checking */
	for (unsigned i = 0; i < _rotor_count; i++) {
		bool clipping_high = false;
		bool clipping_low = false;

		// check for saturation against static limits
		if (outputs[i] > 0.99f) {
			clipping_high = true;

		} else if (outputs[i] < _idle_speed + 0.01f) {
			clipping_low = true;

		}

		// check for saturation against slew rate limits
		if (_delta_out_max > 0.0f) {
			float delta_out = outputs[i] - _outputs_prev[i];

			if (delta_out > _delta_out_max) {
				outputs[i] = _outputs_prev[i] + _delta_out_max;
				clipping_high = true;

			} else if (delta_out < -_delta_out_max) {
				outputs[i] = _outputs_prev[i] - _delta_out_max;
				clipping_low = true;

			}
		}

		_outputs_prev[i] = outputs[i];

		// update the saturation status report
		update_saturation_status(i, clipping_high, clipping_low);

	}

	// this will force the caller of the mixer to always supply new slew rate values, otherwise no slew rate limiting will happen
	_delta_out_max = 0.0f;

	// Notify saturation status
	if (status_reg != nullptr) {
		(*status_reg) = _saturation_status.value;
	}

    if (frame_state >= 0.1f){
    // Quad-Rotor State
        outputs[4] = -0.3f;
        outputs[5] = -0.3f;
        outputs[6] = -0.3f;
        outputs[7] = -0.3f;

		// Used for setting a 1500us for gluing controlsurface horns 
		// outputs[4] = -0.0f;
        // outputs[5] = -0.0f;
        // outputs[6] = -0.0f;
        // outputs[7] = -0.0f;

    }
    else {
    // Fixed-Wing State
//        outputs[4] = 1.0f;
//        outputs[5] = 1.0f;
//        outputs[6] = 1.0f;
//        outputs[7] = 1.0f;

    }

		if (triggerflag == true){
			// outputs[0] = outputs[0] + 0.05f;
			// outputs[2] = outputs[2] + 0.05f;
			// outputs[0] = outputs[0] + 0.1f;
			// outputs[2] = outputs[2] + 0.1f;
			outputs[0] = outputs[0];
			outputs[2] = outputs[2];
			
			outputs[1] = outputs[1];
			outputs[3] = outputs[3];
		}


    return _rotor_count;


}

/*
 * This function update the control saturation status report using hte following inputs:
 *
 * index: 0 based index identifying the motor that is saturating
 * clipping_high: true if the motor demand is being limited in the positive direction
 * clipping_low: true if the motor demand is being limited in the negative direction
*/
void
MultirotorMixer::update_saturation_status(unsigned index, bool clipping_high, bool clipping_low)
{
	// The motor is saturated at the upper limit
	// check which control axes and which directions are contributing
	if (clipping_high) {
		if (_rotors[index].roll_scale > 0.0f) {
			// A positive change in roll will increase saturation
			_saturation_status.flags.roll_pos = true;

		} else if (_rotors[index].roll_scale < 0.0f) {
			// A negative change in roll will increase saturation
			_saturation_status.flags.roll_neg = true;

		}

		// check if the pitch input is saturating
		if (_rotors[index].pitch_scale > 0.0f) {
			// A positive change in pitch will increase saturation
			_saturation_status.flags.pitch_pos = true;

		} else if (_rotors[index].pitch_scale < 0.0f) {
			// A negative change in pitch will increase saturation
			_saturation_status.flags.pitch_neg = true;

		}

		// check if the yaw input is saturating
		if (_rotors[index].yaw_scale > 0.0f) {
			// A positive change in yaw will increase saturation
			_saturation_status.flags.yaw_pos = true;

		} else if (_rotors[index].yaw_scale < 0.0f) {
			// A negative change in yaw will increase saturation
			_saturation_status.flags.yaw_neg = true;

		}

		// A positive change in thrust will increase saturation
		_saturation_status.flags.thrust_pos = true;

	}

	// The motor is saturated at the lower limit
	// check which control axes and which directions are contributing
	if (clipping_low) {
		// check if the roll input is saturating
		if (_rotors[index].roll_scale > 0.0f) {
			// A negative change in roll will increase saturation
			_saturation_status.flags.roll_neg = true;

		} else if (_rotors[index].roll_scale < 0.0f) {
			// A positive change in roll will increase saturation
			_saturation_status.flags.roll_pos = true;

		}

		// check if the pitch input is saturating
		if (_rotors[index].pitch_scale > 0.0f) {
			// A negative change in pitch will increase saturation
			_saturation_status.flags.pitch_neg = true;

		} else if (_rotors[index].pitch_scale < 0.0f) {
			// A positive change in pitch will increase saturation
			_saturation_status.flags.pitch_pos = true;

		}

		// check if the yaw input is saturating
		if (_rotors[index].yaw_scale > 0.0f) {
			// A negative change in yaw will increase saturation
			_saturation_status.flags.yaw_neg = true;

		} else if (_rotors[index].yaw_scale < 0.0f) {
			// A positive change in yaw will increase saturation
			_saturation_status.flags.yaw_pos = true;

		}

		// A negative change in thrust will increase saturation
		_saturation_status.flags.thrust_neg = true;

	}
}

void
MultirotorMixer::groups_required(uint32_t &groups)
{
	/* XXX for now, hardcoded to indexes 0-3 in control group zero */
	groups |= (1 << 0);
}

uint16_t MultirotorMixer::get_saturation_status()
{
	return _saturation_status.value;
}
