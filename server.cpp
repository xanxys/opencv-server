#include <cmath>
#include <vector>

#include <opencv2/opencv.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "gen-cpp/CalibrationServer.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

class CalibrationServerHandler : virtual public CalibrationServerIf {
 public:
	CalibrationServerHandler() {
	}

	void findChessboardCorners(DetectionResult& _return, const Image& image, const int16_t rows, const int16_t cols) {
		// Load image.
		if(!image.__isset.data) {
			return;
		}
		const std::vector<uint8_t> bytes(image.data.begin(), image.data.end());
		cv::Mat image_n = cv::imdecode(cv::Mat(bytes, CV_8U), CV_LOAD_IMAGE_COLOR);

		// Recognize points.
		std::vector<cv::Vec2d> points;
		if(!cv::findChessboardCorners(image_n, cv::Size(cols, rows), points)) {
			return;
		}

		// Copy points.
		std::vector<Vector2> points_r;
		for(const auto& point : points) {
			Vector2 pt;
			pt.__set_x(point[0]);
			pt.__set_y(point[1]);
			points_r.push_back(pt);
		}
		_return.__set_points(points_r);
	}

	void calibrateCamera(CalibrationResult& _return, const std::vector<CalibrationPoints>& correspondences, const Vector2& image_size) {
		// Decompose correspondences.
		std::vector<std::vector<cv::Vec3d>> sets_points_world;
		std::vector<std::vector<cv::Vec2d>> sets_points_image;
		for(const auto& corresp : correspondences) {
			std::vector<cv::Vec3d> points_world;
			for(const auto& pt : corresp.points_world) {
				points_world.push_back(cv::Vec3d(pt.x, pt.y, pt.z));
			}

			std::vector<cv::Vec2d> points_image;
			for(const auto& pt : corresp.points_image) {
				points_image.push_back(cv::Vec2d(pt.x, pt.y));
			}

			sets_points_world.push_back(points_world);
			sets_points_image.push_back(points_image);
		}

		cv::Mat_<double> intrinsic;
		std::vector<cv::Vec3d> rotations;
		std::vector<cv::Vec3d> translations;
		std::vector<double> coeff;
		const double error = cv::calibrateCamera(sets_points_world, sets_points_image, cv::Size(image_size.x, image_size.y),
			intrinsic, coeff, rotations, translations);

		_return.__set_reprojection_error(error);

		_return.__set_focus_x(intrinsic(0, 0));
		_return.__set_focus_y(intrinsic(1, 1));
		_return.__set_center_x(intrinsic(0, 2));
		_return.__set_center_y(intrinsic(1, 2));

		assert(coeff.size() >= 4);
		_return.__set_dist_k1(coeff[0]);
		_return.__set_dist_k2(coeff[1]);
		_return.__set_dist_p1(coeff[2]);
		_return.__set_dist_p2(coeff[3]);

		std::vector<RigidTransform> poses;
		assert(rotations.size() == translations.size());
		for(int i = 0; i < rotations.size(); i++) {
			RigidTransform trans;

			trans.translation.x = translations[i][0];
			trans.translation.y = translations[i][1];
			trans.translation.z = translations[i][2];
			trans.__isset.translation = true;

			// Euler vector -> quaternion. First calculate sin(theta/2)/theta.
			// cf. https://www.wolframalpha.com/input/?i=sin%28x%2F2%29%2Fx
			const double theta = cv::norm(rotations[i]);
			const double scale = (theta < 1e-6) ? (0.5 - std::pow(theta, 2) / 48.0) : (std::sin(theta / 2) / theta);
			trans.rotation.x = rotations[i][0] * scale;
			trans.rotation.y = rotations[i][1] * scale;
			trans.rotation.z = rotations[i][2] * scale;
			trans.rotation.w = std::sqrt(1 - std::pow(trans.rotation.x, 2) - std::pow(trans.rotation.y, 2) - std::pow(trans.rotation.z, 2));
			trans.__isset.rotation = true;

			poses.push_back(trans);
		}
	}
};

int main(int argc, char** argv) {
	int port = 9090;
	shared_ptr<CalibrationServerHandler> handler(new CalibrationServerHandler());
	shared_ptr<TProcessor> processor(new CalibrationServerProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	server.serve();
	return 0;
}
