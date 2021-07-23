
#include "FRAMES.h"

Mat ld_cir_mat;

void Cross_circle(cv::Mat frame, cv::Point point, cv::Scalar s) {
	int cl = 30;
	cv::Point a(cl, 0), b(-cl, 0), c(0, cl), d(0, -cl);
	line(frame, point + a, point + b, s, 9);
	line(frame, point + c, point + d, s, 9);
}

void Mouseclick_ld_cir(int evt, int x, int y, int flags, void* cir_points) {
	cv::Scalar click_cor(0, 0, 255);
	cv::Scalar choose_cor(255, 0, 0);
	std::vector<cv::Point>* cir_p = (std::vector<cv::Point>*)cir_points;
	Mat temp_mat = ld_cir_mat.clone();
	Mat temp_mat_cho = ld_cir_mat.clone();
	string temp_mat_name = "temp_mat_name";
	if (evt == CV_EVENT_LBUTTONDOWN) {
		cir_p->push_back(Point(x, y));
		Cross_circle(temp_mat_cho, cir_p->back(), choose_cor);
		imshow(temp_mat_name, temp_mat_cho);
		waitKey(10);
		//Cross_circle(temp_mat, Point(x, y), click_cor);
		//imshow(temp_mat_name, temp_mat);
		//waitKey(10);
	}
	else if (evt == CV_EVENT_RBUTTONDOWN) {
		cir_p->push_back(Point(x, y));
		for (auto cirp : *cir_p) {
			Cross_circle(temp_mat_cho, cirp, choose_cor);
		}
		imshow(temp_mat_name, temp_mat_cho);
		waitKey(10);
	}
}

void FRAMES::load_circle(Mat cur_frame, Mat cur_depth) {
	string winname = "temp_mat_name";
	int window_h = 1080, window_w = window_h*cur_frame.size().width / cur_frame.size().height;
	vector<cv::Point2i> v_cir_p;
	vector<int> v_cir_r;
	cv::Scalar blue(255, 0, 0), green(0, 255, 255), red(0, 0, 255);

	vector<cv::Point2i> v_p_updated;

	int new_winsize = 800, imgsize = 600;
	Mat subimg;
	Rect rect_cut;
	Point2i rect_p,cir_p;
	int cir_r;
	Size2i rect_s;
	int temp;

	ld_cir_mat = cur_frame.clone();
	namedWindow(winname, WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow(winname, 0, 0);
	cv::resizeWindow(winname, window_h, window_w);
	cv::setMouseCallback(winname, Mouseclick_ld_cir, &v_cir_p);

	imshow(winname, ld_cir_mat);
	char key = (char)waitKey(10);
	while (key != 'q')
		key = (char)waitKey(10);
	cv::resizeWindow(winname, new_winsize, new_winsize);
	temp = v_cir_p.size();
	//show_frame(cur_frame, 3000);/////
	for (int i = 0; i < temp; i++) {
		ld_cir_mat = cur_frame.clone();
		rect_s = Size2i(imgsize, imgsize);
		rect_p = v_cir_p[i] - Point2i(imgsize/2,imgsize/2);
		rect_cut = Rect(rect_p, rect_s);
		subimg = ld_cir_mat(rect_cut);
		v_p_updated.push_back(rect_p);
		cir_r = 4;
		cv::circle(subimg, v_cir_p[i] - rect_p, cir_r, blue, 2);
		ld_cir_mat = subimg.clone();
		imshow(winname, subimg);
		key = (char)waitKey(0);
		while (key != 'e') {
			//find radius
			if (key == 'w')
				cir_r++;
			cv::circle(subimg, v_cir_p.back(), cir_r, blue, 2);
			imshow(winname, subimg);
			key = (char)waitKey(10);
		};
		v_cir_r.push_back(cir_r);
	}
	//show_frame(cur_frame, 3000);/////
	v_cir_p.erase(v_cir_p.begin(), v_cir_p.begin() + temp);
	for(int i = 0; i < temp; i ++)
		v_p_updated[i] = v_p_updated[i] + v_cir_p[i];

	v_cir_p = v_p_updated;

	//load mat;
	circle_mat.clear();
	for (int i = 0; i < temp; i++) {
		Point2i Left_up_p = v_cir_p[i] - Point2i(v_cir_r[i], v_cir_r[i]);
		Size2i size(v_cir_r[i]*2, v_cir_r[i]*2);
		Rect rect(Left_up_p, size);
		subimg = cur_frame(rect);
		filte_img(subimg);
		//add show
		
		circle_mat.push_back(subimg);
		Mat subimg_depth = cur_depth(rect);
		//cv::cvtColor(subimg, subimg_depth, COLOR_RGB2BGR);
		circle_mat_depth.push_back(subimg_depth);
		//show_frame(cur_frame, 3000);/////
	}
	circle_point = v_cir_p;
	circle_rad = v_cir_r;
	//add show
	vector_c_p.push_back(circle_point);
	vector_c_r.push_back(circle_rad);
	cv::destroyWindow(winname);
	/*
	for (int i = 0; i < temp; i++) {
		show_frame(circle_mat[i], Point2i(circle_rad[i], circle_rad[i]), circle_rad[i], 500);
		show_frame(cur_frame, circle_point[i], circle_rad[i], 5000);
		show_frame(circle_mat_depth[i], 500);
	}*/
	//show_frame(circle_mat[0], 3000);
	
}