#include "FRAMES.h"

void create_learning_name_file(string imgpath, int freq){
	string name_file_path = "C:/Users/dongfangzhao/Documents/OPENCV/darknet-master/build/darknet/x64/data/";

	Public_func pf;
	string train_file = name_file_path + "train.txt";
	string test_file = name_file_path + "test.txt";
	ofstream trainfile, testfile;
	trainfile.open(train_file);
	testfile.open(test_file);
	string string0 = "data/obj/";
	for (int i = 0; pf.exists_file(imgpath + to_string(i*freq) + ".jpg"); i++) {
		if (i % 5)
			trainfile << string0 + to_string(i) + ".jpg\n";
		else 
			testfile << string0 + to_string(i) + ".jpg\n";
	}
}

std::vector<std::string> objects_names_from_file(std::string const filename) {
	std::ifstream file(filename);
	std::vector<std::string> file_lines;
	if (!file.is_open()) return file_lines;
	for (std::string line; getline(file, line);) file_lines.push_back(line);
	std::cout << "object names loaded \n";
	return file_lines;
}

void FRAMES::generate_learning_data(string imgpath, string last_cnn_path, string cnn_path, string learning_img_path, int freq) {
	imgpath = gen_path + imgpath;
	create_learning_name_file(imgpath, freq);
	Public_func pf;
	string dele = "rm -r " + learning_img_path + "*";
	//system(dele.c_str());


	namedWindow("video", WINDOW_NORMAL | WINDOW_KEEPRATIO);
	cv::moveWindow("video", 0, 0);
	cv::resizeWindow("video", 480, 960);

	std::string genpath = "C:/Users/dongfangzhao/Documents/Opencv/darknet-master/build/darknet/x64";
	std::string  names_file = genpath + "/data/obj.names";
	std::string  cfg_file = genpath + "/yolov3_I75_left.cfg";
	std::string  weights_file = last_cnn_path + "yolov3_I75_left_last.weights";

	LIB_API::Detector yolo(cfg_file, weights_file, 0);
	auto obj_names = objects_names_from_file(names_file);
	vector<bbox_t> bbox0;
	for (int i = 0; pf.exists_file(imgpath + to_string(i*freq) + ".jpg"); i++) {
		string img_i_path = imgpath + to_string(i*freq) + ".jpg";
		Mat img;
		int imgnum = i*freq;
		load_frame(img_i_path, img);
		string img_path = learning_img_path + to_string(i) + ".jpg";
		cv::imwrite(img_path, img);

		string txt_path = learning_img_path + to_string(i) + ".txt";
		cout << i << "#";
		bbox0 = yolo.detect(img, 0.5, true);
		writedata(txt_path, img, bbox0);
	}
}




