# default header
import os
env = Environment(
	CXX = "clang++",
	CXXFLAGS = '-std=c++11 -Wno-deprecated -Wno-attributes',
	CCFLAGS = ['-O3', '-g'],
	CPPPATH = [
		'/usr/include/eigen3',
		'/usr/include/pcl-1.6',
		'/usr/include/ni',
	],
	LIBPATH = [
	])
env['ENV']['TERM'] = os.environ['TERM']

# project specific code
env.Program(
	'opencv_server',
	source = Glob('*.cpp') + Glob('*/*.cpp'),
	LIBS = [
		'libboost_system-mt',
		'libboost_thread-mt',
		'libdl',
		'libopencv_core',
		'libopencv_highgui',
		'libopencv_imgproc',
		'libpthread',
		'libthrift',
		])
