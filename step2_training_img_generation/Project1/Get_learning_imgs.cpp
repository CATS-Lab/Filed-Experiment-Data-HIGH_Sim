#include "FRAMES.h"

void FRAMES::get_learning_imgs(string path, int sti, int stj, int &imgnum, const int W, const int H) {
	vector<vector<Point>> center;
	vector<int> fnum;
	load_circle_parameters(gen_path + path, fnum, center);
	string edge_path = gen_path + "temp_data/" + to_string(stj) + "_edge.csv";
	vector<int> edge;

	for(auto &ca:center)
		for(auto & ci:ca)
			ci = ci + Point(IMGW / 2, IMGH / 2);
	if (sti == 2 && stj == 1)
		for (auto &ca : center){
			if (ca.size() == 4)
				ca.erase(ca.begin() + 2);
		}
	load_edge_function(center, edge_path, fnum, edge);
	Mat frame, frame_shifted, frame_rotated, affine_M;
	for (int i = 0; i < center.size(); i += 500) {
		load_frame(fnum[i], frame);
		Size large_size = frame.size() * 2;
		int frame_W = large_size.width, frame_H = large_size.height;					
		//cv::Mat large_f(frame.size()*2, CV_8UC3, cv::Scalar(0, 0, 0));
		if (center[0].size() > 3)
			affine_M = findHomography(center[i], center[0]);
		else {
			Point2f a0[3] = { center[0][0], center[0][1], center[0][2] };
			Point2f a1[3] = { center[i][0], center[i][1], center[i][2] };
			affine_M = getAffineTransform(a1, a0);
		}
		Mat shift_M = (Mat_<double>(2, 3) << 1, 0, large_size.width / 4, 0, 1, large_size.height / 4);
		cv::warpAffine(frame, frame_shifted, shift_M, large_size);
		if (center[0].size() > 3)
			cv::warpPerspective(frame_shifted, frame_rotated, affine_M, large_size);
		else
			cv::warpAffine(frame_shifted, frame_rotated, affine_M, large_size);
		Size2i resized(frame_rotated.cols*ratio_frames, frame_rotated.rows*ratio_frames);
		cv::resize(frame_rotated, frame_rotated, resized);
		cut_and_save_img(sti, frame_rotated, imgnum, edge, W, H);
		cout << i  << "#";
		//show_frame(frame_shifted, 300000);
		string img_path = gen_path + "temp_frame/" + to_string(fnum[i]) + "_calied.jpg";
		cv::imwrite(img_path, frame_rotated);
	}
}

void FRAMES::cut_and_save_img(int sti, Mat frame_rotated, int &imgnum, vector<int> edge, const int W, const int H) {
	
	int x0 = edge[0];
	int y0 = 0;
	int intend = 200;
	int high = frame_rotated.rows - 1;
	int bot = high;
	int frameW = frame_rotated.cols, frameH = frame_rotated.rows;
	for (y0 = 1; y0 < frame_rotated.rows; y0++) {
		Point p1(edge[y0], y0);
		Point p2(edge[y0]+W, y0);
		if (edge[y0] < W || edge[y0] + W > frameW)
			continue;
		Vec3b cor1 = frame_rotated.at<Vec3b>(p1);
		Vec3b cor2 = frame_rotated.at<Vec3b>(p2);
		if (cor1[0] > 0 || cor2[0] > 0)
			break;
	}

	for (bot = y0; bot < high; bot ++) {
		Point p1(edge[bot], bot);
		Point p2(edge[bot]+W, bot);
		if (p1.x < 800)
			break;
		Vec3b cor1 = frame_rotated.at<Vec3b>(p1);
		Vec3b cor2 = frame_rotated.at<Vec3b>(p2);
		if (cor1[0] == 0 && cor2[0] == 0)
			break;
	}
	//cout << bot << endl;
	//show_frame(frame_rotated, 50000);
	Rect cut;
	while (1) {
		double angle = atan(double(edge[y0 + H] - edge[y0]) / double(H));
		x0 = edge[y0];
		Mat rot_mat = getRotationMatrix2D(Point(x0,y0), -angle*180/PI, 1);
		Mat to_cut;
		cv::warpAffine(frame_rotated, to_cut, rot_mat, frame_rotated.size());
		if(sti == 0)
			cut = Rect(x0 - 350, y0, W, H);
		else if (sti == 1)
			cut = Rect(x0 - 310, y0, W, H);
		else
			cut = Rect(x0 - 200, y0, W, H);
		//rectangle(to_cut, cut, Scalar(0, 255, 0), 12);
		
		
		Mat subimg = to_cut(cut);
		//show_frame(subimg, 500);
		string img_path = "N:/LEFT/learning_imgs/imgs/" + to_string(imgnum++) + ".jpg";
		cv::imwrite(img_path, subimg);
		for (double sum = 0; sum < H; y0++) {
			sum += sqrt(pow(edge[y0 + 1] - edge[y0], 2) + 1);
			if (sum > H - intend)
				break;
		}
		if (y0 >= bot)
			break;
	}
}