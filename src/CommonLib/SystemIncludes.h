#pragma once


//Headers from other non - standard, non - system libraries(for example, Qt, Eigen, etc).
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cvaux.h>
#include "opencv2/photo/photo.hpp"

//Headers from other "almost-standard" libraries(for example, Boost)


//Standard C++ headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

//Standard C headers(for example, cstdint, dirent.h, etc.)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <memory.h>



// These types define error codes for vsrs functions
#define VSRS_OK		1
#define VSRS_ERROR	-1

#define EOL '\n'
#define EOS '\0'
#define TAB '\t'
#define COMMENT '#'
#define SPACE ' '


using namespace std;