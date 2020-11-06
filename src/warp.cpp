#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h> // cos, sin

#include "image.h" 
#include "warp.h"


double c0(double t){
    return 0.5 * (std::pow(t, 3) - std::pow(t, 2)); 
}

double c1(double t){
    return -1.5*std::pow(t, 3) + 2*std::pow(t, 2) + 0.5*t;
}

double c2(double t){
    return 1.5*std::pow(t, 3) - 2.5*std::pow(t, 2) + 1;
}

double c3(double t){
    return -0.5*std::pow(t, 3) + std::pow(t, 2) - 0.5*t;
}

double* rotation(int k, int l, int i_c, int j_c, const double theta, double* destCoord){
    double x, y;
    x = j_c + (k - j_c) * std::cos(theta) + (l - i_c) * std::sin(theta);
    y = i_c - (k - j_c) * std::sin(theta) + (l - i_c) * std::cos(theta);
    destCoord[0] = x;
    destCoord[1] = y;
    return destCoord;
}

double* translation(int k, int l, const double delta_X, const double delta_Y, double* destCoord){
    destCoord[0] = k - delta_X;
    destCoord[1] = l - delta_Y;
    return destCoord;
}




// ## New methods Image
double Image::pix_interpol0(double x, double y, double defaultIntensity) const{
    int i, j;
    double d_i, d_j;

    
    j = (int) x;
    i = (int) y;

    d_j = x - j;
    d_i = y - i;

    if(j > imageWidth - 2 || i < 0 || i > imageHeight - 2 || j < 0) return defaultIntensity; // case outcome pixel doesnt come from in input image
    if((*this)(i, j) < 0 || (*this)(i+1, j) < 0 || (*this)(i, j+1) < 0 || (*this)(i+1, j+1) < 0 ) return defaultIntensity;    // case interpolaion with made_up pixel
    
    /* interpolation order 0*/
    if (0 <= d_i && d_i < 0.5){
        if (0 <= d_j && d_j < 0.5){
            return (*this)(i, j);
        }
        else if (0.5 <= d_j && d_j < 1){
            return (*this)(i, j+1);
        }
        else{
            return defaultIntensity;
        }
    }
    else if (0.5 <= d_i && d_i < 1){
        if (0 <= d_j && d_j < 0.5){
            return (*this)(i+1, j);
        }
        else if (0.5 <= d_j && d_j < 1){
            return (*this)(i+1, j+1);
        } else{
            return defaultIntensity;
        }
    }
    return defaultIntensity;
}

double Image::pix_interpol1(double x, double y, double defaultIntensity) const{
    int i, j;
    double d_i, d_j;
    
    j = (int) x;
    i = (int) y;

    d_j = x - j;
    d_i = y - i;

    if(j > imageWidth - 2 || i < 0 || i > imageHeight - 2 || j < 0) return defaultIntensity; // case outcome pixel doesnt come from in input image
    if((*this)(i, j) < 0 || (*this)(i+1, j) < 0 || (*this)(i, j+1) < 0 || (*this)(i+1, j+1) < 0 ) return defaultIntensity;    // case interpolaion with made_up pixel
    
    /* standard bilinear interpolation 1st order */
    double res;
    res =   (1-d_i) * (1-d_j) * (*this)(i, j)
          + d_i * (1-d_j) * (*this)(i+1, j)
          + (1-d_i) * d_j * (*this)(i, j+1)
          + d_i * d_j * (*this)(i+1, j+1);
    return res;
}

double Image::pix_interpol3(double x, double y, double defaultIntensity) const{
    int i, j;
    double d_i, d_j;
    
    j = (int) x;
    i = (int) y;

    d_j = x - j;
    d_i = y - i;

    if(j > imageWidth - 3 || i < 1 || i > imageHeight - 3 || j < 1) return defaultIntensity; // case outcome pixel doesnt come from in input image
    if(    (*this)(i-1, j-1) < 0 || (*this)(i, j-1) < 0 || (*this)(i+1, j-1) < 0 || (*this)(i+2, j-1) < 0 
        || (*this)(i-1, j  ) < 0 || (*this)(i, j  ) < 0 || (*this)(i+1, j  ) < 0 || (*this)(i+2, j  ) < 0
        || (*this)(i-1, j+1) < 0 || (*this)(i, j+1) < 0 || (*this)(i-1, j+1) < 0 || (*this)(i+2, j+1) < 0
        || (*this)(i-1, j+2) < 0 || (*this)(i, j+2) < 0 || (*this)(i+1, j+2) < 0 || (*this)(i+2, j+2) < 0) return defaultIntensity;    // case interpolaion with made_up pixel
    
    /* interpolation 3rd order, cubic spline */
    double R1, R2, R3, R4;
    R1 = (*this)(i-1, j-1)*c3(d_i) + (*this)(i, j-1)*c2(d_i) + (*this)(i+1, j-1)*c1(d_i) + (*this)(i+2, j-1)*c0(d_i);
    R2 = (*this)(i-1, j)*c3(d_i) + (*this)(i, j)*c2(d_i) + (*this)(i+1, j)*c1(d_i) + (*this)(i+2, j)*c0(d_i);
    R3 = (*this)(i-1, j+1)*c3(d_i) + (*this)(i, j+1)*c2(d_i) + (*this)(i+1, j+1)*c1(d_i) + (*this)(i+2, j+1)*c0(d_i);
    R4 = (*this)(i-1, j+2)*c3(d_i) + (*this)(i, j+2)*c2(d_i) + (*this)(i+1, j+2)*c1(d_i) + (*this)(i+2, j+2)*c0(d_i);
    return std::min(1., std::max(0., R1*c3(d_j) + R2*c2(d_j) + R3*c1(d_j) + R4*c0(d_j)));
}

