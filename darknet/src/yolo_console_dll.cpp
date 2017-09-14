#include <iostream>
#include <iomanip> 
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>
#include <conio.h>
#include <cstdint>

#define OPENCV

#include "yolo_v2_class.hpp"	// imported functions from DLL

#ifdef OPENCV
#include <opencv2/opencv.hpp>			// C++
#include "opencv2/core/version.hpp"
#ifndef CV_VERSION_EPOCH
#include "opencv2/videoio/videoio.hpp"
#pragma comment(lib, "opencv_world330.lib")  
#else
#pragma comment(lib, "opencv_core2413.lib")  
#pragma comment(lib, "opencv_imgproc2413.lib")  
#pragma comment(lib, "opencv_highgui2413.lib") 
#endif


extern "C" void __declspec(dllexport) __stdcall draw_boxes(cv::Mat mat_img, std::vector<bbox_t> result_vec, std::vector<std::string> obj_names, unsigned int wait_msec = 0) {
	for (auto &i : result_vec) {
		cv::Scalar color(60, 160, 260);
		cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 5);
		if (obj_names.size() > i.obj_id) {
			std::string obj_name = obj_names[i.obj_id];
			if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
			cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
			int const max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
			cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 3, 0), std::max((int)i.y - 30, 0)), 
				cv::Point2f(std::min((int)i.x + max_width, mat_img.cols-1), std::min((int)i.y, mat_img.rows-1)), 
				color, CV_FILLED, 8, 0);
			putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(0, 0, 0), 2);
		}
	}
	/*cv::imshow("window name", mat_img);
	cv::waitKey(wait_msec);*/
}
#endif	// OPENCV


extern "C" void __declspec(dllexport) __stdcall show_result(std::vector<bbox_t> const result_vec, std::vector<std::string> const obj_names) {
	for (auto &i : result_vec) {
		if (obj_names.size() > i.obj_id) std::cout << obj_names[i.obj_id] << " - ";
		std::cout << "obj_id = " << i.obj_id << ",  x = " << i.x << ", y = " << i.y 
			<< ", w = " << i.w << ", h = " << i.h
			<< std::setprecision(3) << ", prob = " << i.prob << std::endl;
	}
}

extern "C" std::vector<std::string> __declspec(dllexport) __stdcall objects_names_from_file(std::string const filename) {
	std::ifstream file(filename);
	std::vector<std::string> file_lines;
	if (!file.is_open()) return file_lines;
	//for(std::string line; file >> line;) file_lines.push_back(line);

	for (std::string line; getline(file, line); )
	{
		file_lines.push_back(line);
	}

	std::cout << "object names loaded \n";
	return file_lines;
}

typedef void(*FuncPtr)(const char *);
FuncPtr Debug;

extern "C" void __declspec(dllexport) __stdcall SetDebugFunction(FuncPtr fp)
{
	Debug = fp;
}

extern "C" void __declspec(dllexport) __stdcall Test(char* s) {
	Debug(s);
}

std::unique_ptr<Detector> detector;
std::vector<std::string> obj_names;
std::string filename;
extern "C" void __declspec(dllexport) __stdcall Init(char* cfg, char* weights, char* names, char* file = '\0') {
	detector.reset(new Detector(cfg, weights));
	obj_names = objects_names_from_file(names);
	filename = std::string(file);
}

