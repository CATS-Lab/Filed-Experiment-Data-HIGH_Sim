#include "FRAMES.h"
#include <iostream>

int main()
{	
	string gen_path = "L:/MIDDLE/";
	FRAMES FRAMES(gen_path);
	//TODO:

	//1.
	FRAMES.get_motion_parameters(10000, 5);
	//2.
	//string path = "verify/to_show.csv";
	//FRAMES.test_circles(path);
	//3.
	//string path[2] = { "mid_processing/para_1_11400.csv", "mid_processing/para_138000_216000.csv" };
	//FRAMES.test_angle(path[1]);
	//FRAMES.test_angle(path[0]);
	return 0;
}
