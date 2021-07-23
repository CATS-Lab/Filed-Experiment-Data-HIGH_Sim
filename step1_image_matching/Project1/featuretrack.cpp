#include "myVideo.h"
#include "Veh.h"

Mat edgeground;

void Cross(cv::Mat frame, cv::Point point, cv::Scalar s) {
	int cl = 10;
	cv::Point a(cl, 0), b(-cl, 0), c(0, cl), d(0, -cl);
	line(frame, point + a, point + b, s, 4);
	line(frame, point + c, point + d, s, 4);
}

void Mouseclick1(int evt, int x, int y, int flags, void* edgepoints) {
	cv::Scalar s(0, 0, 255);
	std::vector<cv::Point>* edgep = (std::vector<cv::Point>*)edgepoints;
	if (evt == CV_EVENT_LBUTTONDOWN) {
		edgep->push_back(Point(x, y));
		if (edgep->size() >= 1) {
			Point tem = edgep->back();
			Cross(edgeground, tem, s);
		}
		imshow("Background", edgeground);
	}
}

Rect myVideo::addedge() {
	vector<cv::Point> edgepoints;
	edgeground = edgebackground.clone();
	namedWindow("Background", WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow("Background", 50, 50);
	cv::resizeWindow("Background", 1500, 750);
	cv::setMouseCallback("Background", Mouseclick1, &edgepoints);
	imshow("Background", edgeground);
	char c = waitKey(0);
	Rect output;
	
	if (c == ' ') {
		if (edgepoints.size() > 3) {
			vector<Point> outpoints(edgepoints.end() - 4, edgepoints.end());
			output = boundingRect(outpoints);
		}
			
		else {
			int x = 0.1 * edgeground.cols, y = 0.1 * edgeground.rows;
			int w = 0.8 * edgeground.cols, h = 0.8 * edgeground.rows;
			output = Rect(x, y, w, h);
			cout << "x" << x << "y" << y << "w" << w << "h" << h;
		}
	}
	rectangle(edgeground, output, Scalar(255), 5, 6, 0);
 	imshow("Background", edgeground);
	c = waitKey(3000);
	destroyAllWindows();
	return output;
}

bool myVideo::inEdge(Veh* vehi) {
	return edge.contains(Point(vehi->cx, vehi->cy));
}


void myVideo::checkVehicles(vector<vector<Veh*> > &V) {
	Veh * temp;
	edge = addedge();
	vector<Veh*> undetectnv;
	vector<Veh*> undetectpv;
	Veh * vehi;
	for (int i = 100; i < V.size(); i++) {
		for (int j = 0; j < V[i].size(); j++) {
			vehi = V[i][j];
			if (vehi->nv == NULL || vehi->pv == NULL) {
				if (inEdge(vehi)) {
					if (vehi->nv == NULL) {
						undetectnv.push_back(vehi);
					}
					else {
						undetectpv.push_back(vehi);
					}
				}
			}
		}
	}/*
	for (auto i : undetectpv) {
		GpuMat d_frame, d_frame_rot;
		Mat frame;
		int count = i->ct;
		cap.set(CV_CAP_PROP_POS_FRAMES, OPERFRAMENUM1 + i->ct);
		cap.read(frame);
		int rotnum = int(floor(count / ROTFRQ));
		rotnum = rotnum > 1 ? rotnum - 1 : 0;
		float * rotp;
		rotp = rotpara[rotnum];
		d_frame.upload(frame);
		RotateFrame(d_frame, d_frame_rot, rotp, count);
		d_frame_rot.download(frame);
		rectangle(frame, i->rect, Scalar(0, 225, 0), 8);
		Showframe("undetected", frame);
	}*/
	int count = 1;
	/*for (int i = 0; i < 1; i ++){// undetectnv.size(); i++) {
		Veh* vehi = undetectnv[i];
		Findfeature(vehi);
		while (inEdge(vehi) && count) {
			count = vehi->ct + 1;
			findVeh(count, vehi);
			V[count].push_back(vehi);
			cout << count<<"-";
		}
	}*/
	for (int i = 0; i < 1; i++){// undetectpv.size(); i++) {
		Veh* vehi = undetectpv[i];
		Findfeature(vehi);
		while (inEdge(vehi) && count < OPERFRAMENUM) {
			count = vehi->ct - 1;
			findVeh(count, vehi);
			V[count].push_back(vehi);
			cout << count << "-";
		}
	}
}

bool myVideo::findVeh(int count, Veh* &vehi) {
	Mat frame;
	cap.set(CV_CAP_PROP_POS_FRAMES, OPERFRAMENUM1 + count);
	cap.read(frame);
	int best[2] = { 0,0 };
	int score, bestscore = 0;
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 20; j++) {
			score = getScore(i, j, frame, vehi); 
			if (score > bestscore) {
				bestscore = score;
				best[0] = i;
				best[1] = j;
			}
		}
	}
	refreshveh(frame, vehi, best[0], best[1], count - vehi->ct);
	Showframe("vehi", frame(vehi->rect), 200, 200, 200, 200);
	return 1;
}

