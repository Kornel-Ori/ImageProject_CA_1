//
// Created by floodd on 26/01/2026.
//

#include "Image.h"

#include <iostream>

/**
 * This function creates the image object, and then loads in the image from the given filename.
 *
 * @param filename: The name of the file to load
 * /
 */
MyImage::MyImage(string filename): fileName(filename) {
    this->load(filename);
}

/**
 *  This function will reload the original image.
 */
void MyImage::reload() {
    this->load(fileName);
}

/**
 *  This function is responsible for drawing the image scaling to fit on the screen. It also draws a grey
 *  background behind the image to allow for scaling.
 */
void MyImage::draw(sf::RenderTarget &target, sf::RenderStates states)const {

    float x = 0, y = 0;
    int type = this->size.x > this->size.y? LANDSCAPE: PORTRAIT;
    float step=1;
    float startX=this->position.x, startY=this->position.y;
    float dimW = this->targetSize.x;
    float dimH = this->targetSize.y;


    if (this->size.x < this->targetSize.x && this->size.y < this->targetSize.y) {

        dimW = this->size.x;
        startY += (this->targetSize.y - this->size.y)/2.0f;
        dimH = this->size.y;
        startX += (this->targetSize.x- this->size.x)/2.0f;;
    }
    else {
        if (this->size.x / this->targetSize.x > this->size.y / this->targetSize.y) {
            step = this->size.x / this->targetSize.x;
            dimH = this->size.y/step;
            startY += (this->targetSize.y - dimH)/2;;
        }
        else {
            step = this->size.y / (float)this->targetSize.y;
            dimW = this->size.x/step;
            startX +=  (this->targetSize.x - dimW)/2;
        }
    }
    float xStep = 0, yStep=0;
    for (float i = 0; i < dimH; ++i) {
        for (float j = 0; j < dimW; ++j)
        {
            x = (int)xStep;
            y = (int)yStep;
            int pix = (y*this->size.x) + x;
            sf::VertexArray circle(sf::PrimitiveType::Points, 1);
            circle[0].color = sf::Color(this->pixels[pix].r,this->pixels[pix].g,this->pixels[pix].b);
            circle[0].position = {j+startX,i+startY};

            target.draw(circle);
            xStep+=step;
        }
        xStep = 0;
        yStep+=step;
    }
}

/**
* The draw function will scale the image to the maximum defined by this function
*
 * @param target : the target size of the image when displayed.
 */
void MyImage::setTargetSize(sf::Vector2f target) {
    this->targetSize = target;
}

/**
 *
 * @return The size of the original image defined in the PPM file.
 */
sf::Vector2f MyImage::getSize() {
    return this->size;
}

/**
 * This function opens the given ppm file and attempts to read in the data to a continuous vector of pixels defined as
 * RGB structs.
 *
 * @param filename the file to load.
 */
void MyImage::load(string filename) {
    ifstream ifs;

    string dummy;
    ifs.open(filename, std::ios::binary); // need to spec. binary mode for Windows users
    try {
        if (ifs.fail()) { throw("Can't open input file"); }
        std::string header;
        int w, h, b;
        // Reads in the first string to check it starts with "P6" defining a ppm file. If not throws an exception as file
        // can't be read.
        ifs >> header;
        if (strcmp(header.c_str(), "P6") != 0) throw("Can't read input file");
        getline(ifs, dummy);
        /* Checks to see if there is a comment after the first line */
        char c;
        c = ifs.peek();
        if (c == '#') {
            getline(ifs, dummy);
        }
        // Read in the width and height of the image.
        ifs >> w >> h >> b;
        // set the size within the image object
        this->size = {(float)w,(float)h};
        ifs.ignore(256, '\n'); // skip empty lines in necessary until we get to the binary data
        unsigned char pix[3];
        this->pixels.clear();// remove any existing pixel data
        // read each pixel one by one and convert bytes to floats
        for (int i = 0; i < w * h; ++i) {
            ifs.read(reinterpret_cast<char *>(pix), 3);
            RGB pixel;
            pixel.r = pix[0];
            pixel.g = pix[1];
            pixel.b = pix[2];
            this->pixels.push_back(pixel);
        }

        ifs.close();
    }
    catch (const char *err) {
        fprintf(stderr, "%s\n", err);
        ifs.close();
    }
}
void MyImage::save()
{
    string filename = "Output/SavedImage.ppm";
    if (this->size.x == 0 || this->size.y == 0) { fprintf(stderr, "Can't save an empty image\n"); return; }
    std::ofstream ofs;
    try {
        ofs.open(filename, std::ios::binary); // need to spec. binary mode for Windows users
        if (ofs.fail()) throw("Can't open output file");
        ofs << "P6\n" << this->size.x << " " <<  this->size.y << "\n255\n";
        unsigned char r, g, b;
        // loop over each pixel in the image, clamp and convert to byte format
        for (int i = 0; i <  this->size.x *  this->size.y; ++i) {
            r = static_cast<unsigned char>(this->pixels[i].r);
            g = static_cast<unsigned char>(this->pixels[i].g);
            b = static_cast<unsigned char>(this->pixels[i].b);
            ofs << r << g << b;
        }
        ofs.close();
    }
    catch (const char *err) {
        fprintf(stderr, "%s\n", err);
        ofs.close();
    }
}

