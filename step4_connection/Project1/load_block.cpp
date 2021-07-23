#include "myVideo.h"
#include "Veh.h"
Mat blockground;
string mousewinname;
void Cross_block(cv::Mat frame, cv::Point point, cv::Scalar s) {
	int cl = 10;
	cv::Point a(cl, 0), b(-cl, 0), c(0, cl), d(0, -cl);
	line(frame, point + a, point + b, s, 4);
	line(frame, point + c, point + d, s, 4);
}

void Mouseclick_block(int evt, int x, int y, int flags, void* edgepoints) {
	cv::Scalar s(0, 0, 255);
	std::vector<cv::Point>* edgep = (std::vector<cv::Point>*)edgepoints;
	if (evt == CV_EVENT_LBUTTONDOWN) {
		edgep->push_back(Point(x, y));
		if (edgep->size() >= 1) {
			Point tem = edgep->back();
			Cross(blockground, tem, s);
		}
		imshow(mousewinname, blockground);
	}
}

Rect myVideo::load_block() {
	vector<cv::Point> blockpoints;
	string file = parapaths + "/block.txt";
	Rect output;
	blockground = background.clone();
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
		output = Rect(paras[0], paras[1], paras[2], paras[3]);
	}
	else {
		namedWindow("Background", WINDOW_NORMAL | WINDOW_KEEPRATIO);
		cv::moveWindow("Background", 50, 50);
		cv::resizeWindow("Background", 1500, 750);
		cv::setMouseCallback("Background", Mouseclick_block, &blockpoints);
		imshow("Background", blockground);
		mousewinname = "Background";
		char c = waitKey(0);
		if (c == 't') {
			if (blockpoints.size() > 3) {
				vector<Point> outpoints(blockpoints.end() - 4, blockpoints.end());
				output = boundingRect(outpoints);
			}

			else {
				int x = 0.15 * blockground.cols, y = 0.05 * blockground.rows;
				int w = 0.7 * blockground.cols, h = 0.93 * blockground.rows;
				output = Rect(x, y, w, h);
				cout << "x" << x << "y" << y << "w" << w << "h" << h;
			}
		}
		output.x = max(50, output.x);
		output.height = min(YPIXEL - 50 - output.y, output.height);
		rectangle(blockground, output, Scalar(255), 5, 6, 0);
		imshow("Background", blockground);
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




Rect myVideo::load_rect_etc(Mat frame_rted) {
	cout << "loading rect etc" << endl;
	vector<cv::Point> blockpoints;
	string file = parapaths + "/rect_etc.txt";
	Rect output;
	blockground = frame_rted.clone();
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
		output = Rect(paras[0], paras[1], paras[2], paras[3]);
	}
	else {
		string name = "rect_etc";
		mousewinname = name;
		namedWindow(name, WINDOW_NORMAL | WINDOW_KEEPRATIO);
		cv::moveWindow(name, WINDOWSXP, 0);
		cv::resizeWindow(name, 400, 800);
		cv::setMouseCallback(name, Mouseclick_block, &blockpoints);
	reshow:
		imshow(name, blockground);
		char c = waitKey(0);
		if (c == 't') {
			if (blockpoints.size() > 3) {
				output = boundingRect(blockpoints);
			}
			else {
				cout << "rect_etc is not correctly loaded" << endl;
				blockground = frame_rted.clone();
				blockpoints.clear();
				goto reshow;
			}
		}
		rectangle(blockground, output, Scalar(255), 5, 6, 0);
		imshow(name, blockground);
		c = waitKey(2000);
		std::ofstream outfile(file);
		outfile << to_string(output.x) << std::endl;
		outfile << to_string(output.y) << std::endl;
		outfile << to_string(output.width) << std::endl;
		outfile << to_string(output.height) << std::endl;
		outfile.close();
		destroyWindow(name);
	}
	return output;
}
