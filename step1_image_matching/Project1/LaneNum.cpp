//****************
#include "myVideo.h"
#include "Veh.h"

void LaneNum(vector<vector<Veh*> > V) {
	TNetwork tnet;
	tnet.img = imread("back.jpg", CV_LOAD_IMAGE_COLOR);
	for (auto trajectory : V) {
		if (tnet.lanes.empty()) {
			tnet.createlane (trajectory);
		}
		else {
			tnet.alter.sign = 1;
			//cout << trajectory[0]->num<<"numsign"<<tnet.alter.sign<<endl;
			for (int i = 0; i < trajectory.size(); i ++) {
				Veh * veh = trajectory[i];
				tnet.alter.refresh(veh, (tnet.contain(veh)));
			}
			tnet.alter.refresh();
		}
		tnet.addlane();
		
		//for (int i = 0; i < tnet.lanes.size(); i++)
			//cout << "size" << tnet.lanes[i]->lanenum << endl;
	}
	tnet.del7();
	tnet.fillmat(V);
	int save = 1;
	tnet.findlane(V);
	tnet.show(1,0,save);
	
}


double Vehdis(Veh* a, Veh* b) {
	//veh b is the obj that already in the stack;
	//speed limit limit;
	double spdlim = 2000;//ft/s
	double dis = 50;
	if (b->state) {
		dis = sqrt(pow((*a).cx - b->forcast[0], 2) + pow((*a).cy - b->forcast[1], 2));
	}
	else {
		dis = sqrt(pow((*a).cx - (*b).cx, 2) + pow((*a).cy - (*b).cy, 2));
	}
	//	//////////////////////////////////////////////////////////////////////////////////////////////////////
    double dt;
    dt = static_cast<double>(a->vtype) / b->vtype;
	if (dis > 50.0 || dt > 2.0 || dt < 0.5)
		return 1000;
	else if (dt < 1.05 && dt > 0.95 && dis < 1)
		return -1;
	else
		return dis;
}

void Constructveh(vector<vector<Veh*> > &Fig) {
	std::string fil = "output.csv";
	int n, ty, x, y, gtime, w, h;
	std::string sn, sty, sx, sy, sgtime, slnum;
	ifstream mycsv(fil);
	int pvn = 0;
	vector<Veh *> onecar;
	Rect kiss(XPIXEL*0.0, 0, XPIXEL*0.99, YPIXEL*0.35);
	while (1) {
		if (!getline(mycsv, sn, ','))
			break;
		n = stoi(sn);
		getline(mycsv, sgtime, ',');
		gtime = stof(sgtime);
		getline(mycsv, sx, ',');
		x = stof(sx);
		getline(mycsv, sy, ',');
		y = stof(sy);
		getline(mycsv, slnum, ',');
		getline(mycsv, sty);
		ty = stoi(sty);
		if (kiss.contains(Point(x, y)))
			continue;
		int w = 0, h = 0;
		Veh* v = new Veh(n, double(ty),RotatedRect(Point2f(x,y), Size2f(w,h), 0), n, 0);
		if (pvn != n) {
			Fig.push_back(onecar);
			onecar.clear();
			onecar.push_back(v);
			pvn = n;
		}
		else {
			if(onecar.size() > 0)
				onecar.back()->nv = v;
			onecar.push_back(v);
		}
	}
	Rect center(XPIXEL*0.01, YPIXEL*0.5, XPIXEL*0.98, YPIXEL*0.55);
	for (int i = 0; i < Fig.size(); i++) {
		if(Fig[i].size() < 150)
			Fig.erase(Fig.begin() + i--);
		else {
			if (center.contains(Point2f(Fig[i][0]->cx, Fig[i][0]->cy)) || center.contains(Point2f(Fig[i].back()->cx, Fig[i].back()->cy))) {
				Fig.erase(Fig.begin() + i);
				i--;
			}
		}
	}
	cout << Fig.size();
}