void MyImage::filterRed()
{
    for (auto &p : pixels) {
        p.g = 0;
        p.b = 0;
    }
   cout << "Filter Red" << endl;

}

// loop through every pixel in the image
// remove red and blue channels so only green remains (same for the others)
void MyImage::filterGreen()
{
    for (auto &p : pixels) {
        p.r = 0;
        p.b = 0;
    }
    cout << "Filter Green" << endl;
}
void MyImage::filterBlue()
{
    for (auto &p : pixels) {
        p.r = 0;
        p.g = 0;
    }
    cout << "Filter Blue" << endl;
}

// converts pixel to greyscale by averaging RGB values
// setting rgb to the same value removes the colour
void MyImage::greyScale()
{
    for (auto &p : pixels) {
        unsigned char grey = (p.r + p.g + p.b) / 3;
        p.r = grey;
        p.g = grey;
        p.b = grey;
    }

    cout << "Greyscale Applied" << endl;
}

// loop through each row
// swap pixels from left side with matching pixel on right side
// only loop to half the width to avoid swapping twice (same with vertical but with columns)
void MyImage::flipHorizontal()
{
    int width = (int)this->size.x;
    int height = (int)this->size.y;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width / 2; x++) {

            int leftIndex  = (y * width) + x;
            int rightIndex = (y * width) + (width - 1 - x);

            std::swap(pixels[leftIndex], pixels[rightIndex]);
        }
    }

    cout << "Flip Horizontal" << endl;
}

void MyImage::flipVertical()
{
    int width = (int)this->size.x;
    int height = (int)this->size.y;

    for (int y = 0; y < height / 2; y++) {
        for (int x = 0; x < width; x++) {

            int topIndex    = (y * width) + x;
            int bottomIndex = ((height - 1 - y) * width) + x;

            std::swap(pixels[topIndex], pixels[bottomIndex]);
        }
    }

    cout << "Flip Vertical" << endl;
}

// moves each pixel to its new rotated position and swap width/height
void MyImage::rotateLeft()
{
    //original size
    int oldWidth = static_cast<int>(size.x);
    int oldHeight = static_cast<int>(size.y);

    //new array
    std::vector<RGB> newPixels(oldWidth * oldHeight);

    //loops through pixels
    for (int y = 0; y < oldHeight; y++)
    {
        for (int x = 0; x < oldWidth; x++)
        {
            int oldIndex = y * oldWidth + x;

            //new rotated position
            int newX = oldHeight - 1 - y;
            int newY = x;

            int newIndex = newY * oldHeight + newX;

            //copy pixel
            newPixels[newIndex] = pixels[oldIndex];
        }
    }

    //replace pixel
    pixels = newPixels;

    //swap width and height
    size.x = oldHeight;
    size.y = oldWidth;

    std::cout << "Rotate 90 Clockwise\n";
}

//mirrors left side pixels to the matching right side
void MyImage::mirror()
{
    int width = static_cast<int>(size.x);
    int height = static_cast<int>(size.y);

    for (int y = 0; y < height; y++)
    {
        //only go to middle
        for (int x = 0; x < width / 2; x++)
        {
            int leftIndex = y * width + x;
            int rightIndex = y * width + (width - 1 - x);

            //copy left pixel to right
            pixels[rightIndex] = pixels[leftIndex];
        }
    }
    std::cout << "Mirror Left to Right\n";
}

//box blue averages colour of surrouding pixelsvoid MyImage::blur()
{
    int width = static_cast<int>(size.x);
    int height = static_cast<int>(size.y);

    //copy of pixels
    std::vector<RGB> newPixels = pixels;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int totalR = 0;
            int totalG = 0;
            int totalB = 0;
            int count = 0;

            //checks surrounding 3x3
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = x + dx;
                    int ny = y + dy;

                    //ignores the pixels outside
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                    {
                        int index = ny * width + nx;

                        totalR += pixels[index].r;
                        totalG += pixels[index].g;
                        totalB += pixels[index].b;
                        count++;
                    }
                }
            }

            int currentIndex = y * width + x;

            //average colour
            newPixels[currentIndex].r = totalR / count;
            newPixels[currentIndex].g = totalG / count;
            newPixels[currentIndex].b = totalB / count;
        }
    }

    //replaces pixels
    pixels = newPixels;

    std::cout << "Box Blur Applied\n";
}
