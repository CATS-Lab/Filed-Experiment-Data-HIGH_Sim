#include "myVideo.h"
#include "Veh.h"

using namespace std::this_thread;
using namespace std::chrono;

vector<vector<Veh*>> myVideo::extractBackground(int startnum) {
	string s = outputpaths;
	string s_del = "rm -rf ";
	s_del += s + "/*.jpg";
	const char* todel = s_del.c_str();
	cout << s_del << "\n" << todel << endl;
	system(todel);
	int n = 0, cnum = 0, count0 = 0, count = startnum;

	vector<vector<Veh*>> Fig;
	Mat frame_org, fgmask, fgmask_cali_rot, fgmask_cali, frame, fgimg, bgimg, contours, frame_cali, frame_cali_rot;
	GpuMat d_fgmask, d_fgimg, d_bgimg, d_bgimg_rot, d_fgmask_rot, d_frameb, d_frame, d_frame_cali, d_bg_cali;
	Stream stream;


	int GPU_work = getCudaEnabledDeviceCount();
	if (GPU_work)
		cout << "The GPU is working with OPENCV" << endl;
	string fname = "s";
	int Win = 300, Hin = 600;
	namedWindow(fname, WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow(fname, WINDOWSXP, WINDOWSYP);
	cv::resizeWindow(fname, Win, Hin);
	//cv::moveWindow(fname, WINDOWSXP + mov, WINDOWSYP);

	

	while (1) {
		cout << count << "--";
		readvehframe(frame_org, count+1);
		//cv::flip(frame_org, frame_org, -1);
		if (count == startnum) {
			cali(frame_org);
			CalculateVideoDirection(Fig);
		}
		Calibrateframe(frame_org, frame_cali);

		Size ms = frame_org.size();
		d_frame_cali.upload(frame_cali, stream);


		//TODO: load area to match in rotation
		//FindRotationParameter(d_frame, d_frame_cali, fgmask_cali_rot, count, cnum);
		float rotp[3] = { 0,0,0 };

		RotateFrame(d_frame_cali, d_fgmask_rot, rotp, count);
		d_fgmask_rot.download(frame_cali_rot);


		if(count%100 == 0){
			imshow(fname, frame_cali_rot);
			waitKey(10);
		}
		saveframe(frame_cali_rot, count, count0);

		//OperateOneFrame(fgmask_rot_cali, Fig, count, n, cnum);


		if (count == OPFRAMENUM + 200)
			break;
		count++;
	}
	//writerotparas();

	cv::destroyAllWindows();
	return Fig;
}
