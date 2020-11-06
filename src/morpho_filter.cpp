#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "image.h"
#include "morpho_filter.h"
#include "utils.h"


 /*****************
        PIXEL 
 ******************/
Pixel::Pixel(int y, int x, double val){
    row = y;
    col = x;
    value = std::max(0., std::min(1., val));
}


int Pixel::getRow(){
    return row;
}


int Pixel::getCol(){
    return col;
}

double Pixel::getVal(){
    return value;
}


void Pixel::setRowCol(int y, int x){
    row = y;
    col = x;
}

 /*******************************
        Structuring element
 ********************************/
StructuringElement::StructuringElement(bool** selem, int w, int h, int aw, int ah){
    width = w;
    height = h;
    anchorHeight = ah;
    anchorWidth = aw;

    /* We keep only the activated pixels withing the global rectangle of size width height */
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            if (selem[i][j]){
                shape.push_back(Pixel(i - anchorHeight, j - anchorWidth));
            }
        }
    }
}


StructuringElement::StructuringElement(double** selem, int w, int h, int aw, int ah){
    width = w;
    height = h;
    anchorHeight = ah;
    anchorWidth = aw;

    /* We keep only the activated pixels withing the global rectangle of size width height */
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            if (selem[i][j] > 0){
                shape.push_back(Pixel(i - anchorHeight, j - anchorWidth, selem[i][j]));
            }
        }
    }
}


StructuringElement::StructuringElement(SELEM_SHAPE generalShape, int n, bool flat){
    width = 2 * n + 1;
    height = 2 * n + 1;
    anchorWidth = n;
    anchorHeight = n;

    if (flat){
        /* Initialize for different kind of general shapes */
        if (generalShape == SELEM_CROSS){
            /* cross of width and height 2*n+1 */
            shape.push_back(Pixel(0, 0));
            for (int i = 1; i <= n; i++){
                shape.push_back(Pixel(n + i - anchorHeight, n - anchorWidth));
                shape.push_back(Pixel(n - i - anchorHeight, n - anchorWidth));
                shape.push_back(Pixel(n - anchorHeight, n + i - anchorWidth));
                shape.push_back(Pixel(n - anchorHeight, n - i - anchorWidth));
            }
        }

        else if (generalShape == SELEM_SQUARE){
            /* square of width and height 2*n+1 */
            for (int i = 0; i < height; i++){
                for (int j = 0; j < width; j++){
                    shape.push_back(Pixel(i - anchorHeight, j - anchorWidth));
                }
            }
        }

        else if (generalShape == SELEM_CIRCLE){
            /* circle of radius n */
            for (int i = 0; i < height; i++){
                for (int j = 0; j < width; j++){
                    if (pow(i - anchorHeight, 2) + pow(j - anchorWidth, 2) < n*(n+1)){
                        shape.push_back(Pixel(i - anchorHeight, j - anchorWidth));
                        //std::cout << "1 ";
                    }
                    else {
                        //std::cout << "0 ";
                    }
                }
                //std::cout << std::endl;
            }
            //std::cout << std::endl;
        }

        else if (generalShape == SELEM_DIAMOND){
            /* diamond of height and width 2*n + 1*/
            for (int i = 0; i < height; i++){
                for (int j = 0; j < width; j++){
                    if (abs(i - anchorHeight) + abs(j - anchorWidth) <= n){
                        shape.push_back(Pixel(i - anchorHeight, j - anchorWidth));
                    }
                }
            }
        }

    }
    else{

        /* Initialize for different kind of general shapes */
        if (generalShape == SELEM_CROSS){
            /* cross of width and height 2*n+1 */
            shape.push_back(Pixel(0, 0, 1.));
            for (int i = 1; i <= n; i++){
                shape.push_back(Pixel(n + i - anchorHeight, n - anchorWidth, (n-i)/n));
                shape.push_back(Pixel(n - i - anchorHeight, n - anchorWidth, (n-i)/n));
                shape.push_back(Pixel(n - anchorHeight, n + i - anchorWidth, (n-i)/n));
                shape.push_back(Pixel(n - anchorHeight, n - i - anchorWidth, (n-i)/n));
            }
        }

        else if (generalShape == SELEM_SQUARE){
            /* square of width and height 2*n+1 */
            double distInf = -1;
            for (int i = 0; i < height; i++){
                for (int j = 0; j < width; j++){
                    distInf = std::max(std::abs(i - anchorHeight), std::abs(j - anchorWidth));
                    shape.push_back(Pixel(i - anchorHeight, j - anchorWidth, (n - distInf)/n));
                }
            }
        }

        else if (generalShape == SELEM_CIRCLE){
            /* circle of radius n */
            double distEucl = -1;
            for (int i = 0; i < height; i++){
                for (int j = 0; j < width; j++){
                    distEucl = pow(i - anchorHeight, 2) + pow(j - anchorWidth, 2);
                    if (distEucl < n * (n+1)){
                        shape.push_back(Pixel(i - anchorHeight, j - anchorWidth, (n-std::sqrt(distEucl))/n));
                    }
                }
            }
        }

        else if (generalShape == SELEM_DIAMOND){
            /* diamond of height and width 2*n + 1*/
            double distManh = -1;
            for (int i = 0; i < height; i++){
                for (int j = 0; j < width; j++){
                    distManh = abs(i - anchorHeight) + abs(j - anchorWidth);
                    if (distManh <= n){
                        shape.push_back(Pixel(i - anchorHeight, j - anchorWidth, (n - distManh)/n));
                    }
                }
            }
        }
    }


}


