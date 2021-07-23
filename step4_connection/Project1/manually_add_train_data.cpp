#include "FRAMES.h"

Mat theframe;
void Cross2(cv::Mat &frame, cv::Point point, cv::Scalar s) {
	int cl = 6;
	cv::Point a(cl, 0), b(-cl, 0), c(0, cl), d(0, -cl);
	line(frame, point + a, point + b, s, 2);
	line(frame, point + c, point + d, s, 2);
}

void Mouseclick3(int evt, int x, int y, int flags, void* edgepoints) {
	cv::Scalar s(0, 0, 255);
	std::vector<cv::Point>* edgep = (std::vector<cv::Point>*)edgepoints;
	if (evt == CV_EVENT_LBUTTONDOWN) {
		Mat frame = theframe.clone();
		edgep->push_back(Point(x, y));
		if (edgep->size() >= 1) {
			for (int i = 0; i * 2 + 1 < edgep->size(); i++)
				rectangle(frame, Rect((*edgep)[i * 2], (*edgep)[i * 2 + 1]), Scalar(255, 100, 0), 3);
			Cross2(frame, edgep->back(), Scalar(0, 0, 255));
		}
		imshow("video", frame);
	}
	if (evt == CV_EVENT_RBUTTONDOWN) {
		Mat frame = theframe.clone();
		edgep->pop_back();
		if (edgep->size() >= 1) {
			for (int i = 0; i * 2 + 1 < edgep->size(); i++)
				rectangle(frame, Rect((*edgep)[i * 2], (*edgep)[i * 2 + 1]), Scalar(255, 100, 0), 3);
			Cross2(frame, edgep->back(), Scalar(0, 0, 255));
		}
		imshow("video", frame);
	}
}

void FRAMES::writedata(string txt_path, Mat img, vector<bbox_t> bbox0) {
	vector<cv::Point> contourpoints;
	cv::setMouseCallback("video", Mouseclick3, &contourpoints);
	double w = img.cols, h = img.rows;
	ofstream txtfile;
	txtfile.open(txt_path);
	
	for (auto i : bbox0){
		rectangle(img, Rect(i.x, i.y, i.w, i.h), Scalar(0, 0, 255), 2);
		//txtfile<< (i.h > truck_len) << " " << (i.x+i.w/2) / w << " " << (i.y+i.h/2) / h << " " << i.w / w << " " << i.h / h << "\n";
	}
	theframe = img.clone();
	imshow("video", img);
reload:
	char c = waitKey(0);
	if (c == 'c') {
		if (contourpoints.size() % 2) {
			cout << "points not correct, plz reload. \n";
			contourpoints.clear();
			goto reload;
		}
		for (int i = 0; i < contourpoints.size(); i += 2) {
			Point2i a = contourpoints[i], b = contourpoints[i + 1];
			Rect r(a, b);
			rectangle(img, Rect(r.x, r.y, r.width, r.height), Scalar(0, 255, 255), 2);
			txtfile << (r.height > truck_len) << " " << (r.x+ r.width/2) / w << " " << (r.y+ r.height/2) / h << " " << r.width / w << " " << r.height / h << "\n";
			for (int j = 0; j < bbox0.size(); j++){
				Rect box(bbox0[j].x, bbox0[j].y, bbox0[j].w, bbox0[j].h);
				if ((box&r).area()>0 || box.area() < 50)
					bbox0.erase(bbox0.begin() + j--);
			}

		}
		for (auto i : bbox0) {
			txtfile << (i.h > truck_len) << " " << (i.x + i.w / 2) / w << " " << (i.y + i.h / 2) / h << " " << i.w / w << " " << i.h / h << "\n";
		}
	}
}