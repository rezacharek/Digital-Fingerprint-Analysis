#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

#include <opencv2/highgui.hpp>
#include <opencv2/plot.hpp>

#include "image.h" 
#include "warp.h"


double Image::meanSquaredError(const Image& other, int order_sub_pix = 1) const{
    if(imageWidth != other.getWidth() || imageHeight != other.getHeight()){
        perror("the 2 images don't have the same dimensions");
        perror("can't compute the cost in cost_square");
        exit(1);
    }
    double nb_true_pixels = 0;     // pixels not made up, with default value (here -1)
    double res = 0;

    for(int i = 0; i < imageHeight; i += order_sub_pix){
        for(int j = 0; j < imageWidth; j += order_sub_pix){
            if ((*this)(i, j) >= 0 && other(i, j) >= 0){
                nb_true_pixels += 1;
                res += pow((*this)(i, j) - other(i, j), 2);
            } 
        }
    }
    return res / nb_true_pixels;
}


double Image::correlationRate(const Image& other, int order_sub_pix = 0) const{
    if(imageWidth != other.getWidth() || imageHeight != other.getHeight()){
        perror("the 2 images don't have the same dimensions");
        perror("can't compute the cost in cost_square");
        exit(1);
    }
    double nb_true_pixels = 0;
    double mean_this = 0;
    double mean_other = 0;
    double cov = 0;
    double var_this = 0;
    double var_other = 0;

    // computation means
    for(int i = 0; i < imageHeight; i += order_sub_pix){
        for(int j = 0; j < imageWidth; j += order_sub_pix){
            if(other(i, j) >= 0 && (*this)(i, j) >= 0){
                mean_other += other(i, j);
                mean_this += (*this)(i, j);
                nb_true_pixels += 1;
            }
        }
    }
    mean_other /= nb_true_pixels;
    mean_this /= nb_true_pixels;

    // computation covariance and variance
    for(int i = 0; i < imageHeight; i += order_sub_pix){
        for(int j = 0; j < imageWidth; j += order_sub_pix){
            if(other(i, j) >= 0 && (*this)(i, j) >= 0){
                cov += ((*this)(i, j) - mean_this) * (other(i, j) - mean_other);
                var_this += pow((*this)(i, j) - mean_this, 2);
                var_other += pow(other(i, j) - mean_other, 2);
            }
        }
    }
    return cov / pow(var_this * var_other, 0.5);
}
