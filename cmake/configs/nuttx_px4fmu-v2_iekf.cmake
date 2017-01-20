include(configs/nuttx_px4fmu-v2_common)

set(PARAM_DEFAULT_OVERRIDES "{\\\"SYS_MC_EST_GROUP\\\": 3}")

list(REMOVE_ITEM config_module_list
	modules/sdlog2
	)

list(APPEND config_module_list
	modules/iekf
	lib/ros
	modules/logger
	)