StructuringElement::StructuringElement(const StructuringElement& other){
    width = other.width;
    height = other.height;
    anchorHeight = other.anchorHeight;
    anchorWidth = other.anchorWidth;

    // deep copy of the other's shape
    for (std::vector<Pixel>::const_iterator it = other.shape.begin(); it != other.shape.end(); ++it){
        shape.push_back(Pixel(*it));
    }
}


bool StructuringElement::include(bool** data, int dataWidth, int dataHeight, int row, int col){
    for (std::vector<Pixel>::iterator it = shape.begin(); it != shape.end(); ++it){
        if (data[std::max(0, std::min(dataHeight-1, row + it->getRow()))][std::max(0, std::min(dataWidth-1, col + it->getCol()))]){
            return true;
        } 
    }
    return false;
}

bool StructuringElement::hit(bool** data, int dataWidth, int dataHeight, int row, int col){
    for (std::vector<Pixel>::iterator it = shape.begin(); it != shape.end(); ++it){
        if (!data[std::max(0, std::min(dataHeight-1, row + it->getRow()))][std::max(0, std::min(dataWidth-1, col + it->getCol()))]){
            return false;
        } 
    }
    return true;
}


double StructuringElement::min(double** data, int dataWidth, int dataHeight, int row, int col){
    /* case where data[i][j] == -1 */
    if (data[row][col] < 0){
        return -1;
    }

    double minValue = 1;
    double tempValue;

    for (std::vector<Pixel>::iterator it = shape.begin(); it != shape.end(); ++it){
        tempValue = data[std::max(0, std::min(dataHeight-1, row + it->getRow()))][std::max(0, std::min(dataWidth-1, col + it->getCol()))] - it->getVal();
        if (tempValue < minValue){
            minValue = tempValue;
        } 
    }

    return std::min(1., std::max(0., minValue + 1));

}

double StructuringElement::max(double** data, int dataWidth, int dataHeight, int row, int col){
    /* case where data[i][j] == -1 */
    if (data[row][col] < 0){
        return -1;
    }

    double maxValue = 0;
    double tempValue;

    for (std::vector<Pixel>::iterator it = shape.begin(); it != shape.end(); ++it){
        tempValue = data[std::max(0, std::min(dataHeight-1, row + it->getRow()))][std::max(0, std::min(dataWidth-1, col + it->getCol()))] + it->getVal();
        if (tempValue > maxValue){
            maxValue = tempValue;
        } 
    }


    return std::min(1., std::max(0., maxValue - 1));
}

