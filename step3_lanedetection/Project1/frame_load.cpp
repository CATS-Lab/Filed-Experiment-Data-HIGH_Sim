#include "FRAMES.h"

bool FRAMES::load_frame(int fnum, Mat &cur_frame) {
	string mat_path = frame_path + to_string(fnum) + ".jpg";
	cur_frame = imread(mat_path);
	if (cur_frame.empty())
		return false;
	else return true;
}


bool FRAMES::load_frame(string frame_path, Mat &cur_frame) {
	cur_frame = imread(frame_path);
	if (cur_frame.empty())
		return false;
	else return true;
}