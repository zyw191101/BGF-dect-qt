#include "BackgroundlDetect.h"

#define GLOG
#ifdef GLOG
#endif

void BIIR(cv::Mat& m, float alpha, float beta)
{
    for (int i = 0; i < m.rows; i++)
    {
        for (int j = 1; j < m.cols; j++)
        {
            m.at<uchar>(i, j) = (uchar)((((float)(m.at<uchar>(i, j - 1)) * alpha) + ((float)(m.at<uchar>(i, j)) * (1 -
                alpha))));
        }
        for (int j = m.cols - 2; j >= 0; j--)
        {
            m.at<uchar>(i, j) = (uchar)((((float)(m.at<uchar>(i, j + 1)) * alpha) + ((float)(m.at<uchar>(i, j)) * (1 -
                alpha))));
        }
    }

    for (int j = 0; j < m.cols; j++)
    {
        for (int i = 1; i < m.rows; i++)
        {
            m.at<uchar>(i, j) = (uchar)((((float)(m.at<uchar>(i - 1, j)) * beta) + ((float)(m.at<uchar>(i, j)) * (1 -
                beta))));
        }
        for (int i = m.rows - 2; i >= 0; i--)
        {
            m.at<uchar>(i, j) = (uchar)((((float)(m.at<uchar>(i + 1, j)) * beta) + ((float)(m.at<uchar>(i, j)) * (1 -
                beta))));
        }
    }
}


BackgroundlDetect::BackgroundlDetect(): shift(10, 10)
{
    // rkRga = _rga;
    cv::namedWindow("Four-Grid Display", cv::WINDOW_NORMAL);
    cv::resizeWindow("Four-Grid Display", 1280, 1024);
}


std::vector<BoxInfo> BackgroundlDetect::backgroundInference(cv::Mat cropFrame,
                                                            cv::Mat* backgroundPtr,
                                                            cv::Ptr<cv::BackgroundSubtractorMOG2> mog,
                                                            float background_update_rate, float alpha, float beta,
                                                            int cv_threshold, int meanStdDev_data,int minw,int minh,int maxw,int maxh)

