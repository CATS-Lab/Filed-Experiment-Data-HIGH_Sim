
#include "FRAMES.h"

FRAMES::FRAMES(string g_path) {
	gen_path = g_path;
	frame_path = gen_path + "imgs/";
	output_data_path = gen_path + "output/";
	temp_data_path = gen_path + "temp_data/";
	temp_frame_path = gen_path + "temp_frame/";

	if (!PF.exists_file(output_data_path))
		PF.create_file(output_data_path);
	if (!PF.exists_file(temp_data_path))
		PF.create_file(temp_data_path);
	if (!PF.exists_file(temp_frame_path))
		PF.create_file(temp_frame_path);

}

