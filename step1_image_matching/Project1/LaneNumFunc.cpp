#include "myVideo.h"
#include "Veh.h"

bool TNetwork::lane::addveh(Veh* veh) {
	Point2f vehloc(veh->cx, veh->cy);
	point.push_back(vehloc);
	veh->lnum = lanenum;
	return 1;
}

bool TNetwork::lane::contain(Veh* veh, int &loc, float& distance) {
	Point2f vehloc(veh->cx, veh->cy);
	float dis = 0;
	float currectdis = 1e6;
	//cout << lanenum << endl;
	for (int i = 0; i < point.size(); i ++) {
		Point2f pot = point[i];
		dis = norm(pot - vehloc);
		if (dis < maxdis) {
			veh->lnum = lanenum;
			while (dis < currectdis && i < point.size()) {
				currectdis = dis;
				i++;
				pot = point[i];
				dis = norm(pot - vehloc);
			}
			loc = i;
			distance = currectdis;
			return 1;
		}
	}
	loc = 0;
	return 0;
}

bool TNetwork::intersection::contain(Veh* veh) {
	//cout << rect.size() << endl;
	Point2f vehloc(veh->cx, veh->cy);
	if (rect.contains(vehloc))
		return 1;
	return 0;
}

bool TNetwork::contain(Veh* veh) {
	for (int i = 0; i < lanes.size(); i ++) {
		int loc = 0;
		float dis = 0.0;
		if (lanes[i]->contain(veh,loc,dis)){
			//cout << "lane contain" << endl;
			if (prelanecontain != lanes[i]->lanenum) {
				if (splitlane(i, loc)) {
					createinter(lanes[i]->point[loc], lanes[i]->lanenum);
					//cout << "intersection" << endl;
				}
			}
			//cout << "?" << endl;
			prelanecontain = lanes[i]->lanenum;
			return 1;
		}
	}
	prelanecontain = -1;
	//cout << "prelanecontain0" << endl;
	for (auto inter : inters) {
		if (inter->contain(veh)) {
			//cout << "intercontain" << endl;
			return 1;
		}
	}
	//cout << "contain over 0" << endl;
	return 0;
}

void TNetwork::createlane(vector<Veh *> trajectory) {
	TNetwork::lane* firstlane = new TNetwork::lane(0);
	for (auto i : trajectory)
		firstlane->addveh(i);
	lanes.push_back(firstlane);
}

void TNetwork::addvehtolane(Veh * veh) {
	int num = lanes.back()->lanenum + 1;
	TNetwork::lane* lane = new TNetwork::lane(num);
	lane->addveh(veh);
	lanes.push_back(lane);
}

bool TNetwork::alterlane::refresh(Veh* veh, int state) {
	//cout << "refresh(11)" << sign << state << endl;
	Point2f p(veh->cx, veh->cy);
	if (state == 0 && sign > 0) {
		vector<Point2f> v;
		v.push_back(p);
		alterlanes.push_back(v);
	}
	if (state == 0 && sign == 0) {
		alterlanes.back().push_back(p);
	}
	sign = state;
	//cout << "refresh(11)end" << endl;
	return 1;
}

bool TNetwork::alterlane::refresh() {
	//cout << "refresh()" << endl;
	if (alterlanes.empty())
		return 0;
	alterlanes.erase(
		std::remove_if(
			alterlanes.begin(), alterlanes.end(),
			[](vector<Point2f> i) {return i.size() < 150; }),
		alterlanes.end()
	);
	if (alterlanes.empty())
		return 0;
	else
		return 1;

}

bool TNetwork::addlane() {
	//cout << "addlane" << endl;
	if (alter.alterlanes.empty())
		return 0;
	else {
		for (int i = 0; i < alter.alterlanes.size(); i++) {
			lane* la = new lane(lanes.back()->lanenum+1);
			la->point = alter.alterlanes[i];
			lanes.push_back(la);
		}
	}
	alter.alterlanes.clear();
	alter.sign = 1;
	return 1;
}

bool TNetwork::splitlane(int i, int loc) {
	//cout << "splitlane" << endl;
	if (lanes[i]->point.size() - loc > lanes[i]->minsplitlen && loc > lanes[i]->minsplitlen) {
		int num = lanes.back()->lanenum + 1;
		lane* lan = new lane(num);
		for (int j = loc; j < lanes[i]->point.size(); j++) {
			lan->point.push_back(lanes[i]->point[j]);
		}
		lanes.push_back(lan);
		lanes[i]->point.erase(lanes[i]->point.begin()+loc, lanes[i]->point.end());
		//lanei->point.clear();
		return 1;
	}
	return 0;
}

