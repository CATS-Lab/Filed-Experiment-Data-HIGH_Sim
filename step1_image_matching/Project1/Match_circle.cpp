
#include "FRAMES.h"

bool FRAMES::match_circles(Mat cur_frame) {
	Size2i size1(100, 100);
	Size2i size2(300, 300);
	Rect rect1, rect2;

	int csize = circle_mat.size();
	vector<Point2i> c_p;
	vector<int> c_r;
	for (int i = 0; i < csize; i++) {
		Point2i p = vector_c_p.back()[i];
		if (p.x < 50 || IMGW - p.x < 50 || IMGH - p.y < 50 || p.y < 50)
			return false;
		int r = vector_c_r.back()[i];
		rect1 = Rect(p.x - size1.width / 2, p.y - size1.height / 2, size1.width, size1.height);
		rect2 = Rect(p.x - size2.width / 2, p.y - size2.height / 2, size2.width, size2.height);
		Mat mat_temp = circle_mat[i];
		Mat mat_sour = cur_frame(rect1).clone();
		Mat mat_temp_depth = circle_mat_depth[i];

		//load cir mat in curframe, load featurepoints 
		//(may need to store feature points of pre_frame,), match both mat and get the location;
		//push locations;
		Point2i result;
		filte_img(mat_sour);
		bool matched = temp_match(mat_temp, mat_temp_depth, mat_sour, result);
		if (!matched) {
			if (p.x < 150 || IMGW - p.x < 150 || IMGH - p.y < 150 || p.y < 150)
				return false;
			mat_sour = cur_frame(rect2).clone();
			filte_img(mat_sour);
			matched = temp_match(mat_temp, mat_temp_depth, mat_sour, result);
			if (!matched) {
				cout << "circle " << i << " is not found" << endl;
				cout << "info: " << vector_c_p.size() << endl;
				show_frame(mat_sour, 5000);
				return false;
			}
			else
				c_p.push_back(result + Point2i(rect2.x, rect2.y));
		}
		else
			c_p.push_back(result + Point2i(rect1.x, rect1.y));

	}
	c_r = circle_rad;
	vector_c_p.push_back(c_p);
	vector_c_r.push_back(c_r);
	return true;
}

void FRAMES::filte_img(Mat &cur_frame) {
	Mat temp1, temp2;
	//cv::cvtColor(cur_frame, temp1, CV_BGR2GRAY);
	temp1 = cur_frame.clone();
	Size2i kernalsize(3, 3);
	cv::GaussianBlur(temp1, temp2, kernalsize, 1, 1, 4);
	cur_frame = temp2.clone();

}

bool FRAMES::temp_match(Mat mat_temp, Mat mat_temp_depth, Mat mat_sour, Point2i &result) {
	/*
	//Add a template
	cv::Ptr<cv::linemod::Detector> detector;
	detector = linemod::getDefaultLINEMOD();
	cv::String class_id = "1";
	//Mat mask = mat_temp;//circle of mat_temp, out of circle or square of vehicle,
	//also need to consider to set scatter points to 1 and others to 0;;
	Mat depth;
	//cv::cvtColor(mat_temp, depth, CV_BGR2GRAY);
	//depth.convertTo(depth, CV_16UC1);

	vector<Mat> temp{ mat_temp, mat_temp_depth };
	Mat mask_temp(mat_temp.size(), CV_8UC1);

	mask_temp.setTo(Scalar(0));
	int rad = mat_temp.size().width / 2;
	if (mat_temp.size().width != mat_temp.size().height) {
	cout << "circle temp size is not square, pls check it.\n";
	return false;
	}
	Point2i center(rad, rad);
	circle(mask_temp, center, rad, Scalar(255), -1);
	show_frame(mask_temp, 3000);

	Rect bordingbox;
	Mat mask_fak;
	int templ_ID = detector->addTemplate(temp, class_id, mask_temp, &bordingbox);// it is -1 not work well pls check it

	vector<cv::linemod::Match> res_match;
	vector<Mat> res_mat;
	const vector<cv::String> class_id_s{ class_id };
	vector<Mat> detect_mat{ mat_sour };
	detector->match(detect_mat, thres_cir, res_match, class_id_s, res_mat);
	*/
	Mat match_mat;
	int match_method = CV_TM_CCOEFF_NORMED;// CV_TM_CCOEFF;
	matchTemplate(mat_temp, mat_sour, match_mat, match_method);
	//TPR->match(d_rect_cmp, d_rect_org, result, stream);
	//cv::normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point2i matchLoc;
	cv::minMaxLoc(match_mat, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = maxLoc;
	float score = maxVal;
	int rad = mat_temp.size().width / 2;
	if (score > 0.8) {
		result = matchLoc + Point2i(rad, rad);

		//test results;
		//show_frame(mat_sour);
		//show_frame(mat_temp);
		//show_frame(mat_sour.clone(), result, rad, 10);

		return true;
	}
	else {
		return false;
	}
}

bool FRAMES::color_match(Mat mat_temp, Mat mat_sour, Point2i result) {


	return true;
}