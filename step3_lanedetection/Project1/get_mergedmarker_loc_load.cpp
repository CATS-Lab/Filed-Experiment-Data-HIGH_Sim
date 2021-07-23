#include "FRAMES.h"


void FRAMES::load_markers_from_file(string marker_path) {
	ifstream csv(marker_path.c_str());
	string ks1, ks2x, ks2y;
	int k1, k2x, k2y;
	Point k2;
	Mat frame;
	while(getline(csv, ks1,',')) {
		getline(csv, ks2x, ',');
		getline(csv, ks2y);
		k1 = stoi(ks1);
		k2.x = stoi(ks2x);
		k2.y = stoi(ks2y);
		marker_all_loc.push_back(k2);
		marker_ID.push_back(k1);
		string imgpath = "N:/LEFT/temp_data/"  + to_string(iffirst) + "_" + to_string(k1) + "_marker.jpg";
		load_frame(imgpath, frame);		
		marker_imgs.push_back(frame.clone());
	}
	string parapath = "N:/LEFT/temp_data/"  + to_string(iffirst) + "_shift_trans.csv";
	ifstream fout(parapath.c_str());
	if (!fout)
	{
		cout << "File Not Opened" << endl;  return;
	}
	getline(fout, ks1, ',');
	shift_left.x = stoi(ks1);
	getline(fout, ks1, ',');
	shift_left.y = stoi(ks1);
	getline(fout, ks1, ',');
	shift_right.x = stoi(ks1);
	getline(fout, ks1);
	shift_right.y = stoi(ks1);
	getline(fout, ks1, ',');
	resize_left = stoi(ks1);
	getline(fout, ks1);
	resize_right = stoi(ks1);
	trans_left = Mat::zeros(3, 3, CV_32F);;

	for (int i = 0; i< 3; i++)
	{
		for (int j = 0; j< 3; j++)
		{
			getline(fout, ks1, ',');
			trans_left.at<float>(i, j) = stof(ks1);
		}
	}

	fout.close();
}


void FRAMES::load_left_right_trans_marker(vector<Point2i> &left_p, vector<Point2i> &right_p) {
	vector<string> g_path = { "N:/LEFT/","L:/MIDDLE/","N:/RIGHT/" };
	string marker_left_p = g_path[0] + +"temp_data/connection_" + to_string(iffirst) + ".csv";
	string marker_right_p = g_path[2] + +"temp_data/connection_" + to_string(iffirst) + ".csv";
	string shift_left_p = g_path[0] + +"temp_data/connect_" + to_string(iffirst) + ".csv";
	string shift_right_p = g_path[2] + +"temp_data/connect_" + to_string(iffirst) + ".csv";
	//load

	load_marker_loc(left_p, marker_left_p);
	load_marker_loc(right_p, marker_right_p);
	vector<Point2i> left_s, right_s;
	load_marker_loc(left_s, shift_left_p);
	load_marker_loc(right_s, shift_right_p);
	//calculate trans;
	//left
	Point2i disp = left_p[2] - left_p[0];
	vector<Point2i> des_p = left_p;
	for (int i = 3; i < des_p.size(); i++)
		des_p[i] = des_p[i - 2] + disp;
	trans_left = findHomography(left_p, des_p);
	resize_left = float(des_p[0].x - des_p[1].x) / float(left_s[0].x - left_s[1].x)*1.03;
	resize_right = float(right_p[0].x - right_p[1].x) / float(right_s[0].x - right_s[1].x);
	shift_left = left_s[0] - des_p[0]*resize_left;
	shift_right = right_s[0] - right_p[0] * resize_right;
	vector<Point2f> points;
	for (auto p : left_p)
		points.push_back(p);
	cv::perspectiveTransform(points, points, trans_left);

	left_p.clear();
	for (auto p : points)
		left_p.push_back(p);
}

void FRAMES::load_marker_loc(vector<Point2i> &markers_p, string path_marker) {
	ifstream ifstr(path_marker.c_str());
	string xs, ys;
	int x, y;
	while (getline(ifstr, xs, ',')) {
		getline(ifstr, ys, ',');
		x = stoi(xs);
		y = stoi(ys);
		markers_p.push_back(Point(x, y));
		getline(ifstr, xs, ',');
		getline(ifstr, ys);
		x = stoi(xs);
		y = stoi(ys);
		markers_p.push_back(Point(x, y));
	}
}

void FRAMES::load_middle_markers(Mat frame, vector<int> edge, vector<Point2i> &middle_markers) {
	//load
	vector<string> g_path = { "N:/LEFT/","L:/MIDDLE/","N:/RIGHT/" };
	string marker_middle_p = g_path[1] + +"temp_data/connection_" + to_string(iffirst) + ".csv";
	load_marker_loc(middle_markers, marker_middle_p);
}

