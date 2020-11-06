#include "image.h"
#include "warp.h"
#include "cost.h"

#include <math.h>
#include <iostream>
#include <chrono>

// plot lib
#include <opencv2/highgui.hpp>
#include <opencv2/plot.hpp>
#include <fstream>

using namespace std;
using namespace cv;


int loss_generator_translation_exemple_rapport(const std::string pathPicOriginal, 
                                                std::string pathDataOutput, 
                                                std::string labelX, 
                                                std::string labelY, 
                                                int orderSubPix){
    Image myImage(pathPicOriginal);
    Image targetImage = myImage.translation_interpol(50, 0);
    /* prepare data out file */
    std::ofstream myfile;
    myfile.open (pathDataOutput);
    myfile << labelX << "," << labelY<< std::endl;

    
    /* parameters for translation */
    // int width = myImage.getWidth();
    // int height = myImage.getHeight();

    int nbPoints = 1000;
    double posX;
    posX = 0;
    double step = 150. / ((double) nbPoints);

    // myImage.display(" ");

    for(int i = 0; i < nbPoints; i++){
        Image newImage = myImage.translation_interpol(posX, 0, INTERP_BILINEAR, -1);
        // if(i==0 || i==nbPoints -1) newImage.display(" ");

        myfile << posX << "," << targetImage.meanSquaredError(newImage, orderSubPix) << std::endl;
        posX += step;      
    }
    myfile.close();

    return 0;
}






int main(){
    
    // Image myImage("../data/original/weak_finger_small.png");
    // double predicted_p = opti_greedy(myImage, 20, 0.052, 1);
    // std::cout << predicted_p << std::endl;


    loss_generator_translation_exemple_rapport("../data/original/tx_finger.png", 
                                                "../data/results/loss_function_translation_tx.txt", 
                                                "p_x", 
                                                "loss", 1);

                                                
/*
    # generating useful data coordinates to plot

    loss_generator_translation("../data/original/weak_finger_small.png", 
                                "../data/results/loss_function_translation2.txt", 
                                "p_x", 
                                "loss", 2);

    loss_generator_translation("../data/original/weak_finger_small.png", 
                                "../data/results/loss_function_translation3.txt", 
                                "p_x", 
                                "loss", 3);

    loss_generator_translation("../data/original/weak_finger_small.png", 
                                "../data/results/loss_function_translation5.txt", 
                                "p_x", 
                                "loss", 5);

    loss_generator_translation("../data/original/weak_finger_small.png", 
                                "../data/results/loss_function_translation8.txt", 
                                "p_x", 
                                "loss", 8);

    correlation_generator_rot("../data/original/weak_finger_small.png", 
                                "../data/results/corr_function_rotation.txt", 
                                "theta", 
                                "correlation rate", 1);

    std::cout << "hey" << std::endl;


    correlation_generator_rot("../data/original/weak_finger_small.png", 
                                "../data/results/corr_function_rotation2.txt", 
                                "theta", 
                                "correlation rate", 2);

    std::cout << "hey" << std::endl;


    correlation_generator_rot("../data/original/weak_finger_small.png", 
                                "../data/results/corr_function_rotation6.txt", 
                                "theta", 
                                "correlation rate", 6);

    std::cout << "hey" << std::endl;


    correlation_generator_rot("../data/original/weak_finger_small.png", 
                                "../data/results/corr_function_rotation10.txt", 
                                "theta", 
                                "correlation rate", 10);

*/


    return 0;
}