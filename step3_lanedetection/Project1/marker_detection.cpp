#include "FRAMES.h"
#include "Poly.h"
void FRAMES::detect_lane_marker(int sti, int stj, string path) {
	//initialize the big image, save lane information and
	vector<vector<vector<Point>>> v_center;
	vector<vector<int>> v_fnum;
	vector<vector<int>> v_edge;
	
	vector<Mat> v_frame;
	load_cir_edge_paras(v_center, v_fnum, v_edge, v_frame, path);
	//get_mergedmarker_loc(v_frame, v_edge[1]);
	string marker_middle_p = temp_data_path + "/connection_" + to_string(stj) + ".csv";
	load_marker_loc(marker_all_loc, marker_middle_p);

	Mat frame, frame_shifted, frame_rotated, affine_M;
	map<int, vector<Rect>> vehs;
	vector<Rect> vrects;
	vector<Point> vp; vector<Mat> vmat; vector<Mat> vmr;
	GpuMat g_frame, g_rotated, g_shifted, g_resized, g_persed;
	vector<int> fnum = v_fnum[sti];
	vector<vector<Point>> center = v_center[sti];
	vector<Mat> cali_mat;
	vector<int> edge = v_edge[sti];

	Mat trans_val, pers2, pers2_trans;
	vector<vector<Point2i>> v_pers;
	vector<Point2i> ori_p, matched_p, forcasted_p, forcasted_p2;
	Point cutp1(12, 30), cutp2(12, 30);
	Point square(80, 80);
	Point ex_1(3, 4), ex_2(3, 26);

	float resize_val;
	Point2i shift_val;
	int shift_dis = 6000;
	ratio_frames = frame_ratios[stj][sti];
	Point2i shift_p(0, shift_dis);
	reverse(marker_all_loc.begin(), marker_all_loc.end());
	int f00 = 0;
	vector<vector<int>> num_loc;
	vector<Mat> trans_res;


	Point2i markersize(8, 20);
	int y0_left = 0;

	for (int i = f00; i < center.size(); i += 1) {
		load_frame(fnum[i], frame);
		Size large_size = frame.size() * 2;
		int frame_W = large_size.width, frame_H = large_size.height;
		cv::Mat large_f(frame.size()*2, CV_8UC3, cv::Scalar(0, 0, 0));
		
		if (center[0].size() > 3){
			Point2f a0[3] = { center[0][0], center[0][1], center[0][2] };
			Point2f a1[3] = { center[i][0], center[i][1], center[i][2] };
			affine_M = getAffineTransform(a1, a0);
			//affine_M = findHomography(center[i], center[0], CV_RANSAC);
		}
		else {
			Point2f a0[3] = { center[0][0], center[0][1], center[0][2] };
			Point2f a1[3] = { center[i][0], center[i][1], center[i][2] };		
			affine_M = getAffineTransform(a1, a0);
		} 
		
		Mat shift_M = (Mat_<double>(2, 3) << 1, 0, large_size.width / 4, 0, 1, large_size.height / 4);
		g_frame.upload(frame);
		cuda::warpAffine(g_frame, g_shifted, shift_M, large_size);
		
		if (center[0].size() > 3){
			cuda::warpAffine(g_shifted, g_rotated, affine_M, large_size);
			//cuda::warpPerspective(g_shifted, g_rotated, affine_M, large_size);
		}
		else
			cuda::warpAffine(g_shifted, g_rotated, affine_M, large_size);
		
		Size2i resized(large_size.width*ratio_frames, large_size.height*ratio_frames);
		cuda::resize(g_rotated, g_resized, resized);
		//cut_and_save_img(sti, frame_rotated, imgnum, edge, W, H);
		//g_rotated.download(frame);
		//imwrite(temp_data_path + "te2xt.jpg", frame);
		
		if ((i-f00) % 1000 == 0) {
			if (ori_p.empty())
				ori_p = marker_all_loc;
			PolynomialRegression<long double> poly;
			vector<long double> vy, vx, f;
			for (int k = 0; k < ori_p.size(); k += 2) {
				Point2i p = ori_p[k];
				vy.push_back(long double(p.y));
				vx.push_back(long double(p.x));
			}
			poly.fitIt(vy, vx, EDGE_FIT_ORDER, f);
			edge.clear();
			for (double y = 0, xi = 100; y < v_edge[sti].size() && xi > 20; y = y + 1.0) {
				xi = 0;
				for (int j = 0; j < EDGE_FIT_ORDER + 1; j++)
					xi += f[j] * pow(y, j);
				edge.push_back(xi);
			}
		}
		if (forcasted_p.empty() || pers2_trans.empty()) {

			Point2i markersize(8, 20);
			slopelen = markersize.y;
			get_sub_imgs(g_resized, marker_all_loc, edge, markersize, marker_img, vrects);

			if (0) {//sti == 
				int num = marker_all_loc.size();
				int interval = 4;
				for (int k = 0, loc = 0; k < (num - 1) * interval/ 2; k++) {
					if (k % interval == 2) {
						
						Point2i p = marker_all_loc[k - 2] + 2.0 / 5.0 * (marker_all_loc[k] - marker_all_loc[k-2]);
						marker_all_loc.insert(marker_all_loc.begin() + k, p);
						marker_img.insert(marker_img.begin() + k, marker_img[k - 2]);
					}
					else if (k % interval == 3){
						Point2i p = marker_all_loc[k - 2] + 2.0 / 5.0 * (marker_all_loc[k+1] - marker_all_loc[k - 2]);
						marker_all_loc.insert(marker_all_loc.begin() + k, p);
						marker_img.insert(marker_img.begin() + k, marker_img[k - 2]);
					}
				}

			}
			forcasted_p = marker_all_loc;
			ori_p = forcasted_p;
			y0_left = forcasted_p[0].y;
		}

		
		//ori_p.clear();
		vector<int> match_bool;
		matched_p.clear();
		int acu = 0;
		//imwrite(temp_data_path + "text.jpg", frame);
		detect_markers(g_resized, forcasted_p,edge, Point2i(14,slopelen*4), ori_p, matched_p, match_bool, acu, vrects);
		acu = 0;
		for (auto b : match_bool)
			acu += b;

		vector<Point2i> points_p;
 		if (acu < 10)
			break;
		int inter = acu;
		int val = 1;
		for (int k = 0, sum = 0, pk = 0; k < match_bool.size(); k++) {
			sum += match_bool[k];
			if (sum / inter == val && (!(sum % inter) || sum / inter == acu / inter)) {
				val++;
				if (sum / inter == acu / inter)
					k = match_bool.size() - 1;
				vector<Point2i> part_ori, part_match;
				double mindis = 0, nearnum = 0, disdet = 6;

				for (int m = pk; m < k + 1; m++) {
					if (match_bool[m]) {
						part_ori.push_back(ori_p[m]);
						part_match.push_back(matched_p[m]);
					}
				}

				vector<Point2f> points(ori_p.begin() + pk, ori_p.begin() + k + 1), points2;
				std::vector<unsigned int> indices(part_ori.size());
				std::iota(indices.begin(), indices.end(), 0);


				for (int k = 0; k < 100; k++) {
					vector<Point2i> i_ori, i_match;
					std::random_shuffle(indices.begin(), indices.end());
					for (int m = 0; m < acu*0.6; m++) {
						i_ori.push_back(part_ori[indices[m]]);
						i_match.push_back(part_match[indices[m]]);
					}
					vector<Point2f> pointsi(part_ori.begin(), part_ori.end());
					Mat pers_trans = cv::findHomography(i_ori, i_match, CV_RANSAC);//, 
					if (pers_trans.empty())
						continue;
					cv::perspectiveTransform(pointsi, pointsi, pers_trans);
					double total_dis = 0, total_near = 0;
					for (int m = 0; m < pointsi.size(); m++) {
						double dis = abs(pointsi[m].x - part_match[m].x) + abs(pointsi[m].y - part_match[m].y);
						total_dis += dis;
						if (dis < disdet)
							total_near++;
					}
					if (total_near > nearnum || (total_near == nearnum && total_dis < mindis)) {
						nearnum = total_near; mindis = total_dis;
						pers2_trans = pers_trans.clone();
					}
				}



				cv::perspectiveTransform(points, points, pers2_trans);
				trans_res.push_back(pers2_trans);
				num_loc.push_back(vector<int>{fnum[i], (int)points.back().y});

				pk = k + 1;
				for (auto p : points)
					points_p.push_back(p);
			}
		}


		forcasted_p = points_p;
		
		v_pers.push_back(forcasted_p);
		
		if ( !(i%100)) {// && i > 680 || i - f00 < 200 || (i< 1000 && 0 == i % 5)
			//merge_with_large_img;
 			g_resized.download(frame);
			for (auto v : vrects) {
				cv::rectangle(frame, v, Scalar(0, 0, 255), 1);
			}
			for(auto p:forcasted_p)
				cv::rectangle(frame, Rect(p-Point(10,10), p+Point(10,10)), Scalar(0, 255, 255), 2);
			for (int k = 100; k < edge.size(); k++)
				cv::rectangle(frame, Rect(Point(edge[k] - 1, k - 1), Point(edge[k] + 1, k + 1)), Scalar(255, 255, 255), 1);
			string imgname = temp_frame_path + to_string(fnum[i]) + "_marker_test.jpg";
			imwrite(imgname, frame);
			
		}
		vrects.clear();
		if (!(i % 3000)) {//
			save_trans_inf(fnum, v_pers, temp_data_path + to_string(fnum[i]) + "_trans.csv");
			save_trans_mat(fnum, temp_data_path + to_string(fnum[i]) + "_trans_mat.csv", num_loc, trans_res);
		}
		cout << fnum[i] << "#";
	}

	save_trans_inf(fnum, v_pers, temp_data_path + to_string(stj) + "_final_trans.csv");
	save_trans_mat(fnum, temp_data_path + to_string(stj) + "_final_mat.csv", num_loc, trans_res);
}


void FRAMES::save_trans_inf(vector<int> fnum, vector<vector<Point2i>> trans_val, string path) {
	ofstream csv;
	csv.open(path.c_str());
	for (int i = 0; i < trans_val.size(); i++) {
		csv << fnum[i] << ",";
		for (int m = 0; m < trans_val[i].size(); m++)
			csv << to_string(trans_val[i][m].x) << "," << to_string(trans_val[i][m].y) << ",";
		csv << "\n";
	}
	csv.close();
}

void FRAMES::save_trans_mat(vector<int> fnum, string path, vector<vector<int>> num_loc, vector<Mat> trans_res) {
	ofstream csv;
	csv.open(path.c_str());
	
	for (int i = 0; i < trans_res.size(); i++) {
		csv << num_loc[i][0] << "," << num_loc[i][1] << ",";
		for (int m = 0; m < trans_res[i].rows; m++)
			for (int n = 0; n < trans_res[i].cols; n++){
				std::stringstream stream;
				stream << std::fixed << std::setprecision(12) << trans_res[i].at<double>(m, n);
				csv << stream.str() << ",";
			}
		csv << "\n";
	}
	csv.close();
}