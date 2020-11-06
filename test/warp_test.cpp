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

double compute_error_2_rotation(const Image& orig, double angle, INTERP_METHOD method){

    int height = orig.getHeight();
    int width = orig.getWidth();
    Image rotated(orig.rotation_interpol(angle, height/2, width/2, method).rotation_interpol(-angle, height/2, width/2, method));

    return orig.meanSquaredError(rotated, 1);
}

double time_mesure(const Image& orig, double angle, INTERP_METHOD method, int repeat = 5){
    int height = orig.getHeight();
    int width = orig.getWidth();
    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();
    double sum = 0;

    for (int i = 0; i < repeat; i++){
        t1 = std::chrono::high_resolution_clock::now();
        Image rotated(orig.rotation_interpol(angle, height/2, width/2, method));
        t2 = std::chrono::high_resolution_clock::now();
        sum = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    }

    sum /= repeat;
    return sum / 1000;
    
}



int main(){
    Image cleanFinger("../data/original/clean_finger.png");
    /* rotation with 3 different interpolations */
    cleanFinger.rotation_interpol(0.8, cleanFinger.getHeight()/2, cleanFinger.getWidth()/2, INTERP_NEAREST).display();
    cleanFinger.rotation_interpol(0.8, cleanFinger.getHeight()/2, cleanFinger.getWidth()/2, INTERP_BILINEAR).display();
    cleanFinger.rotation_interpol(0.8, cleanFinger.getHeight()/2, cleanFinger.getWidth()/2, INTERP_BICUBIC).display();

    /* translation with 3 different interpolations */
    cleanFinger.translation_interpol(100, 100, INTERP_NEAREST).display();
    cleanFinger.translation_interpol(100, 100, INTERP_BILINEAR).display();
    cleanFinger.translation_interpol(100, 100, INTERP_BICUBIC).display();

    /* rain effect functions */
    cleanFinger.rainEffect(HORIZONTAL, 100, 100, 50);
    cleanFinger.display();
    cleanFinger.revert();

    /* swirl functions */
    cleanFinger.swirlImage(cleanFinger.getWidth()/2 + 50, cleanFinger.getHeight()/2, 30, 50);
    cleanFinger.display();
    cleanFinger.revert();


    /* result csv files for time and error versus the angle, depending on the used interpolation */
    // cout << "Method, Angle, Error, Time" << endl; 

    // for (double angle = 0.; angle < 3.14; angle += 0.2){
    //     cout << "0," << angle << "," << compute_error_2_rotation(cleanFinger, angle, INTERP_NEAREST) << "," << time_mesure(cleanFinger, angle,  INTERP_NEAREST) << endl;
    //     cout << "1," << angle << "," << compute_error_2_rotation(cleanFinger, angle, INTERP_BILINEAR) << "," << time_mesure(cleanFinger, angle, INTERP_BILINEAR) << endl;
    //     cout << "3," << angle << "," << compute_error_2_rotation(cleanFinger, angle, INTERP_BICUBIC) << "," << time_mesure(cleanFinger, angle,  INTERP_BICUBIC) << endl;
    // }

    
    /*// error vs angle and method
    cout << "Angle, 0, 1, 3" << endl;

    for (double angle = 0.; angle < 3.14; angle += 0.1){
        cout << angle << "," << compute_error_2_rotation(cleanFinger, angle, 0) << "," << compute_error_2_rotation(cleanFinger, angle, 1) << "," << compute_error_2_rotation(cleanFinger, angle, 3) << endl;

    }
    */
   /*// time vs angle and method
    cout << "Angle, 0, 1, 3" << endl;

    for (double angle = 0.; angle < 3.14; angle += 0.1){
        cout << angle << "," << time_mesure(cleanFinger, angle, 0) << "," << time_mesure(cleanFinger, angle, 1) << "," << time_mesure(cleanFinger, angle, 3) << endl;

    }
    */



    /* gregoire */
    /*
    Image myImage("../data/original/weak_finger_small.png");
    double predicted_p = opti_greedy(myImage, 20, 0.052, 1);
    std::cout << predicted_p << std::endl;

    */
    // loss_generator_translation("../data/original/weak_finger_small.png", 
    //                         "../data/results/loss_function_translation1.txt", 
    //                         "p_x", 
    //                         "loss", 1);

    // loss_generator_translation("../data/original/weak_finger_small.png", 
    //                             "../data/results/loss_function_translation2.txt", 
    //                             "p_x", 
    //                             "loss", 2);

    // loss_generator_translation("../data/original/weak_finger_small.png", 
    //                             "../data/results/loss_function_translation3.txt", 
    //                             "p_x", 
    //                             "loss", 3);

    // loss_generator_translation("../data/original/weak_finger_small.png", 
    //                             "../data/results/loss_function_translation5.txt", 
    //                             "p_x", 
    //                             "loss", 5);

    // loss_generator_translation("../data/original/weak_finger_small.png", 
    //                             "../data/results/loss_function_translation8.txt", 
    //                             "p_x", 
    //                             "loss", 8);

    // correlation_generator_rot("../data/original/weak_finger_small.png", 
    //                             "../data/results/corr_function_rotation.txt", 
    //                             "theta", 
    //                             "correlation rate", 1);

    // std::cout << "hey" << std::endl;


    // correlation_generator_rot("../data/original/weak_finger_small.png", 
    //                             "../data/results/corr_function_rotation2.txt", 
    //                             "theta", 
    //                             "correlation rate", 2);

    // std::cout << "hey" << std::endl;


    // correlation_generator_rot("../data/original/weak_finger_small.png", 
    //                             "../data/results/corr_function_rotation6.txt", 
    //                             "theta", 
    //                             "correlation rate", 6);

    // std::cout << "hey" << std::endl;


    // correlation_generator_rot("../data/original/weak_finger_small.png", 
    //                             "../data/results/corr_function_rotation10.txt", 
    //                             "theta", 
    //                             "correlation rate", 10);


    return 0;
}