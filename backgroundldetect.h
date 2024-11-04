#ifndef BACKGROUNDLDETECT_H
#define BACKGROUNDLDETECT_H

#include <fstream>
#include <tuple>
#include "ImageShift.h"

#define DEBUG_BackgroundDete


struct BoxInfo {
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    bool is_low;
    int class_id;
    std::vector<float> tlbr()
    {
        return {x1, y1, x2, y2};
    }
};

class BackgroundlDetect
{
protected:
    ImageShift shift;

public:
    BackgroundlDetect();


    std::vector<BoxInfo> backgroundInference(cv::Mat cropFrame,
                                             cv::Mat* backgroundPtr,
                                             cv::Ptr<cv::BackgroundSubtractorMOG2> mog,
                                             float background_update_rate, float alpha, float beta, int cv_threshold,
                                             int meanStdDev_data,int minw,int minh,int maxw,int maxh);
};

#endif // BACKGROUNDLDETECT_H