void FRAMES::calculate_load(vector<Point2i> left_markers, vector<Point2i> middle_markers, vector<Point2i> right_markers, vector<Mat> frames){
	
	int shift_dis = 6000;
	
	Point2i shift_p(0,shift_dis);
	shift_left += shift_p;
	shift_right += shift_p;
	int id = 0;
	Point2i p1, p2;
	int circle_size = 5;
	Point left_up(circle_size, circle_size), right_down(circle_size, circle_size);
	Rect cut;
	Size2i left_size(frames[0].cols*resize_left, frames[0].rows*resize_left);
	Size2i right_size(frames[2].cols*resize_right, frames[2].rows*resize_right);
	//cv::resize(frames[0], frames[0], left_size);
	//cv::resize(frames[2], frames[2], right_size);
	cv::warpPerspective(frames[0], frames[0], trans_left, frames[0].size());
	for (auto &p : left_markers){
		cut = Rect(p - left_up, p + right_down);
		//rectangle(frames[0], cut, Scalar(255, 0, 0), 2);
		p *= resize_left;
		
		Mat f = frames[0](cut);
		marker_imgs.push_back(f.clone());
		//use vector;;;;;
		
		p += shift_left;
		marker_all_loc.push_back(p);
		marker_ID.push_back(id++);
		
	}
	imwrite(temp_frame_path+"f0.jpg", frames[0]);
	for (auto &p : middle_markers) {
		cut = Rect(p - left_up, p + right_down);
		Mat f = frames[1](cut);
		marker_imgs.push_back(f.clone());

		p += shift_p;
		marker_all_loc.push_back(p);
		marker_ID.push_back(id++);
		//rectangle(frames[1], cut, Scalar(255, 0, 0), 2);
	}
	imwrite(temp_frame_path + "f1.jpg", frames[1]);
	for (auto &p : right_markers){
		cut = Rect(p - left_up, p + right_down);
		//rectangle(frames[2], cut, Scalar(255, 0, 0), 2);
		p *= resize_right;
		
		Mat f = frames[2](cut);
		marker_imgs.push_back(f.clone());
		p += shift_right;
		marker_all_loc.push_back(p);
		marker_ID.push_back(id++);
		
	}

	imwrite(temp_frame_path + "f2.jpg", frames[2]);
	//merge imgs
	int scale = 30;
	Mat left = frames[0], middle, right = frames[2], big;
	/*
	middle = frames[1].clone();

	Size2i big_size(frames[1].cols, frames[1].rows + shift_dis);

	Point2i shift = shift_p + shift_left;
	Mat shift_M = (Mat_<double>(2, 3) << 1, 0, shift.x, 0, 1, shift.y);
	cv::warpAffine(left, left, shift_M, big_size);

	shift_M = (Mat_<double>(2, 3) << 1, 0, 0, 0, 1, shift_dis);
	cv::warpAffine(middle, middle, shift_M, big_size);

	shift = shift_p + shift_right;
	shift_M = (Mat_<double>(2, 3) << 1, 0, shift.x, 0, 1, shift.y);
	cv::warpAffine(right, right, shift_M, big_size);

	cv::addWeighted(left, 0.5, middle, 0.5, 0, big);
	cv::addWeighted(big, 0.7, right, 0.5, 0, big);
	string path = temp_frame_path + to_string(1) + "ratio.jpg";
	cv::imwrite(path, big);
*/

	/*
	for (float j = 0; j < scale; j++) {
		float ratio = 1.0 + j / 100 - scale / 2 / 100.0;
		Size left_new(left_size.width*ratio, left_size.height*ratio);
		Size right_new(right_size.width*ratio, right_size.height*ratio);
		Mat left, middle, right, big;
		cv::resize(frames[0], left, left_new);
		cv::resize(frames[2], right, right_new);
		middle = frames[1].clone();

		Size2i big_size(frames[1].cols, frames[1].rows + shift_dis);
		
		Point2i shift = shift_left + (1-ratio)*(left_markers[0] - (shift_p + shift_left));
		Mat shift_M = (Mat_<double>(2, 3) << 1, 0, shift.x, 0, 1, shift.y);
		cv::warpAffine(left, left, shift_M, big_size);

		shift_M = (Mat_<double>(2, 3) << 1, 0, 0, 0, 1, shift_dis);
		cv::warpAffine(middle, middle, shift_M, big_size);

		shift = shift_right + (1- ratio)*(right_markers[0] -shift_p - shift_right);
		shift_M = (Mat_<double>(2, 3) << 1, 0, shift.x, 0, 1, shift.y);
		cv::warpAffine(right, right, shift_M, big_size);

		cv::addWeighted(left, 0.5, middle, 0.5, 0, big);
		cv::addWeighted(big, 0.7, right, 0.5, 0, big);
		string path = temp_frame_path + to_string(ratio) + "ratio.jpg";
		cv::imwrite(path, big);


	}
	*/

}

void FRAMES::save_initial_markers() {
	string csvpath = "N:/LEFT/temp_data/" + to_string(iffirst) + "_marker_loc.csv";
	ofstream csv;
	csv.open(csvpath);
	for (int i = 0; i < marker_ID.size(); i ++) {
		csv << marker_ID[i] << "," << marker_all_loc[i].x << "," << marker_all_loc[i].y << "\n";
		string imgpath =  "N:/LEFT/temp_data/" + to_string(iffirst) + "_" + to_string(marker_ID[i]) + "_marker.jpg";
		cv::imwrite(imgpath, marker_imgs[i]);
	}
	string parapath =  "N:/LEFT/temp_data/" + to_string(iffirst) + "_shift_trans.csv";
	ofstream fout(parapath);
	csv.close();

	if (!fout)
	{
		cout << "File Not Opened" << endl;  return;
	}

	fout << shift_left.x<<","<<shift_left.y<<","<<shift_right.x<<","<<shift_right.y << endl;
	fout << resize_left << "," << resize_right << endl;
	for (int i = 0; i<trans_left.rows; i++)
	{
		for (int j = 0; j<trans_left.cols; j++)
		{
			fout << trans_left.at<float>(i, j) << ",";
		}
	}

	fout.close();
	//Point2i shift_left, shift_right;
	//Mat trans_left;
}