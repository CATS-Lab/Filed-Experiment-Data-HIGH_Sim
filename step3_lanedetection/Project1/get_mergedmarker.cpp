#include "FRAMES.h"

void FRAMES::get_mergedmarker_loc(vector<Mat> frames, vector<int> edge) {
	Public_func pf;
	vector<string> g_path = { "N:/LEFT/","L:/MIDDLE/","N:/RIGHT/" };
	string markers_path = "N:/LEFT/temp_data/" + to_string(iffirst) + "_marker_loc.csv";
	if (!pf.exists_file(markers_path)){
		load_markers_from_file(markers_path);
	}
	else {
		vector<Point2i> markers_p;
		vector<Point2i> left_markers, right_markers, middle_markers;
		load_left_right_trans_marker(left_markers, right_markers);
		load_middle_markers(frames[1], edge, middle_markers);
 		calculate_load(left_markers, middle_markers, right_markers, frames);
		save_initial_markers();
	}


}