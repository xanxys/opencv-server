# default header
import os
env = Environment(
	CXX = "clang++",
	CXXFLAGS = '-std=c++11 -Wno-deprecated -Wno-attributes',
	CCFLAGS = ['-O3', '-g'],
	CPPPATH = [
		'/usr/include/eigen3',
	],
	LIBPATH = [
	])
env['ENV']['TERM'] = os.environ['TERM']

# project specific code
proto = env.Command([
	'gen-cpp/CalibrationServer.cpp',
	'gen-cpp/calibration_constants.cpp',
	'gen-cpp/calibration_types.cpp',
	],
	'calibration.thrift',
	'thrift --gen cpp $SOURCE')

env.Command(Dir('gen-py'), 'calibration.thrift', 'thrift --gen py $SOURCE')

prog = env.Program(
	'opencv_server',
	source = [
		'server.cpp',
		'gen-cpp/CalibrationServer.cpp',
		'gen-cpp/calibration_constants.cpp',
		'gen-cpp/calibration_types.cpp',
		],
	LIBS = [
		'libboost_system-mt',
		'libboost_thread-mt',
		'libdl',
		'libopencv_core',
		'libopencv_highgui',
		'libopencv_imgproc',
		'libopencv_calib3d',
		'libpthread',
		'libthrift',
		])

env.Clean(proto, Dir('gen-cpp'))
env.Depends(prog, proto)
