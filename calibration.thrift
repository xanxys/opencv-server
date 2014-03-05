
// Very basic constructs.
struct Quaternion {
	1: required double x,
	2: required double y,
	3: required double z,
	4: required double w,
}

struct Vector2 {
	1: required double x,
	2: required double y,
}

struct Vector3 {
	1: required double x,
	2: required double y,
	3: required double z,
}

// 3D rigid transform f(x) = rotation * x + translation.
struct RigidTransform {
	1: Vector3 translation,
	2: Quaternion rotation,
}


// Image space - world correspondences.
struct CalibrationPoints {
	// Points in each image/world spaces. Lengths must be the same.
	1: optional list<Vector2> points_image,
	2: optional list<Vector3> points_world,
}

struct CalibrationResult {
	// Intrinsic parameters in pixel.
	1: required double focus_x,
	2: required double focus_y,
	3: required double center_x,
	4: required double center_y,

	// Extrinsic parameters expressed as Local -> World transforms.
	5: optional list<RigidTransform> poses,

	// Radial distortion coefficients.
	10: optional double dist_k1,
	11: optional double dist_k2,

	// Tangential distortion coefficients.
	15: optional double dist_p1,
	16: optional double dist_p2,

	// Average reprojection error in pixel.
	20: optional double reprojection_error,
}

struct DetectionResult {
	1: optional list<Vector2> points,
}

struct Image {
	1: optional string mime,
	2: optional string data,
}

service CalibrationServer {
	// Find rows * cols corners of chessboard image and return (if successful) them in
	// [row1, row2, row3, ...] order.
	DetectionResult findChessboardCorners(
		1: Image image,
		2: i16 rows,
		3: i16 cols);

	// http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html#calibratecamera
	CalibrationResult calibrateCamera(
		1: list<CalibrationPoints> correspondences,
		2: Vector2 image_size);
}
