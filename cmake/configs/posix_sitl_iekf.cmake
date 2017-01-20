include(cmake/configs/posix_sitl_common.cmake)

list(APPEND config_module_list
	modules/iekf
	lib/ros
	)

set(config_sitl_rcS_dir
	posix-configs/SITL/init/iekf
	)
