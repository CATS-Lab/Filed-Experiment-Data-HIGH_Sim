#include "FRAMES.h"

void FRAMES::get_motion_parameters(int startframenumber, int freq) {
	//when we want to restart the process, load parameters and find initial fnum is needed;
	Mat cur_frame, pre_frame, cur_depth;
	//138001 or 0
	int endnum = 118000;
	if (startframenumber > 20000)
		endnum = 216000;
	for (int fnum = startframenumber; fnum < 216000 ; fnum+=freq) {
		if (!load_frame(fnum, cur_frame)){
			save_results(pre_frame, fnum, freq, startframenumber);
			break;
		}
		if (circle_mat.empty()){
			load_depth(fnum, cur_depth);
			load_circle(cur_frame, cur_depth, startframenumber);
			if (pre_frame.empty()) 
				pre_frame = cur_frame.clone();
		}
		else{
			bool matched = match_circles(cur_frame);//cur
			if (!matched) {
				cout << "circles matched unsuccefully, reload started" << endl;
				load_circle(cur_frame, cur_depth,1);
				continue;
			}
			if (fnum % 1000 == 0) {
				load_depth(fnum, cur_depth);
				circle_point = vector_c_p.back();
				circle_rad = vector_c_r.back();
				circle_mat.clear();
				circle_mat_depth.clear();
				for (int k = 0; k < circle_rad.size(); k++) {
					int r = circle_rad[k];
					Point p = circle_point[k];
					Rect rect = Rect(p.x - r, p.y - r, r*2, r * 2);
					circle_mat.push_back(cur_frame(rect).clone());
					circle_mat_depth.push_back(cur_depth(rect).clone());
				}
			}
		}
		
		if ((fnum - startframenumber) % (300) == 0)// * 33
			save_results(cur_frame, fnum, freq, startframenumber);//cur
		cout << fnum << "-";

	}
	
}

bool FRAMES::load_frame(int fnum, Mat &cur_frame) {
	string mat_path = frame_path + to_string(fnum) + ".jpg";
	cur_frame = imread(mat_path);
	if (cur_frame.empty())
		return false;
	else return true;
}

bool FRAMES::load_depth(int fnum, Mat &cur_depth) {
	string mat_path = frame_path + to_string(fnum) + ".jpg";
	cur_depth = imread(mat_path, IMREAD_ANYDEPTH);
	if (cur_depth.empty())
		return false;
	else return true;
}

//save frame with circles and locations(framenum, position (x,y) and radius)
void FRAMES::save_results(Mat cur_frame, int fnum, int freq, int startframenumber) {
	for(int i = 0; i < vector_c_p.back().size(); i ++){
		cv::circle(cur_frame, vector_c_p.back()[i], vector_c_r.back()[i], Scalar(255, 0, 0), 5);
		cv::putText(cur_frame, to_string(i), vector_c_p.back()[i], 1, 10, Scalar(255, 0, 0), 2);
	}
	string img_path = temp_frame_path + to_string(fnum) + ".jpg";
	cv::imwrite(img_path, cur_frame);

	ofstream ofstr;
	string csv_path_s = temp_data_path + to_string(fnum) + ".csv";
	ofstr.open(csv_path_s.c_str());
	for (int i = 0; i < vector_c_p.size(); i++) {
		ofstr << i*freq + startframenumber << ",";
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