Image Image::rotation_interpol(double theta, int i_c, int j_c, INTERP_METHOD method, double defaultIntensity) const{
    /*
    creating the empty Image object
    */
    double** destData = NULL;
    /* 
    allocating space 
    */
    destData  = new double*[imageHeight];
    for(int i = 0; i < imageHeight; i++) destData[i] = new double[imageWidth];
    Image newImage(destData, imageWidth, imageHeight);

    /*
    filling Destdata 
    */
    double* destCoordXY = new double[2];
    double newIntensity;
    if (method == INTERP_NEAREST){
        for (int l = 0; l < imageHeight; l++){
            for (int k = 0; k < imageWidth; k++){
                destCoordXY = rotation(k, l, i_c, j_c, theta, destCoordXY);
                newIntensity = this->pix_interpol0(destCoordXY[0], destCoordXY[1], defaultIntensity);
                destData[l][k] = newIntensity;
            }
        }
    }
    else if (method == INTERP_BILINEAR){
        for (int l = 0; l < imageHeight; l++){
            for (int k = 0; k < imageWidth; k++){
                destCoordXY = rotation(k, l, i_c, j_c, theta, destCoordXY);
                newIntensity = this->pix_interpol1(destCoordXY[0], destCoordXY[1], defaultIntensity);
                destData[l][k] = newIntensity;
            }
        }
    }
    else if (method == INTERP_BICUBIC){
        for (int l = 0; l < imageHeight; l++){
            for (int k = 0; k < imageWidth; k++){
                destCoordXY = rotation(k, l, i_c, j_c, theta, destCoordXY);
                newIntensity = this->pix_interpol3(destCoordXY[0], destCoordXY[1], defaultIntensity);
                destData[l][k] = newIntensity;
            }
        }
    }

    delete[] destCoordXY;
    return newImage;
}


Image Image::translation_interpol(double deltaX, double deltaY, INTERP_METHOD method, double defaultIntensity) const{
    /*
    creating the empty Image object
    no change of dimension in the image translated
    */
    double** destData = NULL;
    /* 
    allocating space 
    */
    destData  = new double*[imageHeight];
    for(int i = 0; i < imageHeight; i++) destData[i] = new double[imageWidth];
    Image NewImage(destData, imageWidth, imageHeight);
    /*
    filling Destdata 
    */
    double* destCoordXY = new double[2];
    double newIntensity;

    if (method == INTERP_NEAREST){
        for (int l = 0; l < imageHeight; l++){
            for (int k = 0; k < imageWidth; k++){
                destCoordXY = translation(k, l, deltaX, deltaY, destCoordXY);
                newIntensity = this->pix_interpol0(destCoordXY[0], destCoordXY[1], defaultIntensity);
                destData[l][k] = newIntensity;
            }
        }
    }
    if (method == INTERP_BILINEAR){
        for (int l = 0; l < imageHeight; l++){
            for (int k = 0; k < imageWidth; k++){
                destCoordXY = translation(k, l, deltaX, deltaY, destCoordXY);
                newIntensity = this->pix_interpol1(destCoordXY[0], destCoordXY[1], defaultIntensity);
                destData[l][k] = newIntensity;
            }
        }
    }
    else if (method == INTERP_BICUBIC){
        for (int l = 0; l < imageHeight; l++){
            for (int k = 0; k < imageWidth; k++){
                destCoordXY = translation(k, l, deltaX, deltaY, destCoordXY);
                newIntensity = this->pix_interpol3(destCoordXY[0], destCoordXY[1], defaultIntensity);
                destData[l][k] = newIntensity;
            }
        }
    }
    delete[] destCoordXY;
    return NewImage;
}