cv::VideoCapture capture;
extern "C" int __declspec(dllexport) __stdcall Open(int camIdx = 0, int width = 640, int height = 480) {
	bool opened = capture.open(camIdx);

	if (!opened) {
		return -1;
	}

	capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
	capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall TestShow() {
	cv::Mat frame;
	capture >> frame;

	cv::imshow("Test", frame);
}

extern "C" void __declspec(dllexport) __stdcall Close() {
	capture.release();
}

extern "C" bool __declspec(dllexport) __stdcall TestArray(int* test) {
	test[0] = 234;
	test[1] = 34;
	test[2] = 90;
	test[3] = 78;


	return true;
}

extern "C" __declspec(dllexport) bool __stdcall Detect(uchar* data, char* type, float thresh = 0.200000003F, bool use_mean = false) //int main(int argc, char *argv[])
{
	/*Debug("config:");
	Debug(cfg);
	Debug("weights:");
	Debug(weights);
	Debug("names:");
	Debug(names);*/
	Debug("type:");
	Debug(type);
	/*Debug("width:");
	char buffer[33];
	Debug(_itoa(width, buffer, 10));
	Debug("height:");
	Debug(_itoa(width, buffer, 10));*/

	//std::string filename;
	//if (argc > 1) filename = argv[1];
	//std::string filename(file);

	cv::Mat resFrame;
	//Detector detector("yolo-voc.cfg", "yolo-voc.weights");
	//Detector detector(cfg, weights);

	Debug("constructor works");

	//auto obj_names = objects_names_from_file("data/voc.names");
	//auto obj_names = objects_names_from_file(names);

	/*while (true) 
	{*/
		char str[256] = "";
		snprintf(str, sizeof(str), "%zu", filename.size());
		Debug(str);
		//std::cout << "input image or video filename: ";
		/*if(filename.size() == 0) std::cin >> filename;
		if (filename.size() == 0) break;*/
		
		try {
#ifdef OPENCV
			std::string const file_ext = filename.substr(filename.find_last_of(".") + 1);
			std::string const protocol = filename.substr(0, 7);

			Debug("before if");
			if (std::string(type) == "camera") {

				Debug("inside camera");
				//cv::VideoCapture cap;
				//capture.open(0);

				// -----continuously process frames
				//cv::Mat frame, prev_frame, det_frame;
				//std::vector<bbox_t> result_vec, thread_result_vec;
				//detector.nms = 0.02;	// comment it - if track_id is not required
				//std::thread td([]() {});
				//std::atomic<int> ready_flag;
				//ready_flag = true;

				//while(!_kbhit()) {
				//	cap >> frame;

				//	if (ready_flag || (protocol != "rtsp://" && protocol != "http://" && protocol != "https:/")) {
				//		td.join();
				//		ready_flag = false;
				//		result_vec = thread_result_vec;
				//		result_vec = detector.tracking(result_vec);	// comment it - if track_id is not required
				//		det_frame = frame;
				//		td = std::thread([&]() { thread_result_vec = detector.detect(det_frame, 0.24, true); ready_flag = true; });
				//	}
				//	if (!prev_frame.empty()) {
				//		draw_boxes(prev_frame, result_vec, obj_names, 3);
				//		show_result(result_vec, obj_names);
				//	}
				//	prev_frame = frame;
				//}

				// -----process single frame

				Debug("going to process frame");
				cv::Mat frame;

				capture >> frame;
				cv::flip(frame, frame, 1);

				std::vector<bbox_t> result_vec = detector->detect(frame, thresh, use_mean);
				result_vec = detector->tracking(result_vec);	// comment it - if track_id is not required
				draw_boxes(frame, result_vec, obj_names);
				//show_result(result_vec, obj_names);

				Debug("frame processed");

				resFrame = frame;
			}
			else if (type == "video" && (file_ext == "avi" || file_ext == "mp4" || file_ext == "mjpg" || file_ext == "mov" || 	// video file
				protocol == "rtsp://" || protocol == "http://" || protocol == "https:/"))	// video network stream
			{
				cv::Mat frame, prev_frame, det_frame;
				std::vector<bbox_t> result_vec, thread_result_vec;
				detector->nms = 0.02;	// comment it - if track_id is not required
				std::thread td([]() {});
				std::atomic<int> ready_flag;
				ready_flag = true;
				for (cv::VideoCapture cap(filename); cap >> frame, cap.isOpened();) {
					if (ready_flag || (protocol != "rtsp://" && protocol != "http://" && protocol != "https:/")) {
						td.join();
						ready_flag = false;
						result_vec = thread_result_vec;
						result_vec = detector->tracking(result_vec);	// comment it - if track_id is not required
						det_frame = frame;						
						td = std::thread([&]() { thread_result_vec = detector->detect(det_frame, thresh, use_mean); ready_flag = true; });
					}
					if (!prev_frame.empty()) {						
						draw_boxes(prev_frame, result_vec, obj_names, 3);
						show_result(result_vec, obj_names);
					}
					prev_frame = frame;
				}
			}
			else if (type == "imagelist" && file_ext == "txt") {	// list of image files
				std::ifstream file(filename);
				if (!file.is_open()) std::cout << "File not found! \n";
				else 
					for (std::string line; file >> line;) {
						std::cout << line << std::endl;
						cv::Mat mat_img = cv::imread(line);
						std::vector<bbox_t> result_vec = detector->detect(mat_img, thresh, use_mean);
						show_result(result_vec, obj_names);
						//draw_boxes(mat_img, result_vec, obj_names);
						//cv::imwrite("res_" + line, mat_img);
					}
				
			}
			else if(type == "image") {	// image file
				cv::Mat mat_img = cv::imread(filename);
				std::vector<bbox_t> result_vec = detector->detect(mat_img, thresh, use_mean);
				result_vec = detector->tracking(result_vec);	// comment it - if track_id is not required
				draw_boxes(mat_img, result_vec, obj_names);
				show_result(result_vec, obj_names);
			}

			//uchar* result = new uchar[resFrame.step[0] * resFrame.rows]; //resFrame.step[0] * resFrame.rows //resFrame.cols * resFrame.rows * 3
			/*std::vector<uchar> result;
			result.assign(resFrame.datastart, resFrame.dataend);*/
			memcpy(data, resFrame.data, resFrame.step[0] * resFrame.rows);
			/*for (int i = 0; i < 640 * 480 * 3; i++) {
				data[i] = resFrame.data[i];
			}*/
			/*char buffer[33];
			Debug(_itoa(result.size(), buffer, 10));*/
			Debug("returning result");

			return true;
#else
			//std::vector<bbox_t> result_vec = detector.detect(filename);

			auto img = detector.load_image(filename);
			std::vector<bbox_t> result_vec = detector.detect(img);
			detector.free_image(img);
			show_result(result_vec, obj_names);
#endif			
		}
		catch (std::exception &e) { std::cerr << "exception: " << e.what() << "\n"; getchar(); }
		catch (...) { std::cerr << "unknown exception \n"; getchar(); }
		filename.clear();
	//}

	//return 0;
}