#include "myVideo.h"
#include "Veh.h"

void myVideo::OperateOneFrame(Mat fgmask_rot, vector<vector<Veh*>> &Fig, int count, int &n, int cnum) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(fgmask_rot, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	vector<Veh*> Fig0;
	int hi;
	for (int i = 0; i < int(contours.size()); i++) {
		if (hierarchy[i][3] < 0) {
			if (contours[i].size() < 10)
				continue;
			float ty = static_cast<float>(contourArea(cv::Mat(contours[i]))); 
			Moments mu = moments(contours[i], false);
			Point2f mc = Point2f(static_cast<float>(mu.m10 / mu.m00), static_cast<float>(mu.m01 / mu.m00));
			mc = Point2f(max(0.0f, mc.x), max(0.0f, mc.y)); 
			Rect rect = boundingRect(contours[i]);
			RotatedRect rt = minAreaRect(contours[i]);
			float angle = rt.angle;
			Veh* v = new Veh(n++, ty, rt, cnum, count, angle);
			Point2f pf[4];
			rt.points(pf);
			for (int k = 0; k < 4; k++) {
				v->ep[2 * k] = pf[k].x;
				v->ep[2 * k + 1] = pf[k].y;
				if (cv::norm(pf[k] - pf[(k + 1) % 3]) > 1500)
					ty = 10;
			}
			if (ty < MINCONAREA || ty > MAXCONAREA)
				continue;
			//if (count > 160)
				//rectangle(fgmask_rot, rect, Scalar(255), 10, 8, 0);
			Fig0.push_back(v);
		}
	}
	if (count == 0) {
		Fig.push_back(Fig0);
	}
	else {
		vector<Veh*> Fb = Fig.back();
		double disu = 50.0;
		double d1, d2;
		int k;
		for (int i = 0; i < int(Fig0.size()); i++) {
			d2 = disu;
			for (int j = 0; j < int(Fb.size()); j++) {
				d1 = Vehdis(Fig0[i], Fb[j]);
				if (d1 < d2) {
					d2 = d1;
					k = j;
				}
			}
			int newv = 1;
			if (d2 < disu) {
				if (Fb[k]->nv) {
					float a1, a2, a3;
					a1 = Fb[k]->vtype;
					a2 = Fb[k]->nv->vtype;
					a3 = Fig0[i]->vtype;
					float area1, area2;
					area1 = (a1 - a2) * (a1 - a2);
					area2 = (a1 - a3) * (a1 - a3);
					if (area1 < area2)
						newv = 0;
					else
						newv = 2;
				}
				if (newv) {
					if (newv == 2) {
						Fb[k]->nv->pv = NULL;
						//Fb[k]->nv = NULL;
					}
					Fig0[i]->pv = Fb[k];
					Fb[k]->nv = Fig0[i];
					Fig0[i]->num = Fb[k]->num;
					Fig0[i]->vtype = (Fb[k]->vtype + Fig0[i]->vtype) / 2;
					Fb[k]->vtype = Fig0[i]->vtype;
					if (!Fig0[i]->state) {
						Veh* b;
						b = Fig0[i];
						b->forcast[0] = 2 * b->cx - b->pv->cx;
						b->forcast[1] = 2 * b->cy - b->pv->cy;
						b->state = 1;
					}
				}
			}
		}
		Fig.push_back(Fig0);
		Fig0.clear();
	}
}

void myVideo::RefreshOutput(vector<vector<Veh*>> &Fig) {
	for (int i = 0; i < Fig.size(); i ++) {
		for (int j = 0; j < Fig[i].size(); j++) {
			Veh* v;
			v = Fig[i][j];
			if (v->time == 0) {
				int ti = 1;
				while (v->nv) {
					v = v->nv;
					ti++;
				}
				while (v) {
					v->time = ti;
					v = v->pv;
				}
			}
		}
	}
	for (auto &i : Fig) {
		for (int j = 0; j < int(i.size()); j++) {
			if (i[j]->nv)
				if (i[j]->time != i[j]->nv->time)
					cout << "wrong";
				else if (i[j]->nv->pv == NULL)
					cout << "nopv";
				else if (i[j]->nv->num != i[j]->num)
					cout << "number not equal";
			if (i[j]->time < MINFRAMELEN)
				i.erase(i.begin() + j--);
		}
	}
}