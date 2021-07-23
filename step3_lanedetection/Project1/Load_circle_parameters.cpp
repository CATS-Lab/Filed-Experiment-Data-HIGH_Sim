
#include "FRAMES.h"

void FRAMES::load_circle_parameters(string path, vector<int> &fnum, vector<vector<Point>> &centers, int sti, int stj, int detect) {
	ifstream ifstr(path.c_str());
	string snum;
	getline(ifstr, snum, ',');
	while (1) {
		if (snum.size() < 1)
			break;
		fnum.push_back(stoi(snum));
		int i = 0, x = 0, y = 0, r = 0;
		string xs, ys, rs;
		vector<Point> center;
		while (1) {
			getline(ifstr, xs, ',');
			if (xs[0] == '\n') {
				centers.push_back(center);
				xs.erase(xs.begin());
				snum = xs;
				break;
			}
			x = stoi(xs);
			getline(ifstr, ys, ',');
			y = stoi(ys);
			getline(ifstr, rs, ',');
			r = stof(rs);
			center.push_back(Point(x, y));
		}
	}

	for (auto &ca : centers)
		for (auto & ci : ca)
			ci = ci + Point(IMGW / 2, IMGH / 2);
	if (sti == 2 && stj == 1)
		for (auto &ca : centers) {
			if (ca.size() == 4)
				ca.erase(ca.begin() + 2);
		}
	if(detect == 1){
		vector<int> fnum2;
		vector<vector<Point>> center2;
		for (int i = 0, ni = fnum[0]; i < fnum.size(); ni++) {
			int n = fnum[i];
			if (ni == n) {
				fnum2.push_back(ni);
				center2.push_back(centers[i]);
				i++;
			}
			else {
				fnum2.push_back(ni);
				vector<Point> pi = centers[i-1];
				double k = ni - fnum[i-1], kn = fnum[i] - fnum[i-1];
				for (int j = 0; j < pi.size(); j++) {
					pi[j].x = double(centers[i][j].x - pi[j].x) * k / kn + pi[j].x;
					pi[j].y = double(centers[i][j].y - pi[j].y) * k / kn + pi[j].y;
				}
				center2.push_back(pi);
			}
		}
		fnum = fnum2;
		centers = center2;
	}
}

