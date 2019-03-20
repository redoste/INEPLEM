// Copyright: 2019 redoste
/*
*	This file is part of INEPLEM.
*
*	INEPLEM is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	INEPLEM is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with INEPLEM.  If not, see <https://www.gnu.org/licenses/>.
*/
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