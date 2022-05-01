#include <fstream>
#include <sstream>
#include <iostream>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

std::string keys =
	"{ help  h     | | Print help message. }"
	"{ @alias      | | An alias name of model to extract preprocessing parameters from models.yml file. }"
	"{ zoo         | models.yml | An optional path to file with preprocessing parameters }"
	"{ device      |  0 | camera device number. }"
	"{ input i     | | Path to input image or video file. Skip this argument to capture frames from a camera. }"
	"{ framework f | | Optional name of an origin framework of the model. Detect it automatically if it does not set. }"
	"{ classes     | | Optional path to a text file with names of classes to label detected objects. }"
	"{ thr         | .5 | Confidence threshold. }"
	"{ nms         | .4 | Non-maximum suppression threshold. }"
	"{ backend     |  0 | Choose one of computation backends: "
						 "0: automatically (by default), "
						 "1: Halide language (http://halide-lang.org/), "
						 "2: Intel's Deep Learning Inference Engine (https://software.intel.com/openvino-toolkit), "
						 "3: OpenCV implementation, "
						 "4: VKCOM, "
						 "5: CUDA }"
	"{ target      | 0 | Choose one of target computation devices: "
						 "0: CPU target (by default), "
						 "1: OpenCL, "
						 "2: OpenCL fp16 (half-float precision), "
						 "3: VPU, "
						 "4: Vulkan, "
						 "6: CUDA, "
						 "7: CUDA fp16 (half-float preprocess) }"
	"{ async       | 0 | Number of asynchronous forwards at the same time. "
						"Choose 0 for synchronous mode }";

using namespace cv;
using namespace dnn;

float confThreshold, nmsThreshold;
std::vector<std::string> classes;

inline void preprocess(const cv::Mat& frame, Net& net, Size inpSize, float scale, const cv::Scalar& mean, bool swapRB);

void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& out, Net& net, int backend);

void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);

void callback(int pos, void* userdata);

int main(int argc, char** argv)
{
	CommandLineParser parser(argc, argv, keys);

	const std::string modelName = parser.get<String>("@alias");
	const std::string zooFile = parser.get<String>("zoo");

	parser = CommandLineParser(argc, argv, keys);

	// Confidence threshold.
	confThreshold = 0.5;
	
	// Non-maximum suppression threshold.
	nmsThreshold = 0.4;

	// Preprocess input image by multiplying on a scale factor.
	float scale = 1.0;

	// Preprocess input image by subtracting mean values. Mean values should be in BGR order and delimited by spaces.
	cv::Scalar mean = cv::Scalar(1.0); // parser.get<cv::Scalar>("mean");

	// Indicate that model works with RGB input images instead BGR ones.
	bool useRGB = false;
	
	// Preprocess input image by resizing to a specific width.
	int inpWidth = 320;
	
	// Preprocess input image by resizing to a specific height.
	int inpHeight = 320;

	size_t asyncNumReq = 0;

	// Path to a binary file of model contains trained weights.
	// It could be a file with extensions .caffemodel (Caffe),
	// .pb (TensorFlow), .t7 or .net (Torch), .weights (Darknet), .bin (OpenVINO).
	std::string modelPath = "../../models/yolo/yolov3-320.cfg";

	// Path to a text file of model contains network configuration. "
	// It could be a file with extensions .prototxt (Caffe), .pbtxt (TensorFlow), .cfg (Darknet), .xml (OpenVINO).",
	std::string configPath = "../../models/yolo/yolov3-320.weights";
	std::string input = "../../dataset/highway-traffic.mp4";

	// Load a model.
	Net net = readNet(modelPath, configPath, parser.get<String>("framework"));
	int backend = parser.get<int>("backend");
	net.setPreferableBackend(backend);
	net.setPreferableTarget(parser.get<int>("target"));
	std::vector<String> outNames = net.getUnconnectedOutLayersNames();

	// Create a window
	cv::namedWindow("YOLO", WINDOW_NORMAL);
	// int initialConf = (int)(confThreshold * 100);
	// cv::createTrackbar("Threshold, %", "YOLO", &initialConf, 99, callback);

	// Open a video file or an image file or a camera stream.
	VideoCapture cap;
	cap.open("../../dataset/highway-traffic.mp4");

	if (asyncNumReq) {
		CV_Error(Error::StsNotImplemented, "Asynchronous forward is supported only with Inference Engine backend.");
	}

	// Process frames.
	cv::Mat frame, blob;
	while (waitKey(1) < 0)
	{
		cap >> frame;
		if (frame.empty())
		{
			waitKey();
			break;
		}

		preprocess(frame, net, Size(inpWidth, inpHeight), scale, mean, useRGB);

		std::vector<cv::Mat> outs;
		net.forward(outs, outNames);

		postprocess(frame, outs, net, backend);

		// Put efficiency information.
		std::vector<double> layersTimes;
		double freq = getTickFrequency() / 1000;
		double t = net.getPerfProfile(layersTimes) / freq;
		std::string label = format("Inference time: %.2f ms", t);
		putText(frame, label, cv::Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));

		cv::imshow("YOLO", frame);
	}

	return 0;
}