void StructuringElement::toPGM(std::string path){
    /* converts the vector of the structuring element to a 2D array */
    unsigned int** data = new unsigned int*[height];
    for (int i = 0; i < height; i++){
        data[i] = new unsigned int[width];
        for (int j = 0; j < width; j++){
            data[i][j] = 0;
        }
    } 


    for (std::vector<Pixel>::iterator it = shape.begin(); it != shape.end(); ++it){
        data[it->getRow() + anchorHeight][it->getCol() + anchorWidth] = (unsigned int) std::min(255., std::max(0., (it->getVal() * 255)));
    }

    std::ofstream PGMFile;
    PGMFile.open(path);
    PGMFile << "P2" << std::endl;
    PGMFile << std::to_string(width) << " " << std::to_string(height) << std::endl;
    PGMFile << std::to_string(255) << std::endl;


    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            PGMFile << std::to_string(data[i][j]) << " ";
        }
        PGMFile << std::endl;
    } 
    PGMFile << std::endl;

    /* closing */
    PGMFile.close();
    free_2Darray<unsigned int>(data, height);

}

 /**************************************
        Binar morphological filters
 ***************************************/

double determineThreshold(double** data, int width, int height)
{
    int sizeOfImage = width * height;
    int threshold = 0;
    double var_max = 0.0;
    double sum = 0.0;
    double sumB = 0.0;
    double quantile_1 = 0.0;
    double quantile_2 = 0.0;
    double average_1 = 0.0;
    double average_2 = 0.0;
    int max_itensity  = 256;
    double variance_1 = 0.0;

    double *histogram;
    histogram = new double[max_itensity];

    std::ofstream myfile;
    myfile.open ("../data/results/histogram.txt");
    myfile << "x" << "," << "y" << std::endl;

    std::ofstream myfile2;
    myfile2.open ("../data/results/interclassvariance.txt");
    myfile2 << "x" << "," << "y" << std::endl;

    for(int i = 0; i < max_itensity; i++){
        histogram[i] = 0.0;
    }

    uchar* imageDataInt = new uchar[sizeOfImage];

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            imageDataInt[i * width + j] = (uchar) (data[i][j] * 255);
        }
    }

    for(int i = 0; i < sizeOfImage; i++){
        histogram[imageDataInt[i]] += 1.0;
    }
    for(int i = 0; i < max_itensity; i++){
        myfile << i << "," << histogram[i] << std::endl;
        histogram[i] = histogram[i] / sizeOfImage;
    }

    myfile.close();

    for(int i = 0; i < max_itensity; i++){
        sum += i * histogram[i];
    }

    for(int i = 0; i < max_itensity; i++){
        quantile_1 += histogram[i];
        quantile_2 = 1 - quantile_1;

        sumB = sumB + i * histogram[i];
        average_1 = sumB / quantile_1;
        average_2 = (sum - sumB)/ quantile_2;

        variance_1 = quantile_1 * quantile_2 * pow(average_1 - average_2, 2);
        myfile2 << i << "," << variance_1 << std::endl;

        if(variance_1 > var_max){
            threshold = i;
            var_max = variance_1;
        }

    }
    myfile2.close();

    delete[] imageDataInt;
    delete[] histogram;

    
    return 1.0 - (double)(threshold)/255.0;
}


bool** binarizeData(double** data, int width, int height)
{

    double threshold = determineThreshold(data, width, height);

    bool** binarizedData = new bool*[height];

    for (int i = 0; i < height; i++){
        binarizedData[i] = new bool[width];
        for (int j = 0; j < width; j++){
            binarizedData[i][j] = (bool) (data[i][j] >= threshold);
        }
    }


    return binarizedData;
}


bool** erosion(bool** data, int width, int height, StructuringElement& selem) 
{
    bool** resultData = new bool*[height];

    for (int i = 0; i < height; i++){
        resultData[i] = new bool[width];
        for (int j = 0; j < width; j++){
            resultData[i][j] = selem.include(data, width, height, i, j); 
        }
    }

    free_2Darray<bool>(data, height);

    return resultData;
}


bool** dilation(bool** data, int width, int height, StructuringElement& selem) 
{
    bool** resultData = new bool*[height];

    for (int i = 0; i < height; i++){
        resultData[i] = new bool[width];
        for (int j = 0; j < width; j++){
            resultData[i][j] = selem.hit(data, width, height, i, j); 
        }
    }

    free_2Darray<bool>(data, height);

    return resultData;
}


