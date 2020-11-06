#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <math.h>  
#include<vector>
#include<cmath>
#include <cstdlib>

#include "image.h"
#include "morpho_filter.h"
#include "utils.h"


#define PI 3.14159265





Image::Image(std::string fullPath){
    // Read the image
    cv::Mat image = cv::imread(fullPath, 0); // Grayscale img

    // Checks for failure
    if (image.empty())
    {
        std::cout << "Could not open or find the image at " << fullPath << std::endl;
        std::cin.get(); // wait for any key press
    }
    else 
    {
        // Load size
        cv::Size sz = image.size();
        imageWidth = sz.width;
        imageHeight = sz.height;

        imageData = new double*[imageHeight];

        // Iterate and convert to double between 0 and 1
        for (int i = 0; i < imageHeight; i++){
            imageData[i] = new double[imageWidth];

            for (int j = 0; j < imageWidth; j++){
                imageData[i][j] = ((double) image.data[i * imageWidth + j])/255;
            }
        }

    }
    this->pathToImage = fullPath;

}

Image::Image(double** data, int width, int height){
    if (data == NULL){
        imageWidth = 0;
        imageHeight = 0;
        imageData = NULL;
    }
    else{
        // we assume that 0 <= data[i][j] <= 1
        imageWidth = width;
        imageHeight = height;

        imageData = data;
    }
    this->pathToImage = "";
}


Image::Image(bool** data, int width, int height){
    if (data == NULL){
        imageHeight = 0;
        imageWidth = 0;
        imageData = NULL;
    }
    else{
        imageHeight = height;
        imageWidth = width;

        imageData = new double*[imageHeight];

        // Iterate and convert to double between 0 and 1
        for (int i = 0; i < imageHeight; i++){
            imageData[i] = new double[imageWidth];

            for (int j = 0; j < imageWidth; j++){
                imageData[i][j] = ((double) data[i][j]);
            }
            delete[] data[i];
        }
        delete[]  data;
    }
    this->pathToImage = "";
}

Image::Image(const Image& other){
        imageHeight = other.imageHeight;
        imageWidth = other.imageWidth;

        imageData = new double*[imageHeight];

        // Deep copy of the image data
        for (int i = 0; i < imageHeight; i++){
            imageData[i] = new double[imageWidth];

            for (int j = 0; j < imageWidth; j++){
                imageData[i][j] = other.imageData[i][j];
            }
        }
        this->pathToImage = other.getPath();
}


Image& Image::operator=(const Image& other){
    free_2Darray<double>(imageData, imageHeight);
    
    imageHeight = other.imageHeight;
    imageWidth = other.imageWidth;

    imageData = new double*[imageHeight];

    // Deep copy of the image data
    for (int i = 0; i < imageHeight; i++){
        imageData[i] = new double[imageWidth];

        for (int j = 0; j < imageWidth; j++){
            imageData[i][j] = other.imageData[i][j];
        }
    }
    return *this;
}

Image::~Image()
{
    if (imageData != NULL) free_2Darray<double>(imageData, imageHeight);
}


void Image::displayData(std::ostream& o) const
{
    o << "[";
    for (int i = 0; i < imageHeight; i++){
        o << "[";
        for (int j = 0; j < imageWidth-1; j++){
            o << imageData[i][j] << ", ";
        }
        o << imageData[i][imageWidth-1] << "]," << std::endl;
    o << "]" << std::endl;
    }

}



void Image::display(cv::String windowName) const
{
    if (imageData == NULL){
        return;
    }
    cv::Mat displayImage = this->toMat();

    // Display the image
    cv::namedWindow(windowName);
    cv::imshow(windowName, displayImage);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(windowName);

    // Free the temporary array
    delete[] displayImage.data;
}



int Image::getWidth() const
{
    return imageWidth;
}



int Image::getHeight() const
{
    return imageHeight;
}



bool Image::empty() const
{ 
    return imageData == NULL;
}


std::string Image::getPath() const
{
    return this->pathToImage;
}


void Image::revert(){
    if(this->pathToImage == ""){
        std::cout << "Cannot revert the image" << std::endl;
    }else{
        *(this) = Image(this->pathToImage);
    }
}

void Image::rectangleFill(int xA, int yA, int xB, int yB, double intensity){
    if (xA > xB){
        std::swap(xA, xB);
    }
    if (yA > yB){
        std::swap(yA, yB);
    }

    xA = std::max(0, xA);
    yA = std::max(0, yA);
    xB = std::min(imageWidth-1, xB);
    yB = std::min(imageHeight-1, yB);

    intensity = (intensity < 0) ? 0: ((intensity > 1) ? 1: intensity);

    for (int i = yA; i <= yB; i++){
        for (int j = xA; j <= xB; j++){
            if (imageData[i][j] >= 0.){
                imageData[i][j] = intensity;
            }
        }
    }

}


