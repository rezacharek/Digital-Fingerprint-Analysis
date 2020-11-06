#include <iostream>
#include <stdlib.h>
#include "image.h"
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <string.h>

// #include <unistd.h> // for STDOUT_FILENO

// #include <term.h>

// void ClearScreen()
//   {
//   if (!cur_term)
//     {
//     int result;
//     setupterm( NULL, STDOUT_FILENO, &result );
//     if (result <= 0) return;
//     }

//   putp( tigetstr( "clear" ) );
//   }

using namespace cv;


int main(){
    std::system("clear");   // std::cout << "##############################################################" << std::endl;
    std::cout << "Welcome to the demonstration executable of the Digital Fingerprint Analysis C++ Library" << std::endl;
    std::cout << "We will start first by showing the basic functions that you use."  << std::endl;
    std::cout << std::endl << std::endl << std::endl;

    
    
    std::cout << "We first read the picture clean_finger.png and display it." << std::endl;
    Image demonstrationImage("./data/original/clean_finger.png");
    demonstrationImage.display("");
    std::system("clear"); 

    std::cout <<  "Now, using the function rectangleFill, we will fill a surface of the image with a chosen intensity" << std::endl;
    demonstrationImage.rectangleFill(50,50,150,150,0.4);
    demonstrationImage.display("");
    std::system("clear");


    demonstrationImage.revert();
    demonstrationImage.display("");

    std::cout << "Now we will show the symmetry function on the image with respect to the X axis" << std::endl;
    demonstrationImage.symmetry(SYM_X).display("");
    std::system("clear");

    std::cout << "Now we will show the symmetry function on the image with respect to the Y axis" << std::endl;
    demonstrationImage.symmetry(SYM_Y).display("");
    std::system("clear");
    
    std::cout << "Now we will show the imageFilling function on a cercle." << std::endl;
    demonstrationImage.imageFilling( demonstrationImage.getHeight()/2, demonstrationImage.getWidth()/2, 50, 50, EXPONENTIAL_DECREASING, 0, 2, true );
    demonstrationImage.display("");
    demonstrationImage.revert();
    std::system("clear");
    
    std::cout << "Now we will show the imageFilling function on an ellipse." << std::endl;
    demonstrationImage.imageFilling( demonstrationImage.getHeight()/2, demonstrationImage.getWidth()/2, 50, 70, EXPONENTIAL_DECREASING, 25, 2, false );
    demonstrationImage.display("");
    demonstrationImage.revert();
    std::system("clear");
    
    std::cout << "Now we will show the rainEffect function on a square." << std::endl;
    demonstrationImage.rainEffect(DIAGONAL, 60, 60, 150);
    demonstrationImage.display("");
    std::system("clear");

    // std::cout

    std::system("clear");
    std::cout << "C++ Library of Digital Fingerprint Analysis, a coursework of MSIAM M1 of Grenoble INP-Ensimag and Université Grenoble-Alpes" << std::endl;
    std::cout << "Authors: Maya Costantini, Diana Farkhat, Grégoire Lambert, Matthias Lotta, Roman Edward Zacharek " << std::endl; 
    return 0;
}
