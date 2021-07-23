
#include "FRAMES.h"

void FRAMES::load_circle_parameter(string path) {
	ifstream ifstr(path.c_str());
	string snum;
	while (getline(ifstr, snum, ',')) {
		int i = 0, x = 0, y = 0;
		vfnum.push_back(stoi(snum));
		vector<Point2i> points;
		vector<int> rad;
		while (i < 9) {
			getline(ifstr, snum, ',');
			switch (i++ % 3) {
			case 0:
				x = stoi(snum);
				break;
			case 1:
				y = stoi(snum);
				points.push_back(Point2i(x, y));
				break;
			case 2:
				rad.push_back(stoi(snum));
				break;
			}
		}
		getline(ifstr, snum);
		vector_c_p.push_back(points);
		vector_c_r.push_back(rad);
	}
}