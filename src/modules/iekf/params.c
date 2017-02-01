#include <systemlib/param/param.h>

// 16 is max name length

/*================================================================*/
/* Gyroscope*/
/*================================================================*/

/**
 * Gyro noise density
 *
 * @group IEKF
 * @unit (rad / s) / sqrt(Hz)
 * @min 0
 * @max 1e-2
 * @decimal 7
 */
PARAM_DEFINE_FLOAT(IEKF_GYRO_ND, 9.8e-5f);

/**
 * Gyro random walk noise density
 *
 * @group IEKF
 * @unit (rad / s^2) / sqrt(Hz)
 * @min 0
 * @max 1e-2
 * @decimal 7
 */
PARAM_DEFINE_FLOAT(IEKF_GYRO_RW_ND, 1.2e-5f);

/**
 * Gyro random walk correlation time
 *
 * @group IEKF
 * @unit s
 * @min 0
 * @max 1e6
 * @decimal 0
 */
PARAM_DEFINE_FLOAT(IEKF_GYRO_RW_CT, 1e3);

/*================================================================*/
/* Accelerometer*/
/*================================================================*/

/**
 * Accel noise density
 *
 * @group IEKF
 * @unit (m / s^2) / sqrt(Hz)
 * @min 0
 * @max 1e-2
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_ACCEL_ND, 2.4e-3f);

/**
 * Accel random walk noise density
 *
 * @group IEKF
 * @unit (m / s^3) / sqrt(Hz)
 * @min 0
 * @max 1e-2
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_ACCEL_RW_ND, 2e-3f);

/**
 * Accel random walk correlation time
 *
 * @group IEKF
 * @unit s
 * @min 0
 * @max 1e6
 * @decimal 0
 */
PARAM_DEFINE_FLOAT(IEKF_ACCEL_RW_CT, 1e3);

/**
 * Accel low pass filter cut freq
 *
 * @group IEKF
 * @unit Hz
 * @min 1
 * @max 250
 * @decimal 0
 */
PARAM_DEFINE_FLOAT(IEKF_ACCEL_LP, 25);

/*================================================================*/
/* Barometric Altimeter*/
/*================================================================*/

/**
 * Barometric altitude noise density
 *
 * @group IEKF
 * @unit (m) / sqrt(Hz)
 * @min 0
 * @max 1
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_BARO_ND, 5.68e-2f);

/**
 * Barometric altitiude random walk noise density
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_BARO_RW_ND, 3.8e-2f);

/**
 * Baro random walk correlation time
 *
 * @group IEKF
 * @unit s
 * @min 0
 * @max 1e6
 * @decimal 0
 */
PARAM_DEFINE_FLOAT(IEKF_BARO_RW_CT, 1e4);

/**
 * Baro low pass filter cut freq
 *
 * @group IEKF
 * @unit Hz
 * @min 1
 * @max 250
 * @decimal 0
 */
PARAM_DEFINE_FLOAT(IEKF_BARO_LP, 25);

/*================================================================*/
/* Magnetometer */
/*================================================================*/

/**
 * Mag heading noise density
 *
 * @group IEKF
 * @unit (rad) / sqrt(Hz)
 * @min 0
 * @max 1
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_MAG_ND, 5.43e-2f);

/**
 * Mag heading random walk noise density
 *
 * @group IEKF
 * @unit (rad/s) / sqrt(Hz)

 * @max 1
 * @decimal 7
 */
PARAM_DEFINE_FLOAT(IEKF_MAG_RW_ND, 3.34e-5f);

/**
 * Mag heading random walk correlation time
 *
 * @group IEKF
 * @unit s
 * @min 0
 * @max 1e6
 * @decimal 0
 */
PARAM_DEFINE_FLOAT(IEKF_MAG_RW_CT, 1e3);

/**
 * Magnetic declination
 *
 * @group IEKF
 * @unit deg
 * @min -180
 * @max 180
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(IEKF_MAG_DECL, -4.7);

/**
 * Mag low pass filter cut freq
 *
 * @group IEKF
 * @unit Hz
 * @min 1
 * @max 250
 * @decimal 0
 */
PARAM_DEFINE_FLOAT(IEKF_MAG_LP, 25);

/*================================================================*/
/* GPS */
/*================================================================*/

/**
 * GPS xy position noise density
 *
 * @group IEKF
 * @unit (m) / sqrt(Hz)
 * @min 0
 * @max 1e2
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_GPS_XY_ND, 2e0f);

/**
 * GPS z position noise density
 *
 * @group IEKF
 * @unit (m) / sqrt(Hz)
 * @min 0
 * @max 1e2
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_GPS_Z_ND, 10e0f);

/**
 * GPS xy velocity noise density
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1e2
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_GPS_VXY_ND, 2e-1f);

/**
 * GPS z velocity noise density
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1e2
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_GPS_VZ_ND, 4e-1f);

/*================================================================*/
/* Optical Flow */
/*================================================================*/

/**
 * Optical flow noise density
 *
 * @group IEKF
 * @unit (rad/s) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_FLOW_ND, 1e-2f);

/*================================================================*/
/* Lidar */
/*================================================================*/

/**
 * Lidar noise density
 *
 * @group IEKF
 * @unit (m) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_LIDAR_ND, 1e-3f);


/*================================================================*/
/* Sonar */
/*================================================================*/

/**
 * Sonar noise density
 *
 * @group IEKF
 * @unit (m) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_SONAR_ND, 1e-2f);

/*================================================================*/
/* Land */
/*================================================================*/

/**
 * Land velocity xy noise density
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_LAND_VXY_ND, 1e-1f);

/**
 * Land velocity z noise density
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_LAND_VZ_ND, 1e-1f);

/**
 * Land agl noise density
 *
 * @group IEKF
 * @unit (m) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_LAND_AGL_ND, 1e-1f);

/*================================================================*/
/* Process Noise */
/*================================================================*/

/**
 * Process noise xy position noise density
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_PN_XY_ND, 0);

/**
 * Process noise xy velocity noise density
 *
 * @group IEKF
 * @unit (m/s^2) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_PN_VXY_ND, 1e-1f);

/**
 * Process noise z position noise density
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_PN_Z_ND, 0);

/**
 * Process noise z velocity noise density
 *
 * @group IEKF
 * @unit (m/s^2) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_PN_VZ_ND, 1e-1f);

/**
 * Process noise rotation noise density
 *
 * @group IEKF
 * @unit (rad/s) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_PN_ROT_ND, 1e-3f);

/**
 * Process noise terrain asl
 *
 * @group IEKF
 * @unit (m/s) / sqrt(Hz)
 * @min 0
 * @max 1e0
 * @decimal 5
 */
PARAM_DEFINE_FLOAT(IEKF_PN_T_ASL_ND, 1e-1f);
