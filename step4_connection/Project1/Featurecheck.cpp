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
	string file = parapaths + "/square.txt";
	Rect output;
	edgeground = background.clone();
	int w0 = 960, h0 = HI;
	if (exists_file(file)) {
		ifstream input(file);
		string number;
		int paras[4];
		int i = 0;
		while (i < 4) {
			getline(input, number);
			paras[i] = atoi(number.c_str());
			i++;
		}
		output = Rect(paras[0], paras[1], w0, h0);
	}
	else {
		namedWindow("Background", WINDOW_NORMAL | WINDOW_KEEPRATIO);
		cv::moveWindow("Background", 50, 50);
		cv::resizeWindow("Background", 500, 750);
		cv::setMouseCallback("Background", Mouseclick1, &edgepoints);
		imshow("Background", edgeground);
		char c = waitKey(0);
		if (c == 't') {
			if (edgepoints.size() > 0) {
				Point topleft(edgepoints.back().x, 0);
				output = Rect(topleft, topleft + Point(w0, h0));
			}
		}
		rectangle(edgeground, output, Scalar(255), 5, 6, 0);
		imshow("Background", edgeground);
		c = waitKey(3000);
		destroyAllWindows();
		std::ofstream outfile(file);
		outfile << to_string(output.x) << std::endl;
		outfile << to_string(output.y) << std::endl;
		outfile << to_string(output.width) << std::endl;
		outfile << to_string(output.height) << std::endl;
		outfile.close();
	}
	return output;
}

bool myVideo::inEdge(Veh* vehi) {
	return edge.contains(Point(vehi->rect.x, vehi->rect.y))
		|| edge.contains(Point(vehi->rect.x + vehi->rect.width,
			vehi->rect.y + vehi->rect.height));
}

bool myVideo::inEdge(Rect rect) {
	return edge.contains(Point(rect.x, rect.y))
		|| edge.contains(Point(rect.x + rect.width,
			rect.y + rect.height));
}

bool myVideo::totalinedge(Veh* vehi) {
	return edge.contains(Point(vehi->rect.x, vehi->rect.y))
		&& edge.contains(Point(vehi->rect.x + vehi->rect.width,
			vehi->rect.y + vehi->rect.height));
}

bool myVideo::totalinedge(Rect rect) {
	return edge.contains(Point(rect.x, rect.y))
		&& edge.contains(Point(rect.x + rect.width,
			rect.y + rect.height));
}


void myVideo::checkVehicles(vector<vector<Veh*> > &V) {
	Veh * temp;
	vector<Veh*> undetect;
	Veh * vehi;
	int pvc = 0; 
	for (int i = 0; i < V.size(); i++) {//V.size() - 1
		for (int j = 0; j < V[i].size(); j++) {
			vehi = V[i][j];
			if (vehi->state == 777)
				continue;
			if(!isCorrect(vehi, V, undetect))
				//if(vehi->num == 22827 || vehi->num == 21539)
				Trackvehicle(vehi, V);
			cout << i << " in " << V.size() << endl;
		}
	}
}

void myVideo::Calirect(Rect rect0, Rect &rect) {
	int x01 = rect0.x, x02 = rect0.y, x03 = rect0.x + rect0.width, x04 = rect0.y + rect0.height;
	float x1, x2, x3, x4;
	int y03 = rect.x + rect.width, y04 = rect.y + rect.height;
	int y01 = rect.x, y02 = rect.y;
	float y1, y2, y3, y4;
	Calipoint(x01, x02, x1, x2);
	Calipoint(x03, x04, x3, x4);
	Calipoint(y01, y02, y1, y2);
	Calipoint(y03, y04, y3, y4);

	float w1 = abs(x1 - x3), w2 = abs(x2 - x4);
	float w1p= abs(y1 - y3), w2p = abs(y2 - y4);
	int sw = w1 / w1p * rect.width, sh = w2 / w2p * rect.height;
	int sy0 = (y02 + y04) / 2, sx0 = (y01 + y03) / 2;
	float ratio = 1200.0f;	
	int sy;
	sy = sy0 - sh / 2;
	int sx = sx0 - sw / 2;
	rect = Rect(sx, sy, sw, sh);
}


