#include "FRAMES.h"

void FRAMES::load_cir_edge_paras(vector<vector<vector<Point>>> &v_center, vector<vector<int>> &v_fnum, vector<vector<int>> &v_edge, vector<Mat> &frames, string path) {
	vector<string> g_path = { "N:/LEFT/","L:/MIDDLE/","N:/RIGHT/" };
	v_center.resize(3);
	v_fnum.resize(3);
	v_edge.resize(3);
	frames.resize(3);
	double ratio[2][3] = { { 0.497326203, 1, 0.486631016 },{ 0.455782313, 1, 0.469387755 } };
	int order[3] = { 1,9,5 };
	for (int j = 0; j < g_path.size(); j++) {
		
		ratio_frames = ratio[iffirst][j];
		//load_circle_parameters(g_path[j] + path, v_fnum[j], v_center[j], sti, j, 1);
		string edge_path = g_path[j] + "temp_data/" + to_string(iffirst) + "_edge.csv";
		load_circle_parameters(g_path[j] + path, v_fnum[j], v_center[j], j, iffirst, 1);
		EDGE_FIT_ORDER = order[j];
		load_edge_function(v_center[j], edge_path, v_fnum[j], v_edge[j]);
		load_frame(g_path[j] + "imgs/" + to_string(v_fnum[j][0]) + ".jpg", frames[j]);
		Size large_size = frames[j].size() * 2;
		int frame_W = large_size.width, frame_H = large_size.height;
		Mat shift_M = (Mat_<double>(2, 3) << 1, 0, large_size.width / 4, 0, 1, large_size.height / 4);
		cv::warpAffine(frames[j], frames[j], shift_M, large_size);
		
		Size2i resized(frames[j].cols*ratio[iffirst][j], frames[j].rows*ratio[iffirst][j]);
		cv::resize(frames[j], frames[j], resized);
	}

}