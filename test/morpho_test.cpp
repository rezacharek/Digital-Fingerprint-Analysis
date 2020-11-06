#include <iostream>

#include "image.h"
#include "utils.h"
#include "morpho_filter.h"

using namespace cv;
using namespace std;

/* Applies several time a morphological filter on the same image */
void morphoFilterSucc(Image& initialImage, int n, StructuringElement& selem, MF_TYPE type = MF_EROSION, string name = "erode"){
    for (int i = 0; i < n; i++){
        initialImage = initialImage.morphologicalFilterGrayscale(type, selem);
        // initialImage.save("../data/result/" + name + to_string(i+1) + ".png");
        initialImage.display(name + to_string(i+1));
    }
}

/* saves all structuring elements of a map of structuring elements to pgm format */
void selemsToPGM(map<string, StructuringElement> selems){
    for (map<string, StructuringElement> ::iterator itr = selems.begin(); itr != selems.end(); itr++){
        itr->second.toPGM("../data/selems/" + itr->first + ".pgm");
    }
}


int main()
{
    /* Image loading */
    Image cleanFinger("../data/original/clean_finger.png");

    /* Creation of useful structuring elements 
    Size varying from 1 to maxSize, 4 kinds of structuring elements, flat and non flat */
    map<string, StructuringElement> selems;
    SELEM_SHAPE types[4] = {SELEM_CROSS, SELEM_CIRCLE, SELEM_DIAMOND, SELEM_SQUARE};
    std::string shapes[4] = {"cross", "circle", "diamond", "square"};
    unsigned int maxSize = 10;
    for (int i = 0; i < 4; i++){
        for (unsigned int j = 1; j <= maxSize; j++){
            selems[shapes[i] + to_string(j) + "f"] = StructuringElement(types[i], j, true);
            selems[shapes[i] + to_string(j) + "nf"] = StructuringElement(types[i], j, false);
        }
    }

    /* Best approximations of dry and moist effect for flat and non flat structuring elements */
    cleanFinger.morphologicalFilterGrayscaleCentered(MF_EROSION, SELEM_DIAMOND, true).display("Localized erosion - flat");
    cleanFinger.morphologicalFilterGrayscaleCentered(MF_DILATION, SELEM_DIAMOND, true).display("Localized dilation - flat");
    cleanFinger.morphologicalFilterGrayscaleCentered(MF_EROSION, SELEM_DIAMOND, false).display("Localized erosion - non flat");
    cleanFinger.morphologicalFilterGrayscaleCentered(MF_DILATION, SELEM_DIAMOND, false).display("Localized dilation - non flat");

    /* displays dilation for all created structuring elements */
    // for (map<string, StructuringElement> ::iterator itr = selems.begin(); itr != selems.end(); itr++){
    //     cleanFinger.morphologicalFilterGrayscale(MF_DILATION, itr->second).display(itr->first);
    // }


    /* saves all structuring elements for visualisation*/
    selemsToPGM(selems);


    /* binarized image */
    cleanFinger.morphologicalFilterBin(MF_NONE, selems["cross1f"]).display("No filter, binarization");    // no filter, only binarization
    cleanFinger.morphologicalFilterBin(MF_EROSION, selems["cross1f"]).display("Simple erosion on binarized image");
    cleanFinger.morphologicalFilterBin(MF_OPENING, selems["cross1f"]).display("Simple opening on binarized image");

    /* gray-scale images */
    cleanFinger.morphologicalFilterGrayscale(MF_EROSION, selems["circle2f"]).display("Grayscale erosion with flat circle size 2*2+1");
    cleanFinger.morphologicalFilterGrayscale(MF_DILATION, selems["circle2f"]).display("Grayscale dilation with flat circle size 2*2+1");
    cleanFinger.morphologicalFilterGrayscale(MF_EROSION, selems["square2f"]).display("Grayscale erosion with flat circle size 2*2+1");
    cleanFinger.morphologicalFilterGrayscale(MF_DILATION, selems["square2f"]).display("Grayscale dilation with flat circle size 2*2+1");
    

    /* multiple erosion */
    cleanFinger.morphologicalFilterGrayscale(MF_DILATION, selems["diamond1f"]).display("diamond flat size 2*1+1");
    cleanFinger.morphologicalFilterGrayscale(MF_DILATION, selems["diamond2f"]).display("diamond flat size 2*2+1");
    cleanFinger.morphologicalFilterGrayscale(MF_DILATION, selems["diamond3f"]).display("diamond flat size 2*3+1");
    morphoFilterSucc(cleanFinger, 3, selems["diamond1f"], MF_DILATION, "dilate");   // the images are supposed to be identical pairwise


}