void myVideo::Calipoint(int x0, int y0, float &x, float &y) {
	/*
	float WS = 0.2064, HS = 0.1089;
	float WI = 3840, HI = 2160;
	float XS0 = WS / 2, YS0 = HS / 2;
	float FF = 0.0886, HH = 400;
	float AL = 0.9687, BE = 0.5510;
	*/
	float xp, yp, cs = cos(AL), ss = sin(AL);
	float t = HH / cs, fy = FF / cs;
	xp = x0*WS / WI - XS0;
	yp = y0*HS / HI - YS0;
	y = float(t*yp / (fy + ss * yp));
	x = float(xp * (t - ss * y) / FF);
}

bool exist(vector<Veh*> &vi, Veh * &vehi) {
	for (int i = 0; i < vi.size(); i++)
		if (vi[i]->num == vehi->num){
			if (vi[i]->ct > vehi->ct) {
				vehi->nv = vi[i];
				vi[i]->pv = vehi;
			}
			else{
				vi[i]->nv = vehi;
				vehi->pv = vi[i];
			}
			return true;
		}
	return false;
}


bool myVideo::tempmatch(Mat frame0_rect, Rect rect, Mat frame1, Rect trackrect, Rect &trackresult) {
	Mat img = frame1(trackrect);
	Mat templ = frame0_rect.clone();
	Mat result;

	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	if (result_cols <= 0 || result_rows <= 0)
		return false;
	result.create(result_rows, result_cols, CV_32FC1);

	int match_method = CV_TM_CCOEFF;
	matchTemplate(img, templ, result, match_method);
	cv::normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = maxLoc;
	if (maxVal > 0.9) {
		trackresult = Rect(matchLoc.x, matchLoc.y, rect.width, rect.height);
		return true;
	}
	else
		return false;
}

void myVideo::add15vehs(Veh* vehn, Veh* vehn2, vector<vector<Veh*> > &V, int blockn, int si) {
	float disw = (vehn2->rect.x - vehn->rect.x) / blockn;
	float dish = (vehn2->rect.y - vehn->rect.y) / blockn;
	int vehx = vehn->rect.x, vehy = vehn->rect.y;
	int w = vehn->rect.width, h = vehn->rect.height;
	Veh *newveh, *newveh0 = vehn;
	int count;

	if(si > 0){	
		for (int i = 0; i < blockn - 1; i++) {
			vehx += disw;
			vehy += dish;
			Rect newrect(vehx, vehy, w, h);
			Veh* newveh = new Veh(newveh0, 1, newrect);
			count = newveh->ct;
			newveh->pv = newveh0;
			newveh0->nv = newveh;
			newveh0 = newveh;
			V[count].push_back(newveh);
		}
		count = vehn2->ct;
		vehn2->pv = newveh0;
		newveh0->nv = vehn2;
		V[count].push_back(vehn2);
	}
	else {
		count = vehn->ct;
		V[count].push_back(vehn);
		for (int i = 0; i < blockn - 1; i++) {
			vehx += disw;
			vehy += dish;
			Rect newrect(vehx, vehy, w, h);
			Veh* newveh = new Veh(newveh0, 1, newrect);
			count = newveh->ct;
			newveh->pv = newveh0;
			newveh0->nv = newveh;
			newveh0 = newveh;
			V[count].push_back(newveh);
		}
	}
}

