#include "myVideo.h"
#include "Veh.h"

void myVideo::store2(const char* file, vector<vector<Veh *>> V) {
	if (edge.empty())
		edge = addedge();
	//_mkdir(file);
	string s = file;
	string s_del = "rm -r ";
	s_del += s + "/*";
	const char* todel = s_del.c_str();
	cout << s_del << "\n" << todel << endl;
	//system(todel);
	int n = 0, cnum = 0, count = 0;

	ofstream myfile;
	string f_csv = s;
	const char* f_cs = f_csv.c_str();
	myfile.open(f_cs);
	myfile << "i->num" << "," << "i->cx" << "," << "i->cy" << ","
		<< "i->rect.x" << "," << "i->rect.y" << "," << "i->rect.width" << ","
		<< "i->rect.height" << "," << "i->state" << ",i->ep[i],,,,,,,," << "time" << "\n";
	while (1) {
		if (count == V.size())
			break;
		Point2f pf[2];
		for (auto &i : V[count]) {
			if (i->state > 0 && i->rect.x >= edge.x) {
				myfile << i->num << "," << i->cx << "," << i->cy << "," << i->rect.x << "," << i->rect.y << "," << i->rect.width << ","
					<< i->rect.height << "," << i->state;
				for (int p = 0; p < 4; p++)
					myfile << "," << i->ep[2 * p] << "," << i->ep[2 * p + 1];
				myfile << "," << i->ct;
				myfile << "\n";
			}

		}
		count++;
	}
	cv::destroyAllWindows();
}

void myVideo::load2(const char* file, vector<vector<Veh *>> &V) {

	int num, x, y, w, h, state, count;
	float cx, cy, ep[8];
	string snum, sx, sy, sw, sh, sstate, scx, scy, sep[8], scount;
	ifstream datafile(file);
	getline(datafile, snum);
	int count0 = 0, precount = 0;
	vector<Veh *> V0;
	while (getline(datafile, snum, ',')) {
		if (snum.length() < 2) {
			getline(datafile, snum);
			continue;
		}
		getline(datafile, scx, ',');
		getline(datafile, scy, ',');
		getline(datafile, sx, ',');
		getline(datafile, sy, ',');
		getline(datafile, sw, ',');
		getline(datafile, sh, ',');
		getline(datafile, sstate, ',');
		for (int i = 0; i < 8; i++) {
			getline(datafile, sep[i], ',');
			ep[i] = stof(sep[i]);
		}
		getline(datafile, scount);
		num = stoi(snum);
		cx = stof(scx);
		cy = stof(scy);
		x = stoi(sx);
		y = stoi(sy);
		w = stoi(sw);
		h = stoi(sh);
		state = stoi(sstate);
		count = stoi(scount);

		Veh *vehi = new Veh(num, cx, cy, x, y, w, h, state, ep, count);	
		for (auto vehs : V0)
			if (vehs->num == vehi->num && count == precount)
				goto cont;
		if ((vehi->rect & block).area() > 0 || vehi->x > edge.x+edge.width || vehi->rect.width > 70 || !inEdge(vehi))
			continue;
		//copy here new.
		if (count > V.size() + 1) {
			V.push_back(V0);
			V0.clear();
			for (int i = 0; i < count - V.size() - 1; i++)
				V.push_back(V0);
		}
		if (count == precount) {
			if (!V.empty())
				for (int i = 0; i < V.back().size(); i++) {
					if (V.back()[i]->num == vehi->num) {
						vehi->pv = V.back()[i];
						V.back()[i]->nv = vehi;
						break;
					}
				}
			V0.push_back(vehi);
		}
		else {
			V.push_back(V0);
			V0.clear();
			if (!V.empty())
				for (int i = 0; i < V.back().size(); i++) {
					if (V.back()[i]->num == vehi->num) {
						vehi->pv = V.back()[i];
						V.back()[i]->nv = vehi;
						break;
					}
				}
			V0.push_back(vehi);
			precount = count;
		}
	cont:
		continue;
	}
	V.push_back(V0);
}