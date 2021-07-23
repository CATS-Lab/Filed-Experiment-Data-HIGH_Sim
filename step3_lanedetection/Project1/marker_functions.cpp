#include "FRAMES.h"

void FRAMES::get_sub_imgs(GpuMat gmat, vector<Point2i> locs, vector<int> edge, Point2i size, vector<GpuMat> &output, vector<Rect> &vrects) {
	Point outerp(size.x + size.y, size.x + size.y);
	Mat rotmat;
	GpuMat boundmat, bound_rot, gmati;
	for (int i = 0; i < locs.size(); i++) {
		Point2i p = locs[i];
		int len = 14;
		double angle = atan(double(edge[p.y + 60] - edge[p.y]) / double(60)) * 180 / PI;
		//Mat rot_mat = getRotationMatrix2D(Point(x0, y0), -angle * 180 / PI, 1);
		Rect bound(p - outerp, p + outerp);
		boundmat = gmat(bound);
		//vrects.push_back(bound);
		rotmat = getRotationMatrix2D(outerp, -angle, 1);
		cuda::warpAffine(boundmat, bound_rot, rotmat, boundmat.size());
		Rect cut(outerp - Point2i(size.x / 2, size.x / 2), outerp + Point2i(size.x / 2, len - size.x / 2));
		gmati = bound_rot(cut);
		output.push_back(gmati.clone());
		//vrects.push_back(bound);
	}
}


void FRAMES::detect_markers(GpuMat gmat, vector<Point2i> &forcasted_p, vector<int> edge, Point2i size, vector<Point2i> &ori_p, vector<Point2i> &matched_p, vector<int> &match_bool, int &acu, vector<Rect> &vrects) {
	Mat frame;
	gmat.download(frame);
	
	Mat rotmat, vmr;
	GpuMat boundmat, bound_rot, gmati, g_res;

	cv::Ptr<cv::cuda::TemplateMatching> ptr;
	int match_method = CV_TM_CCOEFF_NORMED;// CV_TM_CCOEFF;
	ptr = cuda::createTemplateMatching(CV_8UC3, match_method, Size(0, 0));
	for (int j = 0; j < forcasted_p.size(); j++) {
		Point outerp(marker_img[j].rows*3+marker_img[j].cols, marker_img[j].rows*3 + marker_img[j].cols);
		Point2i orip = forcasted_p[j];
		int bdg = 0;
		if (orip.y >= gmat.rows - 50 || orip.x >= gmat.cols - 50 || orip.x <= 50 || orip.y < 50)
			bdg = 1;
		Vec3b cor;
		if (!bdg)
			cor = frame.at<Vec3b>(orip);
		if (bdg || cor[0] == 0) {
			matched_p.push_back(forcasted_p[j]);
			match_bool.push_back(0);
			continue;
		}

		Point p = forcasted_p[j];
		double angle = -atan(double(edge[p.y + 60] - edge[p.y]) / double(60)) * 180 / PI;
		//Mat rot_mat = getRotationMatrix2D(Point(x0, y0), -angle * 180 / PI, 1);
		Rect bound(p - outerp, p + outerp);
		bound.x = max(1, bound.x);
		bound.y = max(1, bound.y);
		boundmat = gmat(bound);

		rotmat = getRotationMatrix2D(outerp, angle, 1);
		cuda::warpAffine(boundmat, bound_rot, rotmat, boundmat.size());
		Rect cut(outerp - Point2i(size.x / 2, marker_img[j].rows / 2), outerp + Point2i(size.x / 2, 1.5 * marker_img[j].rows + size.x/2));
		gmati = bound_rot(cut);
		ptr->match(gmati, marker_img[j], g_res);
		Mat match_mat;
		g_res.download(match_mat);
		g_res.release();


		Mat mker, outr;
		gmati.download(outr);
		marker_img[j].download(mker);

		//imwrite(temp_frame_path + "0_markeri.jpg", mker);
		//(temp_frame_path + "0_mouti.jpg", outr);



		float score = 0;
		Point2i matchLoc;
		if (!match_mat.empty()) {
			double minVal; double maxVal; Point minLoc; Point maxLoc;
			cv::minMaxLoc(match_mat, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
			maxLoc += Point2i(cut.x, cut.y);
			vmr = getRotationMatrix2D(outerp, -angle, 1);
			maxLoc += Point2i(marker_img[j].cols / 2, marker_img[j].cols / 2);
			matchLoc.x = maxLoc.x*vmr.at<double>(0, 0) + maxLoc.y*vmr.at<double>(0, 1) + vmr.at<double>(0, 2);
			matchLoc.y = maxLoc.x*vmr.at<double>(1, 0) + maxLoc.y*vmr.at<double>(1, 1) + vmr.at<double>(1, 2);
			score = maxVal;
		}
		if (score > 0.8) {//
			matchLoc += Point(bound.x, bound.y);
			matched_p.push_back(matchLoc);
			match_bool.push_back(1);
			vrects.push_back(Rect(matchLoc - Point(4, 4), matchLoc + Point(4, 4)));
			acu++;
		}
		else {
			matched_p.push_back(forcasted_p[j]);
			match_bool.push_back(0);
		}
	}


}


