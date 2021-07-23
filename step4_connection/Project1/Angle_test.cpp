#include "FRAMES.h"

void FRAMES::test_angle(string path, bool st1) {
	vector<Point> center;
	vector<float> angle;
	vector<int> fnum;
	load_angle_parameters(gen_path + path, fnum, center, angle);
	Mat frame, frame_shifted, frame_rotated;
	Point c0 = center[0];
	for (int i = 0; i < center.size(); i+=1000) {
		load_frame(fnum[i], frame);
		int shiftx = center[i].x - c0.x;
		int shifty = center[i].y - c0.y;
		//Mat rotate_M = getRotationMatrix2D(c0, (angle[i] - PI / 2)*180.0/PI, 1);
		//Mat shift_M = (Mat_<double>(2, 3) << 1, 0, -shiftx, 0, 1, -shifty);
		//cv::warpAffine(frame, frame_shifted, shift_M, frame.size());
		Mat rotate_M = getRotationMatrix2D(center[i], (angle[i] - PI / 2)*180.0 / PI, 1);
		//for left 1st video, shift 8190-5600=2500;
		Mat shift_M = (Mat_<double>(2, 3) << 0, 0, -shiftx, 0, 0, -shifty+2500*st1);
		rotate_M = rotate_M + shift_M;
		cv::warpAffine(frame, frame_rotated, rotate_M, frame.size());
		string img_path = "N:/LEFT/mid_processing/rotated/" + to_string(fnum[i]) + ".jpg";
		cv::imwrite(img_path, frame_rotated);

	}

}
void FRAMES::load_angle_parameters(string path, vector<int> &fnum, vector<Point> &center, vector<float> &angle) {
	ifstream ifstr(path.c_str());
	string snum;
	while (getline(ifstr, snum, ',')) {
		fnum.push_back(stoi(snum));
		int i = 0, x = 0, y = 0;
		string xs, ys, as;
		float a;
		getline(ifstr, xs, ',');
		x = stoi(xs);
		getline(ifstr, ys, ',');
		y = stoi(ys);
		getline(ifstr, as);
		a = stof(as);
		center.push_back(Point(x, y));
		angle.push_back(a);
	}
}

