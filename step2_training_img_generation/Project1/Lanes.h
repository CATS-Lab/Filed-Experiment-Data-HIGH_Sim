#pragma once
#include "FRAMES.h"
#include "Poly.h"

class LaneS {
public:
	vector<vector<float>> vehpointx;
	vector<vector<float>> vehpointy;

	int order;
	int lanenum;
	float time0;
	vector<vector<float>> fy;
	Mat frame_0;
	string path;


public:
	LaneS();
	~LaneS();

	void loadlanenum();
	void loadorderandpoints();
	void regresslanes();
	void findlanenum(Veh * v);
	void showlane(Mat frame);
};
