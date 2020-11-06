#include <iostream>
#include "utils.h"
#include "image.h"
#include <math.h>
#include <chrono>
#include <fstream>

void barycenter(double** data, int width, int height, int& retW, int& retH){
    double imSum = .0;

    double dretW = 0;
    double dretH = 0;

    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            if (data[i][j] >= 0){
                imSum += 1 - data[i][j];
                dretH += (1 - data[i][j]) * i;
                dretW+= (1 - data[i][j]) * j;
            }
        }
    }

    retW = dretW / imSum;
    retH = dretH / imSum;

}



void barycenter(const Image& im, int& retW, int& retH){
    double imSum = .0;

    double dretW = 0;
    double dretH = 0;

    for (int i = 0; i < im.getHeight(); i++){
        for (int j = 0; j < im.getWidth(); j++){
            if (im(i, j) >= 0){
                imSum += 1 - im(i, j);
                dretH += (1 - im(i, j)) * i;
                dretW += (1 - im(i, j)) * j;
            }
        }
    }

    retW = dretW / imSum;
    retH = dretH / imSum;

}

double Coefficients(COEFFICIENT_TYPE functionType, double x, float k){

    switch(functionType){
        case FUNCTION_1:
            return 1/(1+pow(x,k));
            break;
        case FUNCTION_2:
            return pow(1+x,-x);
            break;
        case EXPONENTIAL_DECREASING:
            return exp(-pow(x, k));
            break;
        default:
            perror("Such function doesn't exist.");
            return 0;
            break;
    }
}


int loss_generator_translation(const std::string pathPicOriginal, 
                                std::string pathDataOutput, 
                                std::string labelX, 
                                std::string labelY, 
                                int orderSubPix){
    Image myImage(pathPicOriginal);
    /* prepare data out file */
    std::ofstream myfile;
    myfile.open (pathDataOutput);
    myfile << labelX << "," << labelY<< std::endl;

    
    /* parameters for translation */
    // int width = myImage.getWidth();
    // int height = myImage.getHeight();

    int nbPoints = 1000;
    double posX, deltaX;
    deltaX = 50;
    posX = -deltaX;
    double step = 2 * deltaX / nbPoints;

    // myImage.display(" ");

    for(int i = 0; i < nbPoints; i++){
        Image newImage = myImage.translation_interpol(posX, 0, INTERP_BILINEAR, -1);
        if(i==0 || i==nbPoints -1) newImage.display(" ");
        myfile << posX << "," << myImage.meanSquaredError(newImage, orderSubPix) << std::endl;
        posX += step;      
    }
    myfile.close();

    return 0;
}



int correlation_generator_rot(const std::string pathPicOriginal, 
                                std::string pathDataOutput, 
                                std::string labelX, 
                                std::string labelY, 
                                int orderSubPix){
    Image myImage(pathPicOriginal);
    
    /* prepare data out file */
    std::ofstream myfile;
    myfile.open (pathDataOutput);
    myfile << labelX << "," << labelY << std::endl;

    /* parameters for rotation */
    int width = myImage.getWidth();
    int height = myImage.getHeight();
    int nbPoints = 1000;
    double theta, deltaTheta;
    deltaTheta = 3.14;
    theta = - deltaTheta;
    double step = 2 * deltaTheta / nbPoints;

    /* computation & filling data file*/
    for(int i = 0; i < nbPoints; i++){
        Image newImage = myImage.rotation_interpol(theta, width/2, height/2, INTERP_BILINEAR, -1);
        if(i==0 || i==nbPoints -1) newImage.display(" ");        
        myfile << theta << "," << myImage.correlationRate(newImage, orderSubPix) << std::endl;
        theta += step;
    }
    myfile.close();

    return 0;
}

int loss_generator_rot(const std::string pathPicOriginal, 
                                std::string pathDataOutput, 
                                std::string labelX, 
                                std::string labelY, 
                                int orderSubPix){
    Image myImage(pathPicOriginal);
    
    /* prepare data out file */
    std::ofstream myfile;
    myfile.open (pathDataOutput);
    myfile << labelX << "," << labelY << std::endl;

    /* parameters for rotation */
    int width = myImage.getWidth();
    int height = myImage.getHeight();
    int nbPoints = 1000;
    double theta, deltaTheta;
    deltaTheta = 3.14;
    theta = - deltaTheta;
    double step = 2 * deltaTheta / nbPoints;

    /* computation & filling data file*/
    for(int i = 0; i < nbPoints; i++){
        Image newImage = myImage.rotation_interpol(theta, width/2, height/2, INTERP_BILINEAR, -1);
        if(i==0 || i==nbPoints -1) newImage.display(" ");        
        myfile << theta << "," << myImage.meanSquaredError(newImage, orderSubPix) << std::endl;
        theta += step;
    }
    myfile.close();

    return 0;
}



double opti_greedy(Image& myImage, double p0, double optiRate, int isLoss){
    Image newImage = myImage;
    int nbStep = 1000;
    double p = p0;
    double costLeft, costRight;
    if (isLoss == 1){
        for (int i = 0; i < nbStep; i++){
            if( p < 0.1 && p > -0.1){
                std::cout << i << std::endl;
                return p;
            }
            newImage = myImage.translation_interpol(p * (1 - optiRate), 0);
            costLeft = newImage.meanSquaredError(myImage, 1);
            newImage = myImage.translation_interpol(p * (1 + optiRate), 0);
            costRight = newImage.meanSquaredError(myImage, 1);
            if(costLeft < costRight) p *=  1 - optiRate;
            else p *= 1 + optiRate;
        }
    }
    return p;
}



double* gradient_descent(double* p, const Image &imageTarget, const Image &imageInitial, double ethaTrans, double ethaRot, double hTrans, double hTheta){
    double pX = p[0];
    double pY = p[1];
    double pTheta = p[2];
    int i_c, j_c;
    /* initialisation baricentre i_c et j_c */
    barycenter(imageInitial, i_c, j_c);
    
    Image newImage = imageInitial.translation_interpol(pX , pY);
    newImage.rotation_interpol(pTheta, i_c, j_c);
    
    /* Euler Scheme forward 1st order on loss function and correlation rate */
    double diffX = imageTarget.meanSquaredError(newImage.translation_interpol(hTrans, 0), 1)
                  - imageTarget.meanSquaredError(newImage, 1); // Euler Scheme forward 1st order
    diffX /= hTrans;

    double diffY = imageTarget.meanSquaredError(newImage.translation_interpol(0, hTrans), 1) 
                  - imageTarget.meanSquaredError(newImage, 1);
    diffY /= hTrans;

    double diffTheta = imageTarget.correlationRate(newImage.rotation_interpol(pTheta, i_c, j_c), 1) 
                        - imageTarget.correlationRate(newImage, 1);
    diffTheta /= hTheta;
    
    /* gradient descent method */
    p[0] = pX - ethaTrans * diffX;
    p[1] = pY - ethaTrans * diffY;
    p[2] = pTheta + ethaRot * diffTheta; // "+" as we look for a maximun of the correlation_rate function
    return p;
}
