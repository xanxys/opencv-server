from __future__ import print_function
import sys
sys.path.append('./gen-py')

import argparse
from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

from calibration import *
from calibration.ttypes import *

if __name__ == '__main__':
	# Parse argument.
	parser = argparse.ArgumentParser(description='Check health of the server')
	parser.add_argument('--host', default='localhost')
	parser.add_argument('--port', default=9090, type=int)
	args = parser.parse_args()

	# Connect
	transport = TSocket.TSocket(args.host, args.port)
	transport = TTransport.TBufferedTransport(transport)
	protocol = TBinaryProtocol.TBinaryProtocol(transport)
	client = CalibrationServer.Client(protocol)
	transport.open()
	print('Connected to %s:%d' % (args.host, args.port))

	print('Result', client.calibrateCamera([], Vector2(100, 75)))

	# 
	transport.close()