void preprocess(const cv::Mat& frame, dnn::Net& net, Size inpSize, float scale, const cv::Scalar& mean, bool swapRB)
{
	static cv::Mat blob;

	// Create a 4D blob from a frame.
	if (inpSize.width <= 0) inpSize.width = frame.cols;
	if (inpSize.height <= 0) inpSize.height = frame.rows;
	blobFromImage(frame, blob, 1.0, inpSize, cv::Scalar(), swapRB, false, CV_8U);

	// Run a model.
	net.setInput(blob, "", scale, mean);
	if (net.getLayer(0)->outputNameToIndex("im_info") != -1)  // Faster-RCNN or R-FCN
	{
		resize(frame, frame, inpSize);
		cv::Mat imInfo = (cv::Mat_<float>(1, 3) << inpSize.height, inpSize.width, 1.6f);
		net.setInput(imInfo, "im_info");
	}
}

void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, Net& net, int backend)
{
	static std::vector<int> outLayers = net.getUnconnectedOutLayers();
	static std::string outLayerType = net.getLayer(outLayers[0])->type;

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;

	if (outLayerType == "DetectionOutput")
	{
		// Network produces output blob with a shape 1x1xNx7 where N is a number of
		// detections and an every detection is a vector of values
		// [batchId, classId, confidence, left, top, right, bottom]
		CV_Assert(outs.size() > 0);

		for (size_t k = 0; k < outs.size(); k++)
		{
			float* data = (float*)outs[k].data;
			for (size_t i = 0; i < outs[k].total(); i += 7)
			{
				float confidence = data[i + 2];

				if (confidence > confThreshold)
				{
					int left   = (int)data[i + 3];
					int top    = (int)data[i + 4];
					int right  = (int)data[i + 5];
					int bottom = (int)data[i + 6];
					int width  = right - left + 1;
					int height = bottom - top + 1;
					if (width <= 2 || height <= 2)
					{
						left   = (int)(data[i + 3] * frame.cols);
						top    = (int)(data[i + 4] * frame.rows);
						right  = (int)(data[i + 5] * frame.cols);
						bottom = (int)(data[i + 6] * frame.rows);
						width  = right - left + 1;
						height = bottom - top + 1;
					}
					
					classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
					boxes.push_back(cv::Rect(left, top, width, height));
					confidences.push_back(confidence);
				}
			}
		}
	}
	else if (outLayerType == "Region")
	{
		for (size_t i = 0; i < outs.size(); ++i)
		{
			// Network produces output blob with a shape NxC where N is a number of detected objects and C is a number of classes + 4 where the first 4 numbers are [center_x, center_y, width, height]
			float* data = (float*)outs[i].data;
			for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
			{
				cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
				cv::Point classIdPoint;
				double confidence;
				minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
				if (confidence > confThreshold)
				{
					int centerX = (int)(data[0] * frame.cols);
					int centerY = (int)(data[1] * frame.rows);
					int width = (int)(data[2] * frame.cols);
					int height = (int)(data[3] * frame.rows);
					int left = centerX - width / 2;
					int top = centerY - height / 2;

					classIds.push_back(classIdPoint.x);
					confidences.push_back((float)confidence);
					boxes.push_back(cv::Rect(left, top, width, height));
				}
			}
		}
	}
	else
	{
		CV_Error(Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);
	}

	// NMS is used inside Region layer only on DNN_BACKEND_OPENCV for another backends we need NMS in sample or NMS is required if number of outputs > 1
	if (outLayers.size() > 1 || (outLayerType == "Region" && backend != DNN_BACKEND_OPENCV))
	{
		std::map<int, std::vector<size_t> > class2indices;
		for (size_t i = 0; i < classIds.size(); i++)
		{
			if (confidences[i] >= confThreshold)
			{
				class2indices[classIds[i]].push_back(i);
			}
		}
		std::vector<cv::Rect> nmsBoxes;
		std::vector<float> nmsConfidences;
		std::vector<int> nmsClassIds;
		for (std::map<int, std::vector<size_t> >::iterator it = class2indices.begin(); it != class2indices.end(); ++it)
		{
			std::vector<cv::Rect> localBoxes;
			std::vector<float> localConfidences;
			std::vector<size_t> classIndices = it->second;
			for (size_t i = 0; i < classIndices.size(); i++)
			{
				localBoxes.push_back(boxes[classIndices[i]]);
				localConfidences.push_back(confidences[classIndices[i]]);
			}
			std::vector<int> nmsIndices;
			NMSBoxes(localBoxes, localConfidences, confThreshold, nmsThreshold, nmsIndices);
			for (size_t i = 0; i < nmsIndices.size(); i++)
			{
				size_t idx = nmsIndices[i];
				nmsBoxes.push_back(localBoxes[idx]);
				nmsConfidences.push_back(localConfidences[idx]);
				nmsClassIds.push_back(it->first);
			}
		}
		boxes = nmsBoxes;
		classIds = nmsClassIds;
		confidences = nmsConfidences;
	}

	for (size_t idx = 0; idx < boxes.size(); ++idx)
	{
		cv::Rect box = boxes[idx];
		drawPred(classIds[idx], confidences[idx], box.x, box.y,
				 box.x + box.width, box.y + box.height, frame);
	}
}

void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame)
{
	cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0));

	std::string label = format("%.2f", conf);
	if (!classes.empty())
	{
		CV_Assert(classId < (int)classes.size());
		label = classes[classId] + ": " + label;
	}

	int baseLine;
	Size labelSize = cv::getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

	top = max(top, labelSize.height);
	cv::rectangle(frame, cv::Point(left, top - labelSize.height), cv::Point(left + labelSize.width, top + baseLine), cv::Scalar::all(255), FILLED);
	putText(frame, label, cv::Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar());
}

void callback(int pos, void*)
{
	confThreshold = pos * 0.01f;
}
