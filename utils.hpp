#pragma once

#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <assert.h>
#include <stddef.h>
#include <iostream>


namespace cvutils
{
using Img = cv::Mat;

auto constexpr WINDOW_NAME = "Display Image";
void show_image(const Img& img)
{
    cv::imshow(WINDOW_NAME, img);
    cv::waitKey(3000);
}

Img read_grayscale(std::filesystem::path img)
{
    auto image = cv::imread(img, cv::IMREAD_GRAYSCALE);
    if (image.empty())
    {
        std::cout << "Could not read the image: " << img << std::endl;
        exit(EXIT_FAILURE);
    }
    assert(image.channels() == 1);
    return image;
}

Img pad(const Img& src, int ksize)
{
    auto border = (ksize - 1) / 2;
    Img padded;
    cv::copyMakeBorder(src, padded, border, border, border, border, cv::BORDER_CONSTANT, 0);
    assert((src.rows + (border * 2)) == padded.rows);
    assert((src.cols + (border * 2)) == padded.cols);
    return padded;
}

Img op_on_windows(const Img& src, int ksize, std::function<uint8_t(std::vector<uint8_t>&)> operation_on_window)
{
    auto dst = src.clone();
    auto padded = pad(src, ksize);
    auto padding = (ksize - 1) / 2;
    auto klength = ksize * ksize;
    auto collector = std::vector<uint8_t>(klength);
    for (auto r = padding; r < padded.rows - padding; r++)
    {
        for (auto c = padding; c < padded.cols - padding; c++)
        {
            for (auto i = 0; i < klength; i++)
                collector[i] = padded.at<uint8_t>(r + floor(i / ksize), c + (i % ksize));
            dst.at<uint8_t>(r - padding, c - padding) = operation_on_window(collector);
        }
    }
    return dst;
}

/*
Median for even number of elements cannot use `nth_element`, it needs a mean of the two middle elements.
We take advantage of kernels always being odd sized=odd number of elements.
*/
Img median_blur(const Img& src, int ksize)
{
    auto klength = ksize * ksize;
    auto n = klength / 2;
    return op_on_windows(src, ksize, [n](auto v) {
        std::nth_element(v.begin(), v.begin() + n, v.end());
        return v[n];
        });
}
}