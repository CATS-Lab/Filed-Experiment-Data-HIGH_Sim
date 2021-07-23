#include "myVideo.h"
#include "Veh.h"

Mat theframe;
void Cross2(cv::Mat frame, cv::Point point, cv::Scalar s) {
	int cl = 10;
	cv::Point a(cl, 0), b(-cl, 0), c(0, cl), d(0, -cl);
	line(frame, point + a, point + b, s, 4);
	line(frame, point + c, point + d, s, 4);
}

void Mouseclick3(int evt, int x, int y, int flags, void* edgepoints) {
	cv::Scalar s(0, 0, 255);
	std::vector<cv::Point>* edgep = (std::vector<cv::Point>*)edgepoints;
	Mat frame = theframe.clone();
	if (evt == CV_EVENT_LBUTTONDOWN) {
		edgep->push_back(Point(x, y));
		if (edgep->size() >= 1) {
			for (auto tem : *edgep)
				Cross2(frame, tem, s);
		}
		imshow("video", frame);
	}
	if (evt == CV_EVENT_RBUTTONDOWN) {
		if (edgep->size() >= 1) {
			edgep->pop_back();
			if (edgep->size() >= 1) {
				Point tem = edgep->back();
				Cross2(frame, tem, s);
			}
			imshow("video", frame);
		}
	}
}
void myVideo::mcheck(vector<vector<Veh*>> &V, int count) {
	if (edge.empty())
		edge = addedge();
	if (block.empty())
		block = load_block();
	vector<Veh*> vectorofveh;
	int ss = 0;
	for (int i = 0; i < V.size(); i++) {//V.size()
		for (int j = 0; j < V[i].size(); j++) {
			Veh* vehi = V[i][j];
			if (vehi->pv == NULL && edge.y + edge.height - vehi->y - vehi->height > 100)
				vectorofveh.push_back(vehi);
		}
	}
	cout << "there are " << vectorofveh.size() << " vehs need to be tracked.\n";
	for (int i = 0; i < vectorofveh.size(); i++) {//
		Veh* vehi = vectorofveh[i];
		cout << "there are " << vectorofveh.size() << " vehs need to be tracked.\n";
		cout << "the " << i << "th is being tracked.\n";
		Trackvehicle(vehi, V);
	}
	vectorofveh.clear();
	cout << "done" << endl;
}

void myVideo::deletenostart(vector<vector<Veh*>> &V) {
	vector<Veh*> vectorofveh;
	int ss = 0;
	while (ss++ < 2) {
		for (int i = 50; i < V.size(); i++) {
			for (int j = 0; j < V[i].size(); j++) {
				Veh* vehi = V[i][j];
				if (vehi->pv == NULL && vehi->rect.y + vehi->rect.height < edge.y + edge.height)
					vectorofveh.push_back(vehi);
			}
		}
		for (int i = 0; i < vectorofveh.size(); i++) {//
			Veh* vehi = vectorofveh[i];
			cout << "the " << i << "th is being deleted.\n";
			cout << "there are " << vectorofveh.size() << " vehs need to be tracked.\n";
			Deleteveh(V, vehi->num, 0);
		}
		cout << "done" << endl;
	}
}

void myVideo::getinitialcount(vector<vector<Veh*>> V, int &count) {
	for (int i = count; i < V.size(); i++) {
		for (int j = 0; j < V[i].size(); j++) {
			Veh* vehi = V[i][j];
			if (vehi->nv == NULL && vehi->rect.y > edge.y)
			{
				count = i;
				return;
			}
		}
	}
}

