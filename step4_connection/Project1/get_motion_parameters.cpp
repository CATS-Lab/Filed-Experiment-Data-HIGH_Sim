#include "FRAMES.h"

void FRAMES::get_motion_parameters() {
	//when we want to restart the process, load parameters and find initial fnum is needed;
	Mat cur_frame, pre_frame, cur_depth;
	//138001 or 0
	for (int fnum = 138001; fnum < 216000 ; fnum+=30) {
		if (!load_frame(fnum, cur_frame)){
			save_results(pre_frame, fnum);
			break;
		}
		if (circle_mat.empty()){
			load_depth(fnum, cur_depth);
			load_circle(cur_frame, cur_depth);
			if (pre_frame.empty()) 
				pre_frame = cur_frame.clone();
		}
		else{
			bool matched = match_circles(cur_frame);//cur
			if (!matched) {
				cout << "circles matched unsuccefully, reload started" << endl;
				load_circle(cur_frame, cur_depth);
				continue;
			}
		}
		
		if ((fnum-1) % (1200) == 0)// * 33
			save_results(cur_frame, fnum);//cur
		cout << fnum << "-";

	}
	
}



bool FRAMES::load_depth(int fnum, Mat &cur_depth) {
	string mat_path = frame_path + to_string(fnum) + ".jpg";
	cur_depth = imread(mat_path, IMREAD_ANYDEPTH);
	if (cur_depth.empty())
		return false;
	else return true;
}

//save frame with circles and locations(framenum, position (x,y) and radius)
void FRAMES::save_results(Mat cur_frame, int fnum) {
	for(int i = 0; i < vector_c_p.back().size(); i ++)
		cv::circle(cur_frame, vector_c_p.back()[i], vector_c_r.back()[i], Scalar(255, 0, 0), 5);
	string img_path = temp_frame_path + to_string(fnum) + ".jpg";
	cv::imwrite(img_path, cur_frame);

	ofstream ofstr;
	string csv_path_s = temp_data_path + to_string(fnum) + ".csv";
	ofstr.open(csv_path_s.c_str());
	for (int i = 0; i < vector_c_p.size(); i++) {
		ofstr << i << ",";
		for (int j = 0; j < vector_c_p[i].size(); j++) {
			ofstr << vector_c_p[i][j].x << "," <<
				vector_c_p[i][j].y << "," << vector_c_r[i][j]<<",";
		}
		ofstr << "\n";
	}
}


void FRAMES::push_points() {
	vector_c_p.push_back(circle_point);
	vector_c_r.push_back(circle_rad);
}