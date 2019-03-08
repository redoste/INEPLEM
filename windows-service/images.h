#ifndef IMAGES_H
#define IMAGES_H

#include <FreeImageLite.h>
#include <string>
#include "serviceCore.h"

std::string loadImageToFrameBuffer(std::string filename, ServiceCore *service);
std::string loadImageFromGif(std::string filename, ServiceCore *service);
std::vector<uint8_t> resizeImage(std::vector<uint8_t> source, uint16_t sourceW, uint16_t sourceH, uint16_t dstW, uint16_t dstH);
std::string loadImageFromPng(std::string filename, ServiceCore *service, FREE_IMAGE_FORMAT format);

#endif