void myVideo::Manualcheck(vector<vector<Veh*>> &V) {
	Mat frame_org, frame_rot;
	vector<cv::Point> contourpoints;
	namedWindow("video", WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow("video", WINDOWSXP, 0);
	cv::resizeWindow("video", 1800, 800);
	cv::setMouseCallback("video", Mouseclick3, &contourpoints);
	int count = 0;
	Mat dst, cdst;
	bool stop = true;
	char c = 'a';
	int newvehnum = -1, nvm, delnum;
	for (auto i : V)
		for (auto j : i)
			if(newvehnum > j->num - 1){
				newvehnum = j->num - 1;
			}

	vector<int> vdelete;
	Rect rect;
	RotatedRect rrect;
	if (edge.empty())
		edge = addedge();
	vector<Veh*> vectorofveh;
	mcheck(V, count);
	//deletenostart(V);
	while (count < V.size()) {///////1
		//if (c != ' ' && count > 85)
			//c = ' ';//
		c = 'm';
		readvehframe(frame_rot, count);
		theframe = frame_rot.clone();
		Size ms = frame_rot.size();
		for (auto &i : V[count]) {
			if (1) {
				rectangle(frame_rot, i->rect, Scalar(0, 255, 255), 6);
				//putText(frame_rot, to_string(i->num), Point(i->cx + i->rect.width / 2, i->cy + i->height / 2), 2, 2, Scalar(255, 0, 0), 8);
			}
		}
		rectangle(frame_rot, edge, Scalar(0, 255, 100), 9);
		rectangle(frame_rot, block, Scalar(0, 255, 100), 9);
		imshow("video", frame_rot);
		c = (char)waitKey(2);//
		if (count == OPFRAMENUM - 102 || c == 'o') {
			cout << "\nThe manual check process has been finished.\n";
			break;
		}
		if (c == ' ')
			stop = !stop; 
		if (stop) {
			cont:
			c = (char)waitKey(0);
			int yes = 0;
			switch (c) {
			case ' ':
				stop = !stop;
				break;
			case 'a':
				count -= 2;
				break;
			case 'd':
				break;
			case 'f':
				getinitialcount(V, count);
				break;
			case 'q':
				count -= 11;
				break;
			case 'e':
				count += 9;
				break;
			case 'g':
				count -= 101;
				break;
			case 'h':
				count += 99;
				break;
			case 'j':
				count -= 1001;
				break;
			case 'k':
				count += 999;
				break;
			case 'c':
				if (contourpoints.size() < 4){
					cout << "\n Please click on more points on a vehicle.\n";
					goto cont;
				}
				rect = boundingRect(contourpoints);
				rrect = minAreaRect(contourpoints);
				if(count > 0)
					nvm = findnumber(rect, V[count - 1], newvehnum);
				else {
					nvm = newvehnum--;
				}
				Shownumber(frame_rot, rect, nvm, 0);
				contourpoints.clear();
				goto cont;
			case 'w':
				cin >> nvm;
				Shownumber(frame_rot, rect, nvm, 0);
				goto cont;
			case 's':
				Veh* vehi;
				Createnewveh(nvm, rrect, V, count, vehi);
				rectangle(frame_rot, V[count].back()->rect, Scalar(0,0,255), 10);
				Showframe("video", frame_rot, 100, 0 ,1800 , 800);
				vectorofveh.push_back(vehi);
				goto cont;
			case 'r':

				cin >> delnum;
				cout << "\nyes?: ";
				cin >> yes;
				if (yes == 1)
					vdelete.push_back(delnum);

				for(int i:vdelete)
					Deleteveh(V, i, count);
				cout << "\n vehicles are deleted."<<endl;
				vdelete.clear();
				goto cont;
			case 't':
				int delnump;
				cin >> delnump;
				while (yes != 1 && yes != 9) {
					cout << "\ndelete veh " << delnump << "?\n";
					cin >> yes;
					if (yes == 1) {
						for (int i = 0; i < V[count].size(); i++) {
							if (V[count][i]->num == delnump) {
								Veh* vehp = V[count][i]->pv, *vehn = V[count][i]->nv;
								if(vehp != NULL)
									vehp->nv = NULL;
								if(vehn != NULL)
									vehn->pv = NULL;
								V[count].erase(V[count].begin() + i);
								break;
							}
						}
						cout << "\n vehicle number " << delnump << " is deleted." << endl;
					}
				}
				goto cont;
			case 'z':
				contourpoints.clear();
				goto cont;
			case 'x':
				cout << "vehicle " << nvm << " has been replaced with code "<< Replaceveh(nvm, rrect, V, count)<<".\n";
				rectangle(frame_rot, V[count].back()->rect, Scalar(0, 0, 255), 10);
				Showframe("video", frame_rot);
				goto cont;
			case 'v':
				vectorofveh.clear();
				goto cont;
			case 'b':
				for (int i = 0; i < vectorofveh.size(); i++) {//
					vehi = vectorofveh[i];
					if(vehi != NULL && vehi->num > -1000)
						Trackvehicle(vehi, V);
					cout << "the " << i << "th is being tracked.\n";
					cout << "there are " << vectorofveh.size() << " vehs need to be tracked.\n";
				}
				vectorofveh.clear();
				cout << "done"<<endl;
				goto cont;
			case 'n':
				if(contourpoints.size() > 0)
					for (int i = 0; i < V[count].size(); i++) {
						if (V[count][i]->rect.contains(contourpoints.back())) {
							Deleteveh(V, V[count][i]->num, 0);
						}
					}
				contourpoints.clear();
				cout << "done\n";
				break;
			case 'y':
				if (contourpoints.size() > 0)
					for (int i = 0; i < V[count].size(); i++) {
						if (V[count][i]->rect.contains(contourpoints.back())) {
							vectorofveh.push_back(vehi);
						}
					}
				contourpoints.clear();
				break;
			case 'm':
				if (contourpoints.size() > 0)
					for (int i = 0; i < V[count].size(); i++) {
						if (V[count][i]->rect.contains(contourpoints.back())) {
							V[count][i]->nv = NULL;
							Deletepostveh(V, V[count][i]->num, V[count][i]->ct);
						}
					}
				contourpoints.clear();
				cout << "done\n";
			}
		}
		count++;
	}
	cv::destroyAllWindows();
}

bool myVideo::Deleteveh(vector<vector<Veh*>> &V, int delnum, int count) {
	for(int i = 0; i < V.size(); i ++)
		for (int j = 0; j < V[i].size(); j++) {
			if (V[i][j]->num == delnum) {
				V[i].erase(V[i].begin() + j);
				j--;
			}
		}
	return 0;
}

bool myVideo::Deletepostveh(vector<vector<Veh*>> &V, int delnum, int count) {
	for (int i = count+1; i < V.size(); i++)
		for (int j = 0; j < V[i].size(); j++) {
			if (V[i][j]->num == delnum) {
				V[i].erase(V[i].begin() + j);
				j--;
			}
		}
	return 0;
}

int myVideo::findnumber(const Rect rect, const vector<Veh*> vehicles, int &num) {
	int dis = 1000, vn = 0;
	for (auto &i : vehicles) {
		Point p1(i->rect.x, i->rect.y);
		Point p2(rect.x, rect.y);
		if (abs(p1.x - p2.x) * 10 + abs(p1.y - p2.y) < dis)
		{
			dis = norm(p1 - p2);
			vn = i->num;
		}
	}
	if (dis < 50) {
		return vn;
	}
	else
		return num--;
}

void myVideo::Shownumber(Mat frame, Rect rect, int number, bool setup) {
	int textshift = 100;
	Point P(rect.x + rect.width / 2, rect.y + rect.height / 2);
	Point P2 = P + Point(textshift, 0);
	Mat frame2 = frame.clone();
	char q = setup ? ' ' : '?';
	rectangle(frame2, rect, Scalar(255, 0, 0), 10);
	putText(frame2, to_string(number) + q, P2, 2, 2, Scalar(255, 0, 0), 3);
	line(frame2, P, P2, Scalar(0, 255, 0), 8);
	Showframe("video", frame2, 100, 0, 1800, 800);
}

void myVideo::Createnewveh(int nvm, RotatedRect rrect, vector<vector<Veh*>> &V, int count, Veh* &vehi) {
	vehi = new Veh(nvm, 0, rrect, count, count);
	vehi->state = 777;
	V[count].push_back(vehi);
}

bool myVideo::Replaceveh(int nvm, RotatedRect rrect, vector<vector<Veh*>> &V, int count) {
	for (int i = 0; i < V[count].size(); i++) {
		if (V[count][i]->num == nvm) {
			Veh *veh2 = V[count][i];
			veh2->cx = rrect.center.x;
			veh2->cy = rrect.center.y;
			veh2->width = rrect.size.width;
			veh2->height = rrect.size.height;
			veh2->rect = rrect.boundingRect();
			veh2->rect.height = min(YPIXEL - veh2->rect.y - 1, veh2->rect.height);
			veh2->state = 777;
			return 1;
		}
	}
	return 0;
}

void myVideo::trackothervehicles(Veh* vehi2, int count, vector<vector<Veh*>> &V) {
	cout << "\nentering tracking function.\n";
	if (edge.area() == 0) {
		cout<<"\nedge is empty, going to add it\n";
		edge = addedge();
		edge.height = YPIXEL - 20 - edge.y;
	}
	/*
	if (vehi2->key.size() < 1)
		Findfeature(vehi2);
	int k = 0;
	bool found;
	while (inEdge(vehi2) && count < V.size() - 1 && ++k < 100) {
		count = vehi2->ct + 1;
		if (findVeh(count, vehi2)) {
			cout << "\nveh is found in frame " << count << " with code " << 1; 
			for (auto i : V[count]) {
				if (i->state < 0)
					continue;
				for (int j = 0; j < 4; j++) {
					if (vehi2->rect.contains(Point2i(i->ep[2 * j], i->ep[2 * j + 1])))
						goto bk;
				}
				if (i->rect.contains(vehi2->pt))
					goto bk;
				if (i->num == vehi2->num)
					goto bk;
			}
			V[count].push_back(vehi2);
		}
		else{
			bk:
			break;
		}
	}
	cout << "\nvehicle tracking is done.\n";
	destroyWindow("vehi");*/
}