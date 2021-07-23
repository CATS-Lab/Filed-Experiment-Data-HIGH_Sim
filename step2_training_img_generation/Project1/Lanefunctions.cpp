#include "FRAMES.h"
#include "Poly.h"
#include "Lanes.h"
#include "Public_functions.h"

void LaneS::loadlanenum() {
	string file = path + "/lanenum.txt";
	string lanenum_s;
	char c;
	Public_func PB;
	if (PB.exists_file(file)) {
		ifstream input(file);
		getline(input, lanenum_s);
		lanenum = atoi(lanenum_s.c_str());
	}
	else {
		cout << "lanenumtxt not exist.\n";
		getline(cin, lanenum_s);
		lanenum = atoi(lanenum_s.c_str());
		std::ofstream outfile(file);
		outfile << to_string(lanenum) << std::endl;
		outfile.close();
	}
	for (int i = 0; i < lanenum; i++) {
		vector<float> vec;
		vehpointx.push_back(vec);
		vehpointy.push_back(vec);
	}
}

void LaneS::loadorderandpoints() {

	loadlanenum();
	order = 2;
	string file = path + "/lanes.txt";
	Public_func PB;
	if (PB.exists_file(file)) {//
		ifstream input(file);
		for (int j = 0; j < lanenum; j++) {
			
			vector<float> coefs;
			string ys;
			float y;
			int i = 0;
			while (i++ < order + 1) {
				getline(input, ys);
				y = atof(ys.c_str());
				coefs.push_back(y);
			}
			fy.push_back(coefs);
		}
	}
	else {
		cout << "file not found\n";
	}
	
}

void LaneS::findlanenum(Veh * vehi) {
	float x, y;
	int cutH = 2000;
	if (vehi->pv != NULL && vehi->y > cutH){
		vehi->lnum = vehi->pv->lnum;
		return;
	}
	
	x = vehi->cx;
	y = vehi->cy;
	
	
	vector<float> xdet;;
	for (int i = 0; i < lanenum; i++) {
		float xi = fy[i][0] + fy[i][1] * y + fy[i][2] * y*y;
		xdet.push_back(abs(x - xi));
	}
	int output = 0;
	float dis0 = 10;
	float dis = dis0;
	for (int i = 0; i < lanenum; i++) {
		if (xdet[i] < dis) {
			output = i;
			dis = xdet[i];
		}
	}
	if(dis < dis0){
		vehi->lnum = output;
		if(dis < 3){
			vehpointx[output].push_back(x);
			vehpointy[output].push_back(y);
		}
	}
	else {
		vehi->lnum = -1;
	}

}

LaneS::LaneS() {

}

LaneS::~LaneS() {

}

void LaneS::regresslanes() {
	for (int i = 0; i < lanenum; i++) {
		if (vehpointx[i].size() < 200 || vehpointy[i].size() != vehpointx[i].size())
			continue;
		PolynomialRegression<float> poly;
		poly.fitIt(vehpointy[i], vehpointx[i], order, fy[i]);
	}
}

void LaneS::showlane(Mat frame) {
	string windowname = "Lanes";
	namedWindow(windowname, WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow(windowname, 5, 5);
	cv::resizeWindow(windowname, 384 * 4, 216 * 4);
	imshow(windowname, frame);
	for(float y = 0; y < 8190; y = y + 1.0)
	for (int i = 0; i < lanenum; i++){
		float xi = fy[i][0] + fy[i][1] * y + fy[i][2] * y*y;
		cv::circle(frame, Point(xi, y), 2, Scalar(0, 0, 60 * i), 1);
	}
	imshow(windowname, frame);
	waitKey(200);
	destroyWindow(windowname);	
}