cv::Mat Image::toMat() const{
    /* Creates the Mat object associated to the image 
    After being used, the returned Mat data attribute
    needs to be freed */
    // Reconstruct the uchar data
    uchar* imageDataChar = new uchar[imageHeight*imageWidth];

    for (int i = 0; i < imageHeight; i++){
        for (int j = 0; j < imageWidth; j++){
            if (imageData[i][j] < 0) imageDataChar[i*imageWidth + j] = 255;     //default value = white 
            else imageDataChar[i*imageWidth + j] = (uchar) (imageData[i][j] * 255);
        }
    }

    // Create the appropriate image using opencv
    cv::Mat reconstructedImage(imageHeight, imageWidth, CV_8UC1, imageDataChar);
    return reconstructedImage;
}

void Image::save(std::string fullPath) const
{
    if (imageData == NULL){
        return;
    }
    cv::Mat savedImage = this->toMat();

    cv::imwrite(fullPath, savedImage);
    delete[] savedImage.data;

}


Image Image::symmetry(SYM_TYPE axis) const
{
    double **destData = NULL;   // destination data matrix
    int destWidth, destHeight;  // destination image width and height

    // Determine output size
    if (axis == SYM_NONE || axis == SYM_X || axis == SYM_Y){
        destWidth = imageWidth;
        destHeight = imageHeight;
    }

    else if (axis == SYM_DIAG1 || axis == SYM_DIAG2){
        destWidth = imageHeight;
        destHeight = imageWidth;
    }

    else {
        return Image(NULL);
    }

    // Creation of an array of the desired format
    destData = new double*[destHeight];

    for (int i = 0; i < destHeight; i++){
        destData[i] = new double[destWidth];
    }


    // Fill array for all kind of symmetry
    if (axis == SYM_NONE){
        for (int i = 0; i < destHeight; i++){
            for (int j = 0; j < destWidth; j++){
                destData[i][j] = imageData[i][j];
            }
        }
    }

    else if (axis == SYM_X){
        for (int i = 0; i < destHeight; i++){
            for (int j = 0; j < destWidth; j++){
                destData[destHeight - i - 1][j] = imageData[i][j];
            }
        }
    }

    else if (axis == SYM_Y){
        for (int i = 0; i < destHeight; i++){
            for (int j = 0; j < destWidth; j++){
                destData[i][destWidth - j - 1] = imageData[i][j];
            }
        }
    }

    else if (axis == SYM_DIAG1){
        for (int i = 0; i < destWidth; i++){
            for (int j = 0; j < destHeight; j++){
                destData[j][i] = imageData[i][j];
            }
        }
    }
    else if (axis == SYM_DIAG2){
        for (int i = 0; i < destWidth; i++){
            for (int j = 0; j < destHeight; j++){
                destData[destHeight - j - 1][destWidth - i - 1] = imageData[i][j];
            }
        }
    }


    return Image(destData, destWidth, destHeight);
}


double Image::operator()(int height,int width) const{       // TO BE TESTED
    if(     height > imageHeight 
        ||  height < 0
        ||  width > imageWidth
        ||  width < 0){

        perror("in operator (), doesnt fit the dimension");
        std::cout << "dimension Height x Width : " << imageHeight << " " << imageWidth << std::endl;
        std::cout << "i , j : " << height << ", " << width << std::endl;
        exit(1);
    }
    return imageData[height][width];
}


Image Image::operator+(const Image& other) const
{   
    double** data = new double*[imageHeight];

    for (int i = 0; i < imageHeight; i++){
        data[i] = new double[imageWidth];
        for (int j = 0; j < imageWidth; j++){
            if ((*this)(i, j) >= 0 || other(i, j) >= 0){
                data[i][j] = std::min(1., (*this)(i, j) + other(i, j)); // Rounds value too high to 1 
            }
            else{
                data[i][j] = -1;
            }
        }
    }
    return Image(data, imageWidth, imageHeight);

} 

Image Image::operator-(const Image& other) const
{
    double** data = new double*[imageHeight];

    for (int i = 0; i < imageHeight; i++){
        data[i] = new double[imageWidth];
        for (int j = 0; j < imageWidth; j++){
            if ((*this)(i, j) >= 0 || other(i, j) >= 0){
                data[i][j] = std::max(0., (*this)(i, j) - other(i, j)); // Rounds value too low to 0 
            }
            else{
                data[i][j] = -1;
            }
        }
    }
    return Image(data, imageWidth, imageHeight);

}