void myVideo::Trackvehicle(Veh* &vehi, vector<vector<Veh*> > &V) {
	int motionh = 30, motionw = 20;

	vector<Scalar> colors{ Scalar(0,0,255) };
	float maxtrackdis = 50;
	int blocked, blockn = 1;
	
	string fname = "feature";
	int Win = 1000, Hin = 500, mov = 0;
	namedWindow(fname, WINDOW_NORMAL | WINDOW_KEEPRATIO);//
	cv::moveWindow(fname, WINDOWSXP, WINDOWSYP);//
	cv::resizeWindow(fname, Win, Hin);//
	cv::moveWindow(fname, WINDOWSXP + mov, WINDOWSYP);//

	Mat trackframe;
	Rect trackrect, trackresult, rect0;
	int count, truckednum, end0;

	Mat frame0, frame1, frame2, frame0_rect;
	Veh *vehp = vehi;
	while (vehp->pv != NULL)
		vehp = vehp->pv;
	if (!totalinedge(vehp))
		goto nextstep;
	count = vehp->ct;

	Veh *vehp2 = vehp;	
	int vehw = vehp->rect.width, vehh = vehp->rect.height;
	rect0 = Rect(-motionw, -motionh / 2, motionw * 2 + vehw, vehh + motionh);
	readvehframe(frame0, count);
	frame2 = frame0.clone();
	cv::rectangle(frame2, vehp->rect, Scalar(255, 0, 0), 5);

	cv::imshow(fname, frame2);//
	cv::waitKey(100);//


	blocked = 1;
	truckednum = 0, end0 = 500;
	frame0_rect = frame0(vehp->rect);
	while (truckednum++ < end0) {
		if (vehp->rect.y + vehp->rect.height > edge.y + edge.height || count < 0)
			break;
		if (!frame1.empty() && blockn < 2)
			frame0 = frame1.clone();
		if(blockn < 2)
			trackrect = rect0 + Point(vehp->rect.x, vehp->rect.y);
		count--;
		if (!totalinedge(trackrect))
			break;
		if (count < 0)
			break;
		if (existinframe(vehp->num, V, vehp->ct - blockn)) {
			deleteoneveh(vehp->num, V, vehp->ct - blockn);
		}
		if (int delnum = existcloseveh(vehp, V, vehp->ct - blockn))
			break;
		readvehframe(frame1, count);
		trackframe = frame1(trackrect);
		//Showframe("tf",trackframe, 20, 2000,300,300);
		bool tracked = tempmatch(frame0_rect, vehp->rect, frame1, trackrect, trackresult);
		trackresult += Point(trackrect.x, trackrect.y);
		if (tracked && ((trackresult&block).area() == 0 || vehp->rect.height > 100)) {
			vehp2 = new Veh(vehp, -1, trackresult);
			vehp2->ct = vehp->ct - blockn;
			add15vehs(vehp2, vehp, V, blockn, -1);
			
			vehp = vehp2;
			blockn = 1;
		}
		else {
			//cout << "vehicle " << vehp->num << " in frame " << vehp->ct - 1 << " is blocked.\n";
			//count--;
			blockn++;
			trackrect = trackrect + Point(-10, 0) + Size2i(20,vehp->spd);
			continue;
		}
		
		frame2 = frame1.clone();
		cv::rectangle(frame2, trackresult, colors[0], 3, 4);//
		cv::imshow(fname, frame2);//
		cv::waitKey(1);//
	}
	nextstep:
	Veh *vehn = vehi;
	while (vehn->nv != NULL)
		vehn = vehn->nv;
	count = vehn->ct;
	Veh *vehn2 = vehn;

	readvehframe(frame0, count);
	frame2 = frame0.clone();
	cv::rectangle(frame2, vehn->rect, Scalar(255, 0, 0), 5);

	cv::imshow(fname, frame2);//
	cv::waitKey(100);//

	vehw = vehn->rect.width, vehh = vehn->rect.height;
	rect0 = Rect(-motionw, motionh / 2, motionw * 2 + vehw, vehh + motionh);
	blocked = 1;
	frame0_rect = frame0(vehn->rect);
	truckednum = 0, end0 = 500;
	while (truckednum++ < end0) {
		if (vehn->rect.y < edge.y || count >= V.size() - 1)
			break;
		if (!frame1.empty() && blockn < 2)
			frame0 = frame1.clone();
		if (blockn < 2)
			trackrect = rect0 + Point(vehn->rect.x, vehn->rect.y - motionh);
		count++;
		if (trackrect.y <= 0)
			break;
		if (existinframe(vehn->num, V, vehn->ct + blockn)) {
			deleteoneveh(vehn->num, V, vehn->ct + blockn);
		}
		if (int delnum = existcloseveh(vehn, V, vehn->ct + blockn))
			break;
		readvehframe(frame1, count);
		trackframe = frame1(trackrect);
		//Showframe("tf", trackframe, 20, 2000, 300, 300);
		bool tracked = tempmatch(frame0_rect, vehn->rect, frame1, trackrect, trackresult);
		trackresult += Point(trackrect.x, trackrect.y);
		if (tracked && ((trackresult&block).area() == 0 || vehn->rect.height > 100)) {
			vehn2 = new Veh(vehn, 1, trackresult);
			vehn2->ct = vehn->ct + blockn;
			add15vehs(vehn, vehn2, V, blockn, 1);
			vehn = vehn2;
			blockn = 1;
		}
		else {
			//cout << "vehicle " << vehn->num << " in frame " << vehn->ct + 1 << " is blocked.\n";
			blockn++;
			trackrect = trackrect + Size2i(20, vehn->spd) - Point2i(10, vehn->spd);
			continue;
		}

		frame2 = frame1.clone();
		
		
		cv::rectangle(frame2, trackresult, colors[0], 3, 4);////
		cv::imshow(fname, frame2);//
		cv::waitKey(1);//
	}

}

