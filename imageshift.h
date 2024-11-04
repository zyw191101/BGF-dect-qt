#ifndef IMAGESHIFT_H
#define IMAGESHIFT_H


#include <opencv2/opencv.hpp>

class ImageShift
{
public:
    /*
    @ &Newframe :
        frame input mast be CV8UC1
    @ size  :
        size of match size
    @ limit :
        size of max frame shift
        if match result>limit function update return flase
    */
    ImageShift(int _size = 10, int limit = 10);
    ~ImageShift();
    bool update(cv::Mat &Newframe);
    int frame_movex = 0;
    int frame_movey = 0;
    int deshift_movex = 0;
    int deshift_movey = 0;


private:
    bool has_init = false;
    int shift_limit = 10;
    cv::Size ISize;
    cv::Mat W_feature, H_feature;
    int Move_size;
    // get the gray projections feature
    int get_G_feature(cv::Mat &src, cv::Mat &W_feature, cv::Mat &H_feature);
    // match gray projections feature
    cv::Point match_G_feature(cv::Mat &NW_F, cv::Mat &NH_F);
};


#endif // IMAGESHIFT_H
