#include "myVideo.h"
#include "Veh.h"

void myVideo::Points(vector<vector<Veh*> > V) {
	Mat img(YPIXEL, XPIXEL, CV_8UC3, Scalar(255, 255, 255));
	int num = 0;
	std::ofstream mycsv;
	mycsv.open("output.csv");
	for (auto &i : V) {
		for (auto &j : i) {
			mycsv << j->num << "," << j->glotime << "," << j->cx << "," << j->cy << "," << j->lnum << "," << j->vtype << ",\n";
			num++;
			//if (num)// % 2 == 0)
			circle(img, cv::Point2f(j->cx, j->cy), 2, Scalar(0, 0, 0), 2, 8, 0);
			circle(background, cv::Point2f(j->cx, j->cy), 2, Scalar(0, 0, 255), 2, 8, 0);
		}
	}
	mycsv.close();
	namedWindow("Points", WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow("Points", WINDOWSXP, WINDOWSYP);
	cv::resizeWindow("Points", WINDOWSW, WINDOWSH);
	imshow("Points", img);                   // Show our image inside it.
	imwrite("a.jpg", img);
	imwrite("b.jpg", background);
	waitKey(3000);             
	cv::destroyAllWindows();
}