void Image::imageFilling(int xCenter, int yCenter, int xRadius, int yRadius, COEFFICIENT_TYPE coeffType, float angle, float k, bool randomized){
    angle = angle * PI / 180.0;
    double dist;

    if (!randomized){
        for(int i = 0; i < imageHeight; i++){
            for (int j = 0; j < imageWidth; j++){
                dist = sqrt( pow((double)(i - yCenter)* cos(angle) + (double)(j - xCenter)*sin(angle),2)/(xRadius * xRadius) 
                + pow((double)(i - yCenter)* sin(angle) - (double)(j - xCenter)*cos(angle),2)/(yRadius * yRadius) );

                imageData[i][j] = Coefficients(coeffType, dist, k)*(imageData[i][j] - 1.0)+1.0;
            }
        }
    }
    else {
        double p = 1.;
        double random;
        std::srand(std::time(nullptr));
        bool increase = true;
        for(int i = 0; i < imageHeight; i++){
            for (int j = 0; j < imageWidth; j++){
                random = ((double)std::rand())/RAND_MAX;
                if (random < 0.12){
                    increase = !increase;
                }
                if (increase){
                    if (p < 1.15){
                        p += 0.0008;
                    }
                }
                else{
                    if (p > 0.85){
                        p -= 0.0008;
                    }
                }
                dist =p * sqrt( pow((double)(i - xCenter)* cos(angle) + (double)(j - yCenter)*sin(angle),2)/(xRadius * xRadius) 
                + pow((double)(i - xCenter)* sin(angle) - (double)(j - yCenter)*cos(angle),2)/(yRadius * yRadius) );

                imageData[i][j] = Coefficients(coeffType, dist, k)*(imageData[i][j] - 1.0)+1.0;
            }
        }

    }
}


void Image::swirlImage(float swirlCenterX, float swirlCenterY, float swirlRadius, float swirlTwists){
    swirlTwists = swirlTwists * PI / 180.0;
    Image tempImage = *(this);   
    
    for(int i = 0; i < imageHeight; i++) {
        for(int j = 0; j < imageWidth; j++) {
            
            float pixelY = (float)i - swirlCenterY;
            float pixelX = (float)j - swirlCenterX;
            
            float pixelDistance = pow(pow(pixelX, 2) + pow(pixelY, 2), 0.5);
            float pixelAngle = atan2(pixelX, pixelY);
            float  swirlAmount = 1.0 - (pixelDistance/swirlRadius);
            
            if(swirlAmount > 0) {
                float twistAngle =  swirlTwists * swirlAmount * PI;
                pixelAngle += twistAngle ;
                pixelX = sin(pixelAngle) * pixelDistance ;
                pixelY = cos(pixelAngle) * pixelDistance ;  
            }
            imageData[i][j] = tempImage((int)(swirlCenterY + pixelY),(int)(swirlCenterX + pixelX));   
        }
    }
}



void Image::rainEffect(FUNCTION_MODE mode, int topLeftPixelX, int topLeftPixelY, int squareLength){

    Image tempImage = *(this);      
    

    for(int i = topLeftPixelX; i < topLeftPixelX + squareLength; i++ ){
        for(int j = topLeftPixelY; j < topLeftPixelY + squareLength; j++){
//             if( i >= xCenter - xLength && i < xCenter + xLength && j >= yCenter - yLength && j < yCenter + yLength){
                
                if(mode == HORIZONTAL) {
                    imageData[i][j] = tempImage(i + (int)( 2* cos(j/3)) , j);
                }
                
                if(mode == VERTICAL){
                    imageData[i][j] = tempImage(i, j + (int)(2 * cos(j/3)));
                }

                if(mode == DIAGONAL){   
                    imageData[i][j] = tempImage(i + (int)( 2* cos(j/5)), j + (int)(2 * cos(j/5)));
                }      
//             }
        }
    }
}
    
    
    
/*
void Image::rainEffect(FUNCTION_MODE mode){

    Image tempImage = *(this);      
    
    int xCenter = imageHeight / 2;
    int yCenter = imageWidth / 2;

    int xLength = 50;
    int yLength = 50;

    for(int i = 0; i < imageHeight; i++ ){
        for(int j = 0; j < imageWidth; j++){
            if( i >= xCenter - xLength && i < xCenter + xLength && j >= yCenter - yLength && j < yCenter + yLength){
                
                if(mode == HORIZONTAL) {
                    imageData[i][j] = tempImage(i + (int)( 2* cos(j/3)) , j);
                }
                
                if(mode == VERTICAL){
                    imageData[i][j] = tempImage(i, j + (int)(2 * cos(j/3)));
                }

                if(mode == DIAGONAL){   
                    imageData[i][j] = tempImage(i + (int)( 2* cos(j/5)), j + (int)(2 * cos(j/5)));
                }      
            }
        }
    }
}
*/
    
    
    
    




    
    