bool myVideo::Findfeature(Veh * vehi) {
	// initialize detector and extractor
	cv::
	Mat frame;
	cap.set(CV_CAP_PROP_POS_FRAMES, OPERFRAMENUM1 + vehi->ct);
	cap.read(frame);
	Mat framec = frame(vehi->rect);
	//cv::Ptr<cv::xfeatures2d::SURF> sift = cv::xfeatures2d::SURF::create();// (0, 4, 0.04, 10, 1.6);  
	//Ptr<cv::FastFeatureDetector> fast = cv::FastFeatureDetector::create();
	Size size = framec.size();
	Mat gray(size, CV_8UC1);
	cv::cvtColor(framec, gray, CV_BGR2GRAY);
	Ptr<cv::ORB> orb = cv::ORB::create(500, 1.2f, 8, 10, 0, 2, cv::ORB::FAST_SCORE, 10);
	orb->detect(gray, vehi->key);
	orb->compute(gray, vehi->key, vehi->des);
	//sift->detect(gray, vehi->key);
	//sift->compute(gray, vehi->key, vehi->des);
	//fast->detect(framec, vehi->key);
	return 1;
}

bool myVideo::Findfeature(Mat rect, vector<KeyPoint> &key, Mat &des) {
	Size size = rect.size();
	Mat gray(size, CV_8UC1);
	cv::cvtColor(rect, gray, CV_BGR2GRAY);
	Ptr<cv::ORB> orb = cv::ORB::create(500,1.2f,8, 10,0,2,cv::ORB::FAST_SCORE, 10);
	orb->detect(gray, key);
	orb->compute(gray, key, des);
	return 1;
}

void myVideo::refreshveh(Mat frame, Veh* &vehi, int i, int j, int sign) {
	Veh* vehnew = new Veh(vehi, sign, i, j);
	//vehnew->rect += Point(i - 10, j - 10);
	Findfeature(frame(vehnew->rect), vehnew->key, vehnew->des);
	if (sign > 0) {
		vehnew->pv = vehi;
		vehi->nv = vehnew;
	}
	else {
		vehnew->nv = vehi;
		vehi->pv = vehnew;
	}
	vehi = vehnew;
}


int myVideo::getScore(int i, int j, Mat frame, Veh* vehi) {
	Rect recti = vehi->rect + Point(i - 10, j - 10);
	Mat testmat = frame(recti);
	vector<KeyPoint> key;
	Mat des;
	Findfeature(testmat, key, des);
	int score = 0;
	BFMatcher matcher(cv::NORM_HAMMING, true);
	vector<DMatch> matches, good_matches;
	matcher.match(vehi->des, des, matches); 
	for (int i = 0; i < matches.size(); i++) {
		if (matches[i].distance < 100) {
			score++;
			//good_matches.push_back(matches[i]);
		}
	}
	/*
	Mat img_matches;
	drawMatches(frame(vehi->rect), vehi->key, testmat, key,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>());
	Showframe("matches", img_matches, 20);
	cout << "i" << i << "j" << j << endl;*/
	return score;
}