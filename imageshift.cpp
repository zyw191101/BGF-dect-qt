#include "ImageShift.h"

ImageShift::ImageShift(int _size, int limit)
{
    if (_size % 2)
    {
        this->Move_size = _size + 11;
    }
    else
    {
        this->Move_size = _size + 10;
    }
    this->shift_limit = limit;
}

ImageShift::~ImageShift()
{
}

bool ImageShift::update(cv::Mat &Newframe)
{
    if (has_init == false)
    {
        this->ISize = Newframe.size();

        cv::Mat NewWF;
        cv::Mat NewHF;
        // Step1 get gray
        get_G_feature(Newframe, NewWF, NewHF);

        cv::Mat NewHF_ROI(NewHF, cv::Rect(0, this->Move_size, 1, NewHF.rows - 2 * this->Move_size));
        NewHF_ROI.copyTo(this->H_feature);
        cv::Mat NewWF_ROI(NewWF, cv::Rect(this->Move_size, 0, NewWF.cols - 2 * this->Move_size, 1));
        NewWF_ROI.copyTo(this->W_feature);

        this->W_feature.convertTo(this->W_feature, CV_32FC1);
        this->H_feature.convertTo(this->H_feature, CV_32FC1);
        has_init = true;
        return false;
    }
    else
    {
        this->ISize = Newframe.size();
        cv::Mat NewWF;
        cv::Mat NewHF;
        // Step1 get gray
        get_G_feature(Newframe, NewWF, NewHF);
        // NewWF.convertTo(NewWF, CV_32FC1);
        // NewHF.convertTo(NewHF, CV_32FC1);
        // step2 get shift
        cv::Point move;
        move = match_G_feature(NewWF, NewHF);
        // step3 update feature

        cv::Mat NewHF_ROI(NewHF, cv::Rect(0, this->Move_size, 1, NewHF.rows - 2 * this->Move_size));
        NewHF_ROI.copyTo(this->H_feature);

        cv::Mat NewWF_ROI(NewWF, cv::Rect(this->Move_size, 0, NewWF.cols - 2 * this->Move_size, 1));
        NewWF_ROI.copyTo(this->W_feature);
        if (shift_limit == 0)
        {
            this->shift_limit = Move_size - 8;
        }

        if (abs(move.x) < this->shift_limit && abs(move.y) < this->shift_limit)
        {
            this->frame_movex = move.x;
            this->frame_movey = move.y;
            deshift_movex += this->frame_movex;
            deshift_movey += this->frame_movey;

            // 创建仿射变换矩阵 [1, 0, shift_x; 0, 1, shift_y]
            cv::Mat translation_matrix = (cv::Mat_<double>(2, 3) << 1, 0, move.x, 0, 1, move.y);

            // 应用仿射变换，移动图像
            cv::warpAffine(Newframe, Newframe, translation_matrix, Newframe.size());
            return true;
        }
        else
        {
            this->frame_movex = 0;
            this->frame_movey = 0;
            // std::cout << "get shift flase " << std::endl;
            return false;
        }
    }
}

int ImageShift::get_G_feature(cv::Mat &src, cv::Mat &Width_feature, cv::Mat &Height_feature)
{
    // get cols feature
    cv::reduce(src, Width_feature, 0, cv::REDUCE_AVG, CV_32FC1);  // to one raw
    cv::reduce(src, Height_feature, 1, cv::REDUCE_AVG, CV_32FC1); // to one col
    // std::cout << Width_feature << std::endl;
    // std::cout << Height_feature << std::endl;
    return 0;
}
cv::Point ImageShift::match_G_feature(cv::Mat &NW_F, cv::Mat &NH_F)
{
    // match row feature
    if (NW_F.type() != CV_32FC1 || NH_F.type() != CV_32FC1)
    {
        return cv::Point(999, 999);
    }
    cv::Mat result_w;
    cv::matchTemplate(NW_F, this->W_feature, result_w, cv::TM_SQDIFF);
    cv::Mat result_h;
    cv::matchTemplate(NH_F, this->H_feature, result_h, cv::TM_SQDIFF);

    // std::cout << result_w << std::endl;
    // cv::Mat result_h_t = result_h.t();
    // std::cout << result_h_t << std::endl;

    cv::Point pt_min_w, pt_min_h;
    cv::minMaxLoc(result_w, NULL, NULL, &pt_min_w, NULL);
    cv::minMaxLoc(result_h, NULL, NULL, &pt_min_h, NULL);

    cv::Point result = cv::Point(pt_min_w.x - Move_size, pt_min_h.y - Move_size);
    return result;
}