{
    //视场宽度、光照条件、输入通道、决定策略

    cv::Mat& background = *backgroundPtr;//MOG2背景减除器

    cv::Mat fgmask;

    fgmask = cv::abs(cropFrame - background) * 3;//适用于背景相对静态且变化不大的场景，计算量小，但对噪声和背景变化敏感//计算当前帧与背景模型之间的绝对差值。结果是一个与输入图像大小相同的矩阵，其中每个像素值表示当前帧与背景模型在该位置的差异
    // mog->apply(cropFrame, fgmask);//适用于背景复杂且变化频繁的场景，计算量大，但鲁棒性强，前景检测准确性高
    background = background * background_update_rate + cropFrame * (1 - background_update_rate);

#ifdef  DEBUG_BackgroundDete
    cv::Mat show_fgmask_1 = fgmask.clone();
    cv::Mat show_background_1 = background.clone();

#endif

    // 增大背景更新权重 0.9：背景模型对新的变化响应更慢，适合背景变化较小或较缓慢的场景。
    // 减小背景更新权重：使得背景模型更快响应环境变化，但可能导致短暂的运动目标被认为是背景。
    // 可以根据实际场景的稳定性调整这两个值，观察模型对背景变化的敏感度。
    // 如果场景中的背景变化很快，可以将 0.9 调小一些（例如 0.8），而 0.1 则增大一些（例如 0.2）。

    // mog->apply(cropFrame, fgmask);
    // BIIR(fgmask, 0.7, 0.9);
    BIIR(fgmask, alpha, beta);
    // alpha 控制水平方向相邻像素的平滑程度。
    // beta 控制垂直方向相邻像素的平滑程度。
    // 增大 alpha 和 beta：使得滤波器更加平滑，有助于消除前景噪声，但过高的值可能导致运动目标被模糊化。
    // 减小 alpha 和 beta：减少平滑效果，保留更多细节，但可能会导致噪声和伪影。对于较小的运动目标，可以适当降低这些值。

#ifdef  DEBUG_BackgroundDete
    cv::Mat show_fgmask_2 = fgmask.clone();//滤波处理以后的
#endif

    cv::Mat mean, std;//平均值和标准差

    cv::meanStdDev(fgmask, mean, std);

    cv::threshold(fgmask, fgmask, cv_threshold, 255, cv::THRESH_BINARY); //25

    // 减小二值化阈值（例如 15-20）：对运动更加敏感，尤其是光照变化较大的场景，但可能会引入噪声。
    // 增大二值化阈值（例如 30-40）：只保留较为明显的运动，忽略微小的变化或噪声。
    // 可以尝试将阈值调低至 15 或调高至 35，根据运动目标和背景差异性调整。

#ifdef  DEBUG_BackgroundDete
    cv::Mat show_fgmask_3 = fgmask.clone();
    // 拼接成四宫格图
    cv::Mat top, bottom, result;
    cv::hconcat(show_fgmask_1, show_background_1, top);
    cv::hconcat(show_fgmask_2, show_fgmask_3, bottom);
    cv::vconcat(top, bottom, result);

//    cv::namedWindow("display", cv::WINDOW_NORMAL);
//    cv::setWindowProperty("display", cv::WND_PROP_FULLSCREEN,
//                          cv::WINDOW_FULLSCREEN);

    // 显示拼接图像
    cv::imshow("Four-Grid Display", result);
    cv::waitKey(1);//cv::waitKey(30)处理用户输入，避免高频刷新导致闪烁
#endif


    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(fgmask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<BoxInfo> bbox_array;
    std::cout << mean.at<double>(0);
    std::cout << "contours.size()" << contours.size() << std::endl;
    // if (mean.at<double>(0) < 5)
    // 增大 mean 的阈值（例如从 40 增加到更高）：只有较大的运动被认为是前景，可以消除小的噪声和轻微运动。
    // 减小 mean 的阈值：让检测器对较小的变化更加敏感，可以检测到小的运动物体，但可能会带来更多的噪声和伪影。

    if (mean.at<double>(0) < meanStdDev_data) //40
    {
        for (int i = 0; i < contours.size(); i++)
        {
            auto r = cv::boundingRect(contours[i]);
            //if (r.width > 5 and r.height > 5)
            if (r.width > minw and r.height > minw)
            {
                float fw = r.width, fh = r.height;
                std::cout << "fw:" << fw << "   fh:" << fh << std::endl;
                if (fw < maxw and fh < maxh)
                {
                    bbox_array.push_back(BoxInfo{
                        (float)r.x, (float)r.y,
                        (float)r.x + fw, (float)r.y + fh,
                        0.1, false
                    });
                }
            }
        }
    }
#ifdef  DEBUG_BackgroundDete
//     // Draw bounding boxes on the cropFrame
//    for (const auto& box : bbox_array)
//    {
//        cv::Point topLeft((int)box.x1, (int)box.y1);
//        cv::Point bottomRight((int)box.x2, (int)box.y2);
//       cv::rectangle(fgmask, topLeft, bottomRight, cv::Scalar(255, 255, 255), 2); // 使用绿色绘制边界框(0, 255, 0)

//    }
//    cv::Mat show_fgmask_4 = fgmask.clone();
//    cv::namedWindow("display1", cv::WINDOW_NORMAL);
//    //cv::setWindowProperty("display1", cv::WND_PROP_FULLSCREEN,
//       //                   cv::WINDOW_FULLSCREEN);

//    // 显示拼接图像
//    // cv::namedWindow("Four-Grid Display", cv::WINDOW_NORMAL);
//    cv::imshow("display1", show_fgmask_4);
//    cv::waitKey(1);
#endif
    return bbox_array;
}
