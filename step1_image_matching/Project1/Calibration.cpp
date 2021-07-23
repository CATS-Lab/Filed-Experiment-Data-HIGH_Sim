#include "myVideo.h"
#include "Veh.h"
void myVideo::cali(Mat frame) {
	string file = parapaths + "/cali.txt";
	Mat frame_rted;
	if (exists_file(file)) {
		ifstream input(file);
		string number;
		getline(input, number);
		AL = atof(number.c_str());
		Mat frame_rted;
		Calibration(frame, frame_rted);
		//Showframe("video", frame_rted, 5000);
	}
	else {
		namedWindow("video", WINDOW_NORMAL | WINDOW_KEEPRATIO);
		cv::moveWindow("video", WINDOWSXP, 0);
		cv::resizeWindow("video", 800, 1800);

		AL = 0;
		while (1) {
			//Showframe("video", frame);
			Calibration(frame, frame_rted);
			for (int i = 50; i < frame_rted.cols - 100; i += 100)
				cv::line(frame_rted, Point(i, 20), Point(i, frame_rted.rows - 20), Scalar(0, 0, 200), 3);
			cout << "click on t to set the angle\n";
			Showframe("video", frame_rted, 400, 0, 1800,800);
			char c = (char)waitKey(3000);//
			switch (c) {
			case 'z':
				AL -= 5 * PI / 180;
				break;
			case 'c':
				AL += 5 * PI / 180;
				break;
			case 'q':
				AL -= 2 * PI / 180;
				break;
			case 'e':
				AL += 2 * PI / 180;
				break;
			case 'a':
				AL -= 0.5 * PI / 180;
				break;
			case 'd':
				AL += 0.5 * PI / 180;
				break;
			}

			if (c == 't')
				break;

		}
		std::ofstream outfile(file);
		outfile << to_string(AL) << std::endl;
		outfile.close();
	}
	Calibration(frame, frame_rted);
	background = frame_rted.clone();
	destroyWindow("video");
	//Showframe("video", edgebackground);
	//destroyAllWindows();
}

void myVideo::Calibration(Mat frame, Mat &dst) {
	getangle();

	float Zy = (FF*cos(AL) + HS / HI * sin(AL) * (HI - HI / 2));
	float yrmax = HS / HI * (HI / 2) * HH / cos(AL) / Zy;
	Zy = (FF*cos(AL) - HS / HI * sin(AL) * (HI / 2));
	float yrmin = -HS / HI * (HI / 2.0) * HH / cos(AL) / Zy;
	float Z = -yrmin * sin(AL) + HH / cos(AL);
	float xrmin = -Z / FF * WS / WI * WI / 2;
	float xrmax = Z / FF * WS / WI * WI / 2;
	float xrr = xrmax - xrmin + 1;
	float yrr = yrmax - yrmin + 1;
	float yr, xr, yri, xri;
	int x_, y_;

	cv::Mat_<float> mapx_32f((int)HI, (int)WI), mapy_32f((int)HI, (int)WI);

	//Mat dst2((int)HI,(int)WI,  CV_8UC3, Scalar(0, 0, 0));
	for (int y = 0; y<frame.rows; ++y)
	{
		float *buff_mapx = ((float*)mapx_32f.data) + y*(int)WI;
		float *buff_mapy = ((float*)mapy_32f.data) + y*(int)WI;
		for (int x = 0; x<frame.cols; ++x)
		{
			cv::Mat_<float> pt(3, 1);
			pt(0) = x;
			pt(1) = y;
			pt(2) = 1;

			yri = y;
			xri = x;
			yr = yrr / HI * yri + yrmin;
			xr = xrr / WI * xri + xrmin;
			Zy = (FF*cos(AL) * yr / (-yr*sin(AL) + HH / cos(AL)));

			Z = -yr*sin(AL) + HH / cos(AL);

			pt(1) = y_ = HI / 2 + HI / HS * Zy;
			pt(0) = x_ = FF / Z * WI / WS * (xr)+WI / 2;

			pt /= pt(2);
			buff_mapx[x] = (int)pt(0);
			buff_mapy[x] = (int)pt(1);
			//Vec3b color = frame_org.at<Vec3b>(Point(x, y));
			//dst2.at<Vec3b>(Point(x_, y_)) = color;
		}
	}
	cv::convertMaps(mapx_32f, mapy_32f, map1_16u, map2_16u, CV_32FC2, true);
	//frame_r = dst2.clone();
	cv::remap(frame, dst, map1_16u, map2_16u, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
}

void myVideo::Calibrateframe(Mat fgmask_rot, Mat &fgmask_cali) {
	cv::remap(fgmask_rot, fgmask_cali, map1_16u, map2_16u, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
}

void myVideo::Calibrateframe(GpuMat fgmask, GpuMat &fgmask_cali) {
	Mat m1, m2;
	fgmask.download(m1);
	cv::remap(m1, m2, map1_16u, map2_16u, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

	fgmask_cali.upload(m2);
}

void myVideo::getangle() {
	float H, al, a, pi, x, b, bl, abl, r, focal, hs, ws;
	H = 400.0;
	pi = 3.14159;
	FF = focal = 8.8 / 1000.0*0.3048;
	HH = H;
	WS = 13.2*0.3048 / 1000.0;
	HS = 7.425*0.3048 / 1000.0;
}