void myVideo::readvehframe(Mat &frame, int count) {
	string file = paths + "/" + to_string(count) + ".jpg";
	frame = imread(file);
}

bool myVideo::isCorrect(Veh * &vehi, vector<vector<Veh*> > &V, vector<Veh*> &undetect) {
	bool output = 1;
	int cum1 = 0, cum2 = 0;
	Veh *v1 = NULL, *v2 = NULL;
	while (vehi->nv != NULL){
		vehi->state = 777;
		vehi = vehi->nv;
	}
	vehi->state = 777;
	//output &= anglecheck(vehi, V);
	//output &= sizecheck(vehi, V);
	output &= edgecheck(vehi, V);
	if (!output && vehi != NULL)
		undetect.push_back(vehi);
	return output;
}

void myVideo::deleteprevehicles(Veh * &vehi, vector<vector<Veh*> > &V) {
	int count, num = vehi->num;
	Veh *vj = vehi;
	while (vj != NULL) {
		count = vj->ct;
		vj = vj->pv;
		for (int k = 0; k < V[count].size(); k++) {
			if (V[count][k]->num == num) {
				V[count].erase(V[count].begin() + k);
				k--;
			}
		}
	}
}

bool myVideo::anglecheck(Veh * &vehi, vector<vector<Veh*> > &V) {
	float maxangle = 90.0f;
	float maxanglechange = maxangle / 2.0f;
	int vehn = 0;
	Veh* vi = vehi;
	if (abs(vi->direction - videodirection * PI / 180.0) > maxanglechange)
		vehi = NULL;
	while (vi != NULL) {
		if (abs(vi->direction - videodirection * PI / 180.0) > maxanglechange) {
			if (vi->nv != NULL) {
				Veh *vi1 = vi->nv;
				vi1->pv = NULL;
			}
			deleteprevehicles(vi, V);
			return false;
		}
		vi = vi->pv;
	}
	return true;
}

bool myVideo::sizecheck(Veh * &vehi, vector<vector<Veh*> > &V) {
	if (vehi == NULL)
		return false;
	float maxratio = 0.5f;
	Veh* vi = vehi, *vj = vehi->pv;
	if (vj == NULL) {
		return false;
	}
	while (vi != NULL && vj != NULL) {
		int a1 = vi->rect.area(), a2 = vj->rect.area();
		if (abs(a1 - a2) > MINCONAREA * 15) {
			vi->pv = NULL;
			deleteprevehicles(vj, V);
			return false;
		}
		vi = vj;
		vj = vj->pv;
	}
	return true;
}

bool myVideo::edgecheck(Veh * &vehi, vector<vector<Veh*> > &V) {
	Veh *vi = vehi;
	if (totalinedge(vehi) && vehi->ct < V.size() - 1)
		return false;
	else {
		while (vi->pv != NULL)
			vi = vi->pv;
		if (totalinedge(vi) && vehi->ct > 0)
			return false;
	}
	return true;
}


void deletedups(vector<vector<Veh*>> V) {
	for (int i = 0; i < V.size(); i++) {

	}
}

bool myVideo::existinframe(int num, vector<vector<Veh*> > &V, int count) {
	for (auto i : V[count]) {
		if (i->num == num)
			return num;
	}
	return false;
}

int myVideo::existcloseveh(Veh *vehi, vector<vector<Veh*> > &V, int count) {
	for (auto i : V[count]) {
		if ((i->rect & vehi->rect).area() > 0.8*i->rect.area() || (i->rect & vehi->rect).area() > 0.8*vehi->rect.area())
			return i->num;
	}
	return 0;
}

void myVideo::deleteoneveh(int delnum, vector<vector<Veh*> > &V, int count) {
	for (int i = 0; i < V[count].size(); i++) {
		if (V[count][i]->num == delnum) {
			Veh* vehp = V[count][i]->pv, *vehn = V[count][i]->nv;
			if (vehp != NULL)
				vehp->nv = NULL;
			if (vehn != NULL)
				vehn->pv = NULL;
			V[count].erase(V[count].begin() + i--);
		}
	}
}

