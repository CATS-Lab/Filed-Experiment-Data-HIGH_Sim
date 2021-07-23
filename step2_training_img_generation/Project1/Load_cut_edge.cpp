#include "FRAMES.h"
#include "Poly.h"

void FRAMES::load_edge_function(vector<vector<Point>> center, string edge_path, vector<int> fnum, vector<int> &output) {
	Public_func pf;
	Mat frame, frame_shifted, frame_rotated, frame_merged;
	vector<Point> points;
	Size2i resized(IMGW*2*ratio_frames, IMGH * 2 * ratio_frames);
	Mat affine_M;
	if (pf.exists_file(edge_path)) {
		load_edge_from_file(edge_path, points);
	}
	else {
		for (int i = 0; i < center.size(); i += 4000) {
			load_frame(fnum[i], frame);
			Size large_size = frame.size() * 2;
			if (center[0].size() > 3)
				affine_M = findHomography(center[i], center[0]);
			else{
				Point2f a0[3] = {center[0][0], center[0][1], center[0][2]};
				Point2f a1[3] = { center[i][0], center[i][1], center[i][2] };
				affine_M = getAffineTransform(a1, a0);
			}
			Mat shift_M = (Mat_<double>(2, 3) << 1, 0, large_size.width / 4, 0, 1, large_size.height / 4);
			cv::warpAffine(frame, frame_shifted, shift_M, large_size);
			if (center[0].size() > 3)
				cv::warpPerspective(frame_shifted, frame_rotated, affine_M, large_size);
			else
				cv::warpAffine(frame_shifted, frame_rotated, affine_M, large_size);
			//show_frame(frame_rotated, 10000);
			if (!frame_merged.empty())
				cv::addWeighted(frame_merged, 0.5, frame_rotated, 0.5, 0, frame_merged);
			else
				frame_merged = frame_rotated.clone();
		}
		cv::resize(frame_merged, frame_merged, resized);
		load_edge_from_frame(frame_merged, points);
		ofstream outcsv(edge_path.c_str());
		for (auto p : points)
			outcsv << p.x << "," << p.y << "\n";
	}
	
	PolynomialRegression<long double> poly;
	vector<long double> vy, vx, f;
	for (auto p : points) {
		vy.push_back(long double(p.y));
		vx.push_back(long double(p.x));
	}
	poly.fitIt(vy, vx, EDGE_FIT_ORDER, f);
	output.clear();
	for (double y = 0; y < resized.height; y = y + 1.0) {
		double xi = 0;
		for (int j = 0; j < EDGE_FIT_ORDER + 1; j++)
			xi += f[j] * pow(y, j);
		output.push_back(xi);
	}
}

void FRAMES::load_edge_from_file(string edge_path, vector<Point> &points) {
	ifstream input(edge_path.c_str());
	string xs, ys;
	float x, y;
	while (getline(input, xs, ',')) {
		x = stoi(xs);
		getline(input, ys);
		y = stof(ys);
		points.push_back(Point(x, y));
	}
}

Mat frame_draw;
void Cross(cv::Mat frame, cv::Point point, cv::Scalar s) {
	int cl = 10;
	cv::Point a(cl, 0), b(-cl, 0), c(0, cl), d(0, -cl);
	line(frame, point + a, point + b, s, 4);
	line(frame, point + c, point + d, s, 4);
}

void Mouseclick(int evt, int x, int y, int flags, void* edgepoints) {
	cv::Scalar s(0, 0, 255);
	std::vector<cv::Point>* edgep = (std::vector<cv::Point>*)edgepoints;
	if (evt == CV_EVENT_LBUTTONDOWN) {
		edgep->push_back(Point(x, y));
		if (edgep->size() >= 1) {
			Point tem = edgep->back();
			Cross(frame_draw, tem, s);
		}
		imshow("Merged_frame", frame_draw);
	}
}
void FRAMES::load_edge_from_frame(Mat frame_merged, vector<cv::Point> &points){
	frame_draw = frame_merged.clone();
	string windowname = "Merged_frame";
	namedWindow(windowname, WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow(windowname, 0, 0);
	cv::resizeWindow(windowname, 1820, 1000);

	cv::setMouseCallback(windowname, Mouseclick, &points);
	
	imshow(windowname, frame_draw);
reload:
	points.clear();
	char c = waitKey(0);
	if (c == 'c') {
		if (points.size() > 3) {
			destroyWindow(windowname);
			return;
		}
		else {
			cout << "please enter more points";
			goto reload;
		}
	}
	else
		goto reload;
	
}