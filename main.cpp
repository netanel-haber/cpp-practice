#include <opencv2/opencv.hpp>
#include "./utils.hpp"

int main()
{
    cvutils::cv_filters("./median.png", "./gaussian.png", "./laplacian.png");
}