void myVideo::Trackprevehicle(Veh* &vehi, vector<vector<Veh*> > &V) {
	int motionh = 30, motionw = 20;

	vector<Scalar> colors{ Scalar(0,0,255) };
	float maxtrackdis = 50;
	int blocked, blockn = 1;

	string fname = "feature";
	int Win = 1000, Hin = 500, mov = 0;
	//namedWindow(fname, WINDOW_NORMAL | WINDOW_KEEPRATIO);
	//cv::moveWindow(fname, WINDOWSXP, WINDOWSYP);
	//cv::resizeWindow(fname, Win, Hin);
	//cv::moveWindow(fname, WINDOWSXP + mov, WINDOWSYP);

	Mat trackframe;
	Rect trackrect, trackresult, rect0;
	int count, truckednum, end0;

	Mat frame0, frame1, frame2, frame0_rect;
	Veh *vehp = vehi;
	while (vehp->pv != NULL)
		vehp = vehp->pv;
	count = vehp->ct;

	Veh *vehp2 = vehp;
	int vehw = vehp->rect.width, vehh = vehp->rect.height;
	rect0 = Rect(-motionw, -motionh / 2, motionw * 2 + vehw, vehh + motionh);
	readvehframe(frame0, count);
	frame2 = frame0.clone();
	cv::rectangle(frame2, vehp->rect, Scalar(255, 0, 0), 5);

	//cv::imshow(fname, frame2);
	//cv::waitKey(100);


	blocked = 1;
	truckednum = 0, end0 = 600;
	frame0_rect = frame0(vehp->rect);
	while (truckednum++ < end0) {
		if (vehp->rect.y + vehp->rect.height > edge.y + edge.height || count < 0)
			break;
		if (!frame1.empty() && blockn < 2)
			frame0 = frame1.clone();
		if (blockn < 2)
			trackrect = rect0 + Point(vehp->rect.x, vehp->rect.y);
		count--;
		if (!totalinedge(trackrect))
			break;
		if (count < 0)
			break;
		if (existinframe(vehp->num, V, vehp->ct - blockn)) {
			deleteoneveh(vehp->num, V, vehp->ct - blockn);
		}
		if (int delnum = existcloseveh(vehp, V, vehp->ct - blockn))
			deleteoneveh(delnum, V, vehp->ct - blockn);
		readvehframe(frame1, count);
		trackframe = frame1(trackrect);
		//Showframe("tf",trackframe, 20, 2000,300,300);
		bool tracked = tempmatch(frame0_rect, vehp->rect, frame1, trackrect, trackresult);
		trackresult += Point(vehp->rect.x - motionw, vehp->rect.y - motionh / 2);
		if (tracked && ((trackresult&block).area() == 0 || vehp->rect.height > 100)) {
			vehp2 = new Veh(vehp, -1, trackresult);
			vehp2->ct = vehp->ct - blockn;
			add15vehs(vehp2, vehp, V, blockn, -1);

			vehp = vehp2;
			blockn = 1;
		}
		else {
			//cout << "vehicle " << vehp->num << " in frame " << vehp->ct - 1 << " is blocked.\n";
			//count--;
			//break;
			blockn++;
			trackrect += Size2i(0, vehp->spd);
			continue;
		}

		frame2 = frame1.clone();
		//cv::rectangle(frame2, trackresult, colors[0], 3, 4);
		//cv::imshow(fname, frame2);
		//cv::waitKey(1);
	}
}

