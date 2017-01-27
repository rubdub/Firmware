include(cmake/configs/posix_sitl_common.cmake)

# This is already the default, but lets explicitly set it again to lpe.
set(config_sitl_rcS_dir
	posix-configs/SITL/init/lpe
	)
