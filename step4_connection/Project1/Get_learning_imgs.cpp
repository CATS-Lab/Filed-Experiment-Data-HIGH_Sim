#include "FRAMES.h"

void FRAMES::get_learning_imgs(string path, int sti, int stj, int &imgnum, const int W, const int H) {
	vector<vector<Point>> center;
	vector<int> fnum;
	load_circle_parameters(gen_path + path, fnum, center, sti, stj, 0);
	string edge_path = gen_path + "temp_data/" + to_string(stj) + "_edge.csv";
	vector<int> edge;

	for (auto &ca : center)
		for (auto & ci : ca)
			ci = ci + Point(IMGW / 2, IMGH / 2);
	if (sti == 2 && stj == 1)
		for (auto &ca : center) {
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
		cout << i << "#";
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
		Point p2(edge[y0] + W, y0);
		if (edge[y0] < W || edge[y0] + W > frameW)
			continue;
		Vec3b cor1 = frame_rotated.at<Vec3b>(p1);
		Vec3b cor2 = frame_rotated.at<Vec3b>(p2);
		if (cor1[0] > 0 || cor2[0] > 0)
			break;
	}

	for (bot = y0; bot < high; bot++) {
		Point p1(edge[bot], bot);
		Point p2(edge[bot] + W, bot);
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
		Mat rot_mat = getRotationMatrix2D(Point(x0, y0), -angle * 180 / PI, 1);
		Mat to_cut;
		cv::warpAffine(frame_rotated, to_cut, rot_mat, frame_rotated.size());
		if (sti == 0)
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


void FRAMES::get_cut_paras(int sti, GpuMat g_rotated, vector<int> edge, vector<Point> &vp, vector<Mat> &vmat, vector<Mat> &vmr, const int W, const int H) {
	vp.clear();
	vmr.clear();
	vmat.clear();

	int x0 = edge[0];
	int y0 = 0;
	int intend = 200;
	int high = g_rotated.rows - 1;
	int bot = high;
	int frameW = g_rotated.cols, frameH = g_rotated.rows;
	Mat frame_rotated;
	g_rotated.download(frame_rotated);
	for (y0 = 1; y0 < g_rotated.rows; y0++) {
		Point p1(edge[y0], y0);
		Point p2(edge[y0] + W, y0);
		if (edge[y0] < W || edge[y0] + W > frameW)
			continue;
		Vec3b cor1 = frame_rotated.at<Vec3b>(p1);
		Vec3b cor2 = frame_rotated.at<Vec3b>(p2);
		if (cor1[0] > 0 || cor2[0] > 0)
			break;
	}

	for (bot = y0; bot < high; bot++) {
		Point p1(edge[bot], bot);
		Point p2(edge[bot] + W, bot);
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

	Point2i s_p(H*1.25, H*1.25);
	Size2i s_size(s_p.x * 2, s_p.y * 2);
	Rect big(0, 0, frameW, frameH);
	while (1) {
		double angle = atan(double(edge[y0 + H] - edge[y0]) / double(H));
		x0 = edge[y0];
		Point rot_p(x0, y0);
		Rect outer(rot_p - s_p, rot_p + s_p);
		if ((outer&big).area() == outer.area()) {
			int x0p = x0 - outer.x, y0p = y0 - outer.y;
			Mat rot_mat = getRotationMatrix2D(Point(x0p, y0p), -angle * 180 / PI, 1);
			GpuMat g_cut;
			Mat to_cut;
			if (sti == 0)
				cut = Rect(x0p - 350, y0p, W, H);
			else if (sti == 1)
				cut = Rect(x0p - 310, y0p, W, H);
			else
				cut = Rect(x0p - 200, y0p, W, H);
			//rectangle(to_cut, cut, Scalar(0, 255, 0), 12);
			GpuMat g_rotated_p = g_rotated(outer);
			cuda::warpAffine(g_rotated_p, g_cut, rot_mat, s_size);
			g_cut.download(to_cut);
			Mat subimg = to_cut(cut);
			//show_frame(subimg, 500);
			vmat.push_back(subimg);
			//cv::warpAffine(frame_rotated, to_cut, rot_mat, frame_rotated.size());
			rot_mat = getRotationMatrix2D(Point(x0, y0), +angle * 180 / PI, 1);
			vmr.push_back(rot_mat);
			vp.push_back(Point(cut.x + outer.x, cut.y + outer.y));
			//show_frame(subimg, 50000);
		}
		else {
			Mat rot_mat = getRotationMatrix2D(Point(x0, y0), -angle * 180 / PI, 1);
			GpuMat g_cut;
			Mat to_cut;
			if (sti == 0)
				cut = Rect(x0 - 350, y0, W, H);
			else if (sti == 1)
				cut = Rect(x0 - 310, y0, W, H);
			else
				cut = Rect(x0 - 200, y0, W, H);
			//rectangle(to_cut, cut, Scalar(0, 255, 0), 12);

			cuda::warpAffine(g_rotated, g_cut, rot_mat, frame_rotated.size());
			g_cut.download(to_cut);
			Mat subimg = to_cut(cut);

			vmat.push_back(subimg);
			//cv::warpAffine(frame_rotated, to_cut, rot_mat, frame_rotated.size());
			invertAffineTransform(rot_mat, rot_mat);
			vmr.push_back(rot_mat);
			vp.push_back(Point(cut.x, cut.y));
		}

		for (double sum = 0; sum < H; y0++) {
			sum += sqrt(pow(edge[y0 + 1] - edge[y0], 2) + 1);
			if (sum > H - intend)
				break;
		}
		if (y0 >= bot)
			break;
	}
}



void FRAMES::get_cut_paras(int sti, Mat rotated, vector<int> edge, vector<Point> &vp, vector<Mat> &vmat, vector<Mat> &vmr, const int W, const int H) {
	vp.clear();
	vmr.clear();
	vmat.clear();

	int x0 = edge[0];
	int y0 = 0;
	int intend = 200;
	int high = rotated.rows - 1;
	int bot = high;
	int frameW = rotated.cols, frameH = rotated.rows;
	for (int i = 1000; i < 15000; i++)
		circle(rotated, Point(edge[i], i), 4, Scalar(0, 0, 0), 2);
	imwrite(temp_frame_path + "verify_edge.jpg", rotated);

	for (y0 = 1; y0 < rotated.rows; y0++) {
		Point p1(edge[y0], y0);
		Point p2(edge[y0] + W, y0);
		if (edge[y0] < W || edge[y0] + W > frameW)
			continue;
		Vec3b cor1 = rotated.at<Vec3b>(p1);
		Vec3b cor2 = rotated.at<Vec3b>(p2);
		if (cor1[0] > 0 || cor2[0] > 0)
			break;
	}

	for (bot = y0; bot < high; bot++) {
		Point p1(edge[bot], bot);
		Point p2(edge[bot] + W, bot);
		if (p1.x < 800)
			break;
		Vec3b cor1 = rotated.at<Vec3b>(p1);
		Vec3b cor2 = rotated.at<Vec3b>(p2);
		if (cor1[0] == 0 && cor2[0] == 0)
			break;
	}
	//cout << bot << endl;
	//show_frame(frame_rotated, 50000);
	Rect cut;

	Point2i s_p(H*1.25, H*1.25);
	Size2i s_size(s_p.x * 2, s_p.y * 2);
	Rect big(0, 0, frameW, frameH);
	while (1) {
		double angle = atan(double(edge[y0 + H] - edge[y0]) / double(H));
		x0 = edge[y0];
		Point rot_p(x0, y0);
		Rect outer(rot_p - s_p, rot_p + s_p);
		if ((outer&big).area() == outer.area()) {
			int x0p = x0 - outer.x, y0p = y0 - outer.y;
			Mat rot_mat = getRotationMatrix2D(Point(x0p, y0p), -angle * 180 / PI, 1);
			Mat g_cut;
			Mat to_cut;
			if (sti == 0)
				cut = Rect(x0p - 350, y0p, W, H);
			else if (sti == 1)
				cut = Rect(x0p - 310, y0p, W, H);
			else
				cut = Rect(x0p - 200, y0p, W, H);

			
			Mat rotated_p = rotated(outer);
			cv::warpAffine(rotated_p, g_cut, rot_mat, s_size);
			
			Mat subimg = g_cut(cut);
			//show_frame(g_cut, 50000);
			vmat.push_back(subimg);
			//cv::warpAffine(frame_rotated, to_cut, rot_mat, frame_rotated.size());
			rot_mat = getRotationMatrix2D(Point(x0, y0), +angle * 180 / PI, 1);
			vmr.push_back(rot_mat);
			vp.push_back(Point(cut.x + outer.x, cut.y + outer.y));
			//show_frame(subimg, 50000);
		}
		else {
			Mat rot_mat = getRotationMatrix2D(Point(x0, y0), -angle * 180 / PI, 1);
			Mat g_cut;
			if (sti == 0)
				cut = Rect(x0 - 350, y0, W, H);
			else if (sti == 1)
				cut = Rect(x0 - 310, y0, W, H);
			else
				cut = Rect(x0 - 200, y0, W, H);
			//rectangle(to_cut, cut, Scalar(0, 255, 0), 12);

			cv::warpAffine(rotated, g_cut, rot_mat, rotated.size());
			Mat subimg = g_cut(cut);

			vmat.push_back(subimg);
			//cv::warpAffine(frame_rotated, to_cut, rot_mat, frame_rotated.size());
			invertAffineTransform(rot_mat, rot_mat);
			vmr.push_back(rot_mat);
			vp.push_back(Point(cut.x, cut.y));
		}

		for (double sum = 0; sum < H; y0++) {
			sum += sqrt(pow(edge[y0 + 1] - edge[y0], 2) + 1);
			if (sum > H - intend)
				break;
		}
		if (y0 >= bot)
			break;
	}
}