void myVideo::Trackpostvehicle(Veh* &vehi, vector<vector<Veh*> > &V) {
	int motionh = 30, motionw = 20;

	vector<Scalar> colors{ Scalar(0,0,255) };
	float maxtrackdis = 50;
	int blocked, blockn = 1;

	string fname = "feature";
	int Win = 1000, Hin = 500, mov = 0;
	//namedWindow(fname, WINDOW_NORMAL | WINDOW_KEEPRATIO);
	//cv::moveWindow(fname, WINDOWSXP, WINDOWSYP);
	//cv::resizeWindow(fname, Win, Hin);
	//cv::moveWindow(fname, WINDOWSXP + mov, WINDOWSYP);

	Mat trackframe;
	Rect trackrect, trackresult, rect0;
	int count, truckednum, end0;

	Mat frame0, frame1, frame2, frame0_rect;
	Veh *vehn = vehi;
	while (vehn->nv != NULL)
		vehn = vehn->nv;
	count = vehn->ct;
	Veh *vehn2 = vehn;

	readvehframe(frame0, count);
	frame2 = frame0.clone();
	cv::rectangle(frame2, vehn->rect, Scalar(255, 0, 0), 5);

	//cv::imshow(fname, frame2);
	//cv::waitKey(100);

	int vehw = vehn->rect.width, vehh = vehn->rect.height;
	rect0 = Rect(-motionw, motionh / 2, motionw * 2 + vehw, vehh + motionh);
	blocked = 1;
	frame0_rect = frame0(vehn->rect);
	truckednum = 0, end0 = 900;
	while (truckednum++ < end0) {
		if (vehn->rect.y < edge.y || count >= V.size() - 1)
			break;
		if (!frame1.empty() && blockn < 2)
			frame0 = frame1.clone();
		if (blockn < 2)
			trackrect = rect0 + Point(vehn->rect.x, vehn->rect.y - motionh);
		count++;
		if (trackrect.y <= 0)
			break;
		if (existinframe(vehn->num, V, vehn->ct + blockn)) {
			deleteoneveh(vehn->num, V, vehn->ct + blockn);
		}
		if (int delnum = existcloseveh(vehn, V, vehn->ct + blockn))
			deleteoneveh(delnum, V, vehn->ct + blockn);
		readvehframe(frame1, count);
		trackframe = frame1(trackrect);
		//Showframe("tf", trackframe, 20, 2000, 300, 300);
		bool tracked = tempmatch(frame0_rect, vehn->rect, frame1, trackrect, trackresult);
		trackresult += Point(vehn->rect.x - motionw, vehn->rect.y - motionh / 2 - vehn->spd*(blockn - 1));
		if (tracked && ((trackresult&block).area() == 0 || vehn->rect.height > 100)) {
			vehn2 = new Veh(vehn, 1, trackresult);
			vehn2->ct = vehn->ct + blockn;
			add15vehs(vehn, vehn2, V, blockn, 1);

			vehn = vehn2;
			blockn = 1;
		}
		else {
			//cout << "vehicle " << vehn->num << " in frame " << vehn->ct + 1 << " is blocked.\n";
			break;
			blockn++;
			trackrect = trackrect + Size2i(0, vehn->spd) - Point2i(0, vehn->spd);
			continue;
		}

		frame2 = frame1.clone();


		//cv::rectangle(frame2, trackresult, colors[0], 3, 4);
		//cv::imshow(fname, frame2);
		//cv::waitKey(1);
	}
}

bool myVideo::Trackpostonevehicle(Veh* &vehi, Mat frame0, Mat frame1, Rect &output) {
	Rect trackresult;
	bool tracked = false;
	int vehw = vehi->rect.width, vehh = vehi->rect.height;
	int motionw = 20, motionh = 50;
	int traceN = 10;
	Rect trackrect = Rect(vehi->rect.x - motionw, vehi->rect.y - motionh, motionw * 2 + vehw, vehh + motionh);
	if (!inEdge(trackrect) || !inEdge(vehi) || vehi->rect.x > edge.width + edge.x - 130 ||vehi->rect.y < 200)
		return false;
	if ((trackrect & block).area() > trackrect.area() * 0.2) {
		Veh *vehj = vehi->pv;
		if (vehj == NULL)
			return false;
		for (int i = 0; i < traceN; i++) {
			vehj = vehj->pv;
			if (vehj == NULL)
				return false;
		}
		int ox, oy, ow, oh;
		ox = vehi->rect.x + (vehi->rect.x - vehj->rect.x) / traceN;
		oy = vehi->rect.y + (vehi->rect.y - vehj->rect.y) / traceN;
		ow = vehi->rect.width;
		oh = vehi->rect.height;
		output = Rect(ox, oy, ow, oh);
		return true;
	}
	Mat frame0_rect = frame0(vehi->rect);
	tracked = tempmatch(frame0_rect, vehi->rect, frame1, trackrect, trackresult);
	if (tracked) {
		output = trackresult + Point2i(trackrect.x, trackrect.y);
		return true;
	}
	else
		return false;
}