Image Image::morphologicalFilterBin(MF_TYPE type, StructuringElement& selem){
    bool** binarizedData = binarizeData(this->imageData, imageWidth, imageHeight);

    if (type == MF_NONE){
        return Image(binarizedData, imageWidth, imageHeight);
    }
    else if (type == MF_EROSION){
        return Image(erosion(binarizedData, imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    else if (type == MF_DILATION){
        return Image(dilation(binarizedData, imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    else if (type == MF_OPENING){
        return Image(dilation(erosion(binarizedData, imageWidth, imageHeight, selem), imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    else if (type == MF_CLOSING){
        return Image(erosion(dilation(binarizedData, imageWidth, imageHeight, selem), imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    return Image(binarizedData, imageWidth, imageHeight);
    
}



 /******************************************
        Grayscale morphological filters
 *******************************************/
double** erosion(double** data, int width, int height, StructuringElement& selem) 
{
    double** resultData = new double*[height];

    for (int i = 0; i < height; i++){
        resultData[i] = new double[width];
        for (int j = 0; j < width; j++){
            resultData[i][j] = selem.max(data, width, height, i, j); 
        }
    }

    free_2Darray<double>(data, height);

    return resultData;
}

double** dilation(double** data, int width, int height, StructuringElement& selem) 
{
    double** resultData = new double*[height];

    for (int i = 0; i < height; i++){
        resultData[i] = new double[width];
        for (int j = 0; j < width; j++){
            resultData[i][j] = selem.min(data, width, height, i, j); 
        }
    }

    free_2Darray<double>(data, height);

    return resultData;
}


Image Image::morphologicalFilterGrayscale(MF_TYPE type, StructuringElement& selem){
    double** grayscaleData = copy_2Darray<double>(this->imageData, imageHeight, imageWidth);


    if (type == MF_NONE){
        return Image(grayscaleData, imageWidth, imageHeight);
    }
    else if (type == MF_EROSION){
        return Image(erosion(grayscaleData, imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    else if (type == MF_DILATION){
        return Image(dilation(grayscaleData, imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    else if (type == MF_OPENING){
        return Image(dilation(erosion(grayscaleData, imageWidth, imageHeight, selem), imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    else if (type == MF_CLOSING){
        return Image(erosion(dilation(grayscaleData, imageWidth, imageHeight, selem), imageWidth, imageHeight, selem), imageWidth, imageHeight);
    }
    return Image(grayscaleData, imageWidth, imageHeight);
}


Image Image::morphologicalFilterGrayscaleCentered(MF_TYPE type, SELEM_SHAPE selemType, bool flat){
    double** resultData = new double*[imageHeight];
    double dist;
    const double p = 2.;

    /* determine the barycenter of the image */
    int retW, retH;
    barycenter(imageData, imageWidth, imageHeight, retW, retH);

    unsigned int maxSize;
    double divider;
    if (!flat){
        maxSize = 15;
        divider = 12;
    }
    else{
        maxSize = 4;
        divider = 40;
    }

    std::vector<StructuringElement> selems;
    for (unsigned int i = 0; i < maxSize; i++){
        selems.push_back(StructuringElement(selemType, i+1, flat));
    }


    if (type == MF_EROSION){
        for (int i = 0; i < imageHeight; i++){
            resultData[i] = new double[imageWidth];
            for (int j = 0; j < imageWidth; j++){
                dist = pow(p * pow(retW - j, 2) + pow(retH - i, 2), .5);
                resultData[i][j] = selems[std::min((unsigned int)(dist/divider), maxSize-1)].max(imageData, imageWidth, imageHeight, i, j); 
            }
        }

    }
    else if (type == MF_DILATION){
        for (int i = 0; i < imageHeight; i++){
            resultData[i] = new double[imageWidth];
            for (int j = 0; j < imageWidth; j++){
                dist = pow(p * pow(retW - j, 2) + pow(retH - i, 2), .5);
                resultData[i][j] = selems[maxSize-1 -std::min((unsigned int)(dist/divider), maxSize-1)].min(imageData, imageWidth, imageHeight, i, j); 
            }
        }

    }



    return Image(resultData, imageWidth, imageHeight);
}