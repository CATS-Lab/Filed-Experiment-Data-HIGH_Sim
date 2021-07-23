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
#include <opencv2/calib3d/calib3d.hpp>
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
#include <numeric> 
#include <sys/types.h>

#include "Public_functions.h"
#include "yolo_v2_class.hpp"

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
	float ratio_frames;
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
	int EDGE_FIT_ORDER = 5;
	vector<vector<double>> frame_ratios = { { 0.497326203, 1, 0.486631016 },{ 0.455782313, 1, 0.469387755 } };

	float thres_cir = 0.8;

	Public_func PF;

	const int IMGW = 4320, IMGH = 8192;

	const int truck_len = 65;

	Point2i shift_left, shift_right;
	Mat trans_left;
	float resize_left, resize_right;
	vector<Point2i> marker_all_loc;
	vector<Mat> marker_imgs;
	vector<GpuMat> marker_img;
	vector<int> marker_ID;
	int iffirst;
	int slopelen;


public:
	FRAMES() {};
	FRAMES(string g_path);
	void get_motion_parameters();
	void test_circles(string path);
	void test_angle(string path, bool st1);
	void get_learning_imgs(string path, int st1, int , int&, const int, const int);
	//generate learning data
	void generate_learning_data(string imgpath, string last_cnn_path, string cnn_path, string, int);

	void load_circle_parameters(string path, vector<int> &fnum, vector<vector<Point>> &centers, int, int, int);
	void FRAMES::load_edge_function(vector<vector<Point>> center, string edge_path, vector<int> fnum, vector<int> &output);
	void FRAMES::load_edge_from_frame(Mat frame_merged, vector<cv::Point> &points);
	void FRAMES::load_edge_from_file(string edge_path, vector<Point> &points);

	void FRAMES::save_veh_inf(map<int, vector<Rect>> vehs, string output_path);
	void FRAMES::detect_vehs(string path, string cnn_path, int sti, int stj, int &imgnum, const int W, const int H);
	void FRAMES::get_cut_paras(int sti, GpuMat  frame_rotated, vector<int> edge, vector<Point> &vp, vector<Mat> &vmat, vector<Mat> &vmr, const int W, const int H);
	void FRAMES::get_cut_paras(int sti, Mat rotated, vector<int> edge, vector<Point> &vp, vector<Mat> &vmat, vector<Mat> &vmr, const int W, const int H);
	void FRAMES::load_cir_edge_paras(vector<vector<vector<Point>>> &v_center, vector<vector<int>> &v_fnum, vector<vector<int>> &v_edge, vector<Mat> &frames, string path);
	void FRAMES::detect_lane_marker(int sti, int stj, string);
	void FRAMES::load_markers_from_file(string );
	void FRAMES::load_left_right_trans_marker(vector<Point2i> &left_p, vector<Point2i> &right_p);
	void FRAMES::load_marker_loc(vector<Point2i> &markers_p, string path_marker);
	void FRAMES::get_mergedmarker_loc(vector<Mat> frames, vector<int> edge);
	void FRAMES::load_middle_markers(Mat frame, vector<int> edge, vector<Point2i> &right_markers);
	void FRAMES::calculate_load(vector<Point2i> left_markers, vector<Point2i> middle_markers, vector<Point2i> right_markers, vector<Mat> frames);
	void FRAMES::save_initial_markers();

	void FRAMES::save_trans_inf(vector<int> fnum, vector<vector<Point2i>> trans_val, string path);
	void FRAMES::get_sub_imgs(GpuMat gmat, vector<Point2i> locs, vector<int> edge, Point2i size, vector<GpuMat> &output, vector<Rect> &vrects);
	void FRAMES::detect_markers(GpuMat gmat, vector<Point2i> &forcasted_p, vector<int> edge, Point2i size, vector<Point2i> &ori_p, vector<Point2i> &matched_p, vector<int> &match_bool, int &acu, vector<Rect> &);
	void FRAMES::save_trans_mat(vector<int> fnum, string path, vector<vector<int>>, vector<Mat> trans_res);

private:

	bool load_frame(int fnum, Mat &cur_frame);
	bool load_depth(int fnum, Mat &cur_depth);
	void save_results(Mat cur_frame, int);
	void load_circle(Mat cur_frame, Mat cur_depth);
	bool match_circles(Mat cur_frame);
	void push_points();
	bool temp_match(Mat mat_temp, Mat mat_temp_depth, Mat mat_sour, Point2i &result);
	void filte_img(Mat &cur_frame);
	void show_frame(Mat frame, int duration = 1000, string name = "frame", int startx = 10, int starty = 10);
	void show_frame(Mat frame, Point2i p, int rad, int duration = 1000, string name = "frame", int startx = 10, int starty = 10);
	void show_frame(Mat frame, Rect rect, int duration = 1000, string name = "frame", int startx = 10, int starty = 10);
	bool color_match(Mat mat_temp, Mat mat_sour, Point2i result);
	//test circle matching results;
	//void load_circle_parameter(string path);
	void load_angle_parameters(string path, vector<int> &fnum, vector<Point> &center, vector<float> &angle);
	//generate learning imgs
	void cut_and_save_img(int, Mat frame_rotated, int &imgnum, vector<int> edge, const int W, const int H);
	//create learning data
	void writedata(string txt_path, Mat img, vector<bbox_t> bbox0);
	bool load_frame(string frame_path, Mat &cur_frame);

};

