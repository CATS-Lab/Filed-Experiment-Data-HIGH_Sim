#include "FRAMES.h"
#include <iostream>

int main()
{	
	string gen_path[3] = {"N:/LEFT/","L:/MIDDLE/","N:/RIGHT/" };
	
	//1.
	//FRAMES.get_motion_parameters();
	//2.
	//string path = "verify/to_show.csv";
	//FRAMES.test_circles(path);
	//3.test angle paramters and generate rotated imgs.
	//
	
	string path[2] = { "temp_data/117700.csv", "temp_data/206700.csv" };
	int order[3] = { 1,9,2 };
	//FRAMES.test_angle(path[1]);
	//4. generate learning imgs, with W nad H;
	int learn_w = 480, learn_h = 800;
	string cnn_path = "C:/Users/dongfangzhao/Documents/OPENCV/darknet-master/build/darknet/x64/backup/yolov3_I75_left_last.weights";
	for(int i = 0, imgnum = 0; i < 3; i ++){
		for(int j = 1; j < 2; j ++){
			FRAMES FRAMES(gen_path[i]);
			FRAMES.ratio_frames = FRAMES.frame_ratios[j][i];
			FRAMES.EDGE_FIT_ORDER = order[i];
			FRAMES.iffirst = j;
			cout << "\n" << i << j <<":"<< endl;
			FRAMES.detect_lane_marker(i, j, path[j]);
			return 1;
		}
	}
	
	//5. generate learning data
	/*
	FRAMES FRAMES(gen_path[0]);
	string imgpath = "learning_imgs/imgs/";
	string darknet = "C:/Users/dongfangzhao/Documents/OPENCV/darknet-master/build/darknet/x64/";
	string gen_cnn_path = darknet + "backup/I75/all";
	string learning_img_path = darknet + "data/obj/";
	int interval = 1;
	string last_cnn_path = gen_cnn_path + "_" + to_string(interval * 10) + "/";
	string cnn_path = gen_cnn_path + "_" + to_string(interval) + "/";

	FRAMES.generate_learning_data(imgpath, last_cnn_path, cnn_path, learning_img_path, interval);
*/
	return 0;
} 
