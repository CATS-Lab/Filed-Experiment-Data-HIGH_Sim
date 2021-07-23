#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/cudabgsegm.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/cudabgsegm.hpp>
#include <opencv2/cudalegacy.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/rgbd/linemod.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <stack>
#include <math.h>
#include <string>
#include <unordered_map>
#include <cmath>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sys/stat.h>
#include <utility>
#include <direct.h>
#include <windows.h>
#include <sys/types.h>

#include "Public_functions.h"

using namespace cv;
using namespace std;
using namespace cuda;
using namespace cv::xfeatures2d;
#define PI 3.14159265

class FRAMES
{
public:
	//ori mat and circle points updated every 1000 frames;
	vector<Mat> circle_mat;
	vector<Mat> circle_mat_depth;
	vector<Point2i> circle_point;
	vector<int> circle_rad;

	//instore all circle points
	vector<vector<Point2i>> vector_c_p;
	vector<vector<int>> vector_c_r;//radius of circles, may useless;
	//framenum;
	vector<int> vfnum;
	string gen_path;
	string frame_path;
	string output_data_path;
	string temp_data_path;
	string temp_frame_path;

	float thres_cir = 0.8;

	Public_func PF;

	const int IMGW = 4320, IMGH = 8192;

public:
	FRAMES() {};
	FRAMES(string g_path);
	void get_motion_parameters(int startframenumber, int freq);
	void test_circles(string path);
	void test_angle(string path);

private:

	bool load_frame(int fnum, Mat &cur_frame);
	bool load_depth(int fnum, Mat &cur_depth);
	void save_results(Mat cur_frame, int, int, int);
	void load_circle(Mat cur_frame, Mat cur_depth, int);
	bool match_circles(Mat cur_frame);
	void push_points();
	bool temp_match(Mat mat_temp, Mat mat_temp_depth, Mat mat_sour, Point2i &result);
	void filte_img(Mat &cur_frame);
	void show_frame(Mat frame, int duration = 1000, string name = "frame", int startx = 10, int starty = 10);
	void show_frame(Mat frame, Point2i p, int rad, int duration = 1000, string name = "frame", int startx = 10, int starty = 10);
	void show_frame(Mat frame, Rect rect, int duration = 1000, string name = "frame", int startx = 10, int starty = 10);
	bool color_match(Mat mat_temp, Mat mat_sour, Point2i result);
	//test circle matching results;
	void load_circle_parameter(string path);
	void load_angle_parameters(string path, vector<int> &fnum, vector<Point> &center, vector<float> &angle);
};

