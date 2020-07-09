#include<fstream>
#include<sstream>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

float nnConfidenceThreshold = 0.7;
float nonMaximumSuppressionThreshold = 0.5;
int yoloWidth = 416;
int yoloHeight = 416;

vector<string> classes;

void removeBox(Mat &frame, const vector<Mat> &out);

bool drawBoxIfPerson(int classId, float conf, int left, int top, int right, int bottom, Mat &frame);

vector<String> getOutputsNames(const Net &net);

int main(int argc, char *argv[]) {
    ifstream ifs("coco.names");
    string line;
    while (getline(ifs, line)) classes.push_back(line);

    Net net = readNetFromDarknet("yolov3.cfg", "yolov3.weights");
    Mat frame, blob;

    VideoCapture cap;
    if (argc > 1) {
        cap = VideoCapture(argv[1]);
    } else {
        if (!cap.open(0))
            return 1;
    }

    while (cap.isOpened()) {
        cap >> frame;
        cv::resize(frame, frame, cv::Size(1280, frame.rows * 1280 / frame.cols));

        // convert image to blob
        blobFromImage(frame,
                      blob,
                      1.0 / 255,
                      cv::Size(yoloWidth, yoloHeight),
                      Scalar(0, 0, 0),
                      true,
                      false
        );
        net.setInput(blob);

        vector<Mat> outs;
        net.forward(outs, getOutputsNames(net));

        removeBox(frame, outs);

        imshow("Goblin detector", frame);
        if (cv::waitKey(10) > 0) {
            break;
        }
    }

    return 0;
}

void removeBox(Mat &frame, const vector<Mat> &out) {
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (const auto &i : out) {
        auto *data = (float *) i.data;
        for (int j = 0; j < i.rows; ++j, data += i.cols) {
            Mat scores = i.row(j).colRange(5, i.cols);
            Point classIdPoint;
            double confidence;
            minMaxLoc(scores, nullptr, &confidence, nullptr, &classIdPoint);
            if (confidence > nnConfidenceThreshold) {
                int boxCenterX = (int) (data[0] * frame.cols);
                int boxCenterY = (int) (data[1] * frame.rows);
                int boxWidth = (int) (data[2] * frame.cols);
                int boxHeight = (int) (data[3] * frame.rows);
                int boxLeft = boxCenterX - boxWidth / 2;
                int boxTop = boxCenterY - boxHeight / 2;

                classIds.emplace_back(classIdPoint.x);
                confidences.emplace_back((float) confidence);
                boxes.emplace_back(Rect(boxLeft, boxTop, boxWidth, boxHeight));
            }
        }
    }

    int peopleCount = 0;
    vector<int> indices;
    NMSBoxes(boxes, confidences, nnConfidenceThreshold, nonMaximumSuppressionThreshold, indices);
    for (int idx : indices) {
        Rect box = boxes[idx];
        peopleCount += drawBoxIfPerson(classIds[idx],
                                       confidences[idx],
                                       box.x,
                                       box.y,
                                       box.x + box.width,
                                       box.y + box.height,
                                       frame
        );
    }

    putText(frame,
            std::string("people count = ") + std::to_string(peopleCount),
            Point(8, 16),
            FONT_HERSHEY_SIMPLEX,
            0.5,
            Scalar(0, 0, 255)
    );
}

bool drawBoxIfPerson(int classId, float conf, int left, int top, int right, int bottom, Mat &frame) {
    string label;
    if (!classes.empty()) {
        if (classes[classId] != "person") {
            return false;
        }
        label = classes[classId] + ":" + format("%.2f", conf);
    }

    auto boxColor = Scalar(255, 255, 255);
    auto labelColor = Scalar(0, 0, 0);
    rectangle(frame, Point(left, top), Point(right, bottom), boxColor, 2);

    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame,
              Point(left, top - round(1.5 * labelSize.height)),
              Point(left + round(1.5 * labelSize.width), top + baseLine),
              boxColor,
              FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, labelColor, 1);

    return true;
}

vector<String> getOutputsNames(const Net &net) {
    static vector<String> names;
    if (names.empty()) {
        vector<int> outLayers = net.getUnconnectedOutLayers();
        vector<String> layersNames = net.getLayerNames();

        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}