bool TNetwork::createinter(Point2f point, int num) {
	//cout << "create inter" << endl;
	int internum;
	if (inters.empty()) {
		internum = 0;
	}
	else
		internum = inters.back()->internum + 1;
	intersection* inter = new intersection(internum);
	inter->lanenumbers.push_back(num);
	inter->lanenumbers.push_back(lanes.back()->lanenum);
	inter->rect = Rect(point.x - interw / 2, point.y - interh / 2, interw, interh);
	inters.push_back(inter);
	return 0;
}

void TNetwork::show(int sh, bool rf, bool save) {
	if (rf) {
		img = Mat(YPIXEL, XPIXEL, CV_8UC3, Scalar(255, 255, 255));
	}
	int poi = 20;
	int step = 57;
	if (sh) {
		Rect cut(XPIXEL*0.4, YPIXEL*0.45, XPIXEL*0.55, YPIXEL*0.55);
		for (auto i : lanes) {
			//for (int j = 0; j < i->point.size(); j++) {
			int j = poi % i->point.size();
			poi += step;
			while(!cut.contains(i->point[j])){
				j = poi % i->point.size();
				poi += step;
				poi = poi % 300;
			}
			if(i->lanenum)
				putText(img, to_string(i->lanenum), i->point[j] - Point2f(50,-20), FONT_HERSHEY_SCRIPT_SIMPLEX, 3, cv::Scalar(1, 0, 255), 12);
				//if(i->lanenum==0)
					//circle(img, i->point[j], 2, Scalar(0, 0, 0), 2, 8, 0);
			//}
		}
		/*for (auto i : inters) {
			rectangle(img, i->rect, Scalar(0, 0, 222), 3, 8, 0);
			//putText(img, to_string(i->internum), Point(i->rect.x,i->rect.y), FONT_HERSHEY_SCRIPT_SIMPLEX, 3, cv::Scalar(111, 111, 0.1), 8);
		}*/
		
		img = img(cut);
		namedWindow("Points", WINDOW_NORMAL | WINDOW_KEEPRATIO);
		cv::moveWindow("Points", WINDOWSXP, WINDOWSYP);
		cv::resizeWindow("Points", WINDOWSW, WINDOWSH);
		imshow("Points", img);                   // Show our image inside it.
		waitKey(5000);
		if (save)
			imwrite("lanenumber1.png", img);
		//cv::destroyAllWindows();
	}
}

void TNetwork::fillmat(vector<vector<Veh*> > V) {
	
	for (auto i : V) {
		for (auto j : i) {
			circle(img, cv::Point2f(j->cx, j->cy), 2, Scalar(0, 0, 0), 2, 8, 0);
		}
	}
}

int TNetwork::findlanenumber(Point2f p) {
	int num;
	float dis = 1e9;
	float dis2;
	for (auto i : lanes) {
		for (auto j : i->point) {
			dis2 = norm(j - p);
			if (dis2 < dis) {
				dis = dis2;
				num = i->lanenum;
			}
		}
	}
	return num;
}


void TNetwork::findlane(vector<vector<Veh*> > &V) {
	std::ofstream mycsv;
	mycsv.open("lanenumbered.csv");
	int lnum;
	for (int i = 0; i < V.size(); i++) {
		for (int j = 0; j < V[i].size() - 1; j++) {
			Point p1(V[i][j]->cx, V[i][j]->cy);
			Point p2(V[i][j+1]->cx, V[i][j+1]->cy);
			if (norm(p1 - p2) > 55){
				V.erase(V.begin() + i--);
				break;
			}
		}
	}
	for (auto &i : V) {
		for (auto &j : i) {
			lnum = findlanenumber(Point2f(j->cx, j->cy));
			mycsv << j->num << "," << j->glotime/double(FRAMERATE) << "," << j->cx << "," << j->cy << "," << lnum << "," << j->vtype << ",\n";
		}
	}
	mycsv.close();
}

void TNetwork::del7() {
	int num = 7;;
	for (int i = 0; i < lanes.size(); i++) {
		if (lanes[i]->lanenum == num) {
			lanes.erase(lanes.begin() + i);
			i--;
		}
		if (lanes[i]->lanenum > num) {
			lanes[i]->lanenum--;
		}
	}
}