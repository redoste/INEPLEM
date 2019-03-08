#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <FreeImageLite.h>

#include "images.h"

/* loadImageToFrameBuffer: Charge une image dans le frameBuffer
 * std::string filename: Chemin du fichier
 * ServiceCore *service: Pointeur vers ServiceCore
 * retourne un std::string: Message à retourner à l'UI
 */
std::string loadImageToFrameBuffer(std::string filename, ServiceCore *service){
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str());
	// On détecte le format du fichier et traite les GIF différament (animation)
	if(format == FIF_GIF){
		return loadImageFromGif(filename, service);
	}
	else if(format == FIF_PNG){
		return loadImageFromPng(filename, service, format);
	}
	else{
		return "[loadImageToFrameBuffer] Only GIF and PNG files are supported\n[loadImageToFrameBuffer] Sorry JPG fans :'(";
	}
}

/* loadImageFromGif: Charge une image dans le frameBuffer depuis un gif
 * std::string filename: Chemin du fichier
 * ServiceCore *service: Pointeur vers ServiceCore
 * retourne un std::string: Message à retourner à l'UI
 */
std::string loadImageFromGif(std::string filename, ServiceCore *service){
	FIMULTIBITMAP *gifPages = FreeImage_OpenMultiBitmap(FIF_GIF, filename.c_str(), FALSE, TRUE, FALSE, GIF_DEFAULT);
	if(!gifPages){
		return "[loadImageFromGif] Unable to open gif";
	}
	uint32_t pageCount = FreeImage_GetPageCount(gifPages);

	// La page 0 stocke les veritables dimensions de l'image
	FITAG *tagLogicalHeight, *tagLogicalWidth;
	FIBITMAP *page0 = FreeImage_LockPage(gifPages, 0);
	if(!page0){
		FreeImage_CloseMultiBitmap(gifPages);
		return "[loadImageFromGif] Unable to open gif page 0";
	}
	if(!FreeImage_GetMetadata(FIMD_ANIMATION, page0, "LogicalWidth", &tagLogicalWidth)){
		FreeImage_UnlockPage(gifPages, page0, FALSE);
		FreeImage_CloseMultiBitmap(gifPages);
		return "[loadImageFromGif] Unable to get gif LogicalWidth";
	}
	if(!FreeImage_GetMetadata(FIMD_ANIMATION, page0, "LogicalHeight", &tagLogicalHeight)){
		FreeImage_UnlockPage(gifPages, page0, FALSE);
		FreeImage_CloseMultiBitmap(gifPages);
		return "[loadImageFromGif] Unable to get gif LogicalHeight";
	}
	uint16_t logicalHeight = *((uint16_t*) FreeImage_GetTagValue(tagLogicalHeight));
	uint16_t logicalWidth = *((uint16_t*) FreeImage_GetTagValue(tagLogicalWidth));
	FreeImage_UnlockPage(gifPages, page0, FALSE);


	uint16_t screenX = service->getFrameBufferX();
	uint16_t screenY = service->getFrameBufferY();

	if(screenX < logicalWidth || screenY < logicalHeight){
		FreeImage_CloseMultiBitmap(gifPages);
		return "[loadImageFromGif] For stability reason, you can't load image bigger than your screen.";
	}

	service->setFrameXY(logicalWidth, logicalHeight);
	uint32_t frameDelay = 0;
	std::vector<uint8_t> frameBuffer = std::vector<uint8_t>(logicalWidth * logicalHeight * 3);
	for(uint16_t pageId = 0; pageId < pageCount; pageId++){
		FIBITMAP *currentPage = FreeImage_LockPage(gifPages, pageId);
		if(!currentPage){
			FreeImage_CloseMultiBitmap(gifPages);
			return "[loadImageFromGif] Unable to open gif page " + std::to_string(pageId);
		}

		// Taille de la frame qui ne correspond qu'a la partie qui change depuis la précédante
		uint16_t frameHeight = FreeImage_GetHeight(currentPage);
		uint16_t frameWidth = FreeImage_GetWidth(currentPage);

		FITAG *tagTmp;
		// Position de la frame dans le framebuffer
		if(!FreeImage_GetMetadata(FIMD_ANIMATION, currentPage, "FrameTop", &tagTmp)){
			FreeImage_UnlockPage(gifPages, currentPage, FALSE);
			FreeImage_CloseMultiBitmap(gifPages);
			return "[loadImageFromGif] Unable to open gif page " + std::to_string(pageId) + " metadata FrameTop";
		}
		uint16_t frameTop = *((uint16_t*) FreeImage_GetTagValue(tagTmp));
		if(!FreeImage_GetMetadata(FIMD_ANIMATION, currentPage, "FrameLeft", &tagTmp)){
			FreeImage_UnlockPage(gifPages, currentPage, FALSE);
			FreeImage_CloseMultiBitmap(gifPages);
			return "[loadImageFromGif] Unable to open gif page " + std::to_string(pageId) + " metadata FrameLeft";
		}
		uint16_t frameLeft = *((uint16_t*) FreeImage_GetTagValue(tagTmp));
		// Temps en ms de la frame
		if(!FreeImage_GetMetadata(FIMD_ANIMATION, currentPage, "FrameTime", &tagTmp)){
			FreeImage_UnlockPage(gifPages, currentPage, FALSE);
			FreeImage_CloseMultiBitmap(gifPages);
			return "[loadImageFromGif] Unable to open gif page " + std::to_string(pageId) + " metadata FrameTime";
		}
		frameDelay = *((uint32_t*) FreeImage_GetTagValue(tagTmp));

		// Couleur de la palette qui correspond au transparent
		uint8_t transparentIndex = FreeImage_GetTransparentIndex(currentPage);

		RGBQUAD *palette = FreeImage_GetPalette(currentPage);
		if(!palette){
			FreeImage_UnlockPage(gifPages, currentPage, FALSE);
			FreeImage_CloseMultiBitmap(gifPages);
			return "[loadImageFromGif] Unable to open gif palette page " + std::to_string(pageId);
		}

		for(uint16_t hi = 0; hi < frameHeight; hi++){
			for(uint16_t wi = 0; wi < frameWidth; wi++){
				// FreeImage stocke les images a l'envers verticalement
				// realFrameHi correspond au réel coordonée Y du pixel
				uint16_t realFrameHi = frameHeight - hi;
				// relaLogicalHeigh et realLogicalWidth correspond aux coordonées du pixel en prenant en compte frameTop et frameLeft
				uint16_t realLogicalHeight = frameTop + realFrameHi;
				uint16_t realLogicalWidth = frameLeft + wi;
				uint8_t color = 0;
				uint32_t passedLine = (realLogicalHeight - 1) * logicalWidth * 3;
				FreeImage_GetPixelIndex(currentPage, wi, hi, &color);
				if(color != transparentIndex){
					if(passedLine + (realLogicalWidth * 3) + 2 > frameBuffer.size()){
						// Protection de dépassement du vector, sinon corruption de la heap qui cause un SegFault au FreeImage_UnlockPage
						std::cout << "[loadImageFromGif] Segfault protection" << std::endl;
					}
					else{
						frameBuffer[passedLine + (realLogicalWidth * 3) + 0] = palette[color].rgbRed;
						frameBuffer[passedLine + (realLogicalWidth * 3) + 1] = palette[color].rgbGreen;
						frameBuffer[passedLine + (realLogicalWidth * 3) + 2] = palette[color].rgbBlue;
					}
				}
			}
		}
		// On resizeImage au dimention de l'écran avant de pusher
		service->pushFrame(resizeImage(frameBuffer, logicalWidth, logicalHeight, screenX, screenY));
		FreeImage_UnlockPage(gifPages, currentPage, FALSE);
	}
	service->setFramesDelay(frameDelay);
	FreeImage_CloseMultiBitmap(gifPages);
	return "[loadImageFromGif] New Frames Loaded";
}

/* resizeImage: Redimentionne une image
 * std::vector<uint8_t> source: Vector contenant une frameBuffer source
 * uint16_t sourceW et uin16_t sourceH: Dimensions de l'image source
 * uint16_t dstW et uint16_t dstH: Dimensions de l'image finale
 * retourne un std::vector<uint8_t>: Image finale
 */
std::vector<uint8_t> resizeImage(std::vector<uint8_t> source, uint16_t sourceW, uint16_t sourceH, uint16_t dstW, uint16_t dstH){
	std::vector<uint8_t> dst = std::vector<uint8_t>(dstH * dstW * 3);
	for(uint16_t y = 0; y < sourceH; y++){
		for(uint16_t x = 0; x < sourceW; x++){
			uint32_t passedLineSrc = (y - 1) * sourceW * 3;
			uint32_t passedLineDst = (y - 1) * dstW * 3;
			uint32_t sourceIndex = passedLineSrc + (x * 3);
			uint32_t dstIndex = passedLineDst + (x * 3);
			if(dstIndex + 2 > dst.size()){
				// Le message est produit très souvant mais ne dégrade pas l'image, le message n'est donc pas affiché
				//std::cout << "[resizeImage] Segfault protection dstSize:" << std::to_string(dst.size()) << std::endl;
			}
			else{
				dst[dstIndex + 0] = source[sourceIndex + 0];
				dst[dstIndex + 1] = source[sourceIndex + 1];
				dst[dstIndex + 2] = source[sourceIndex + 2];
			}
		}
	}
	return dst;
}

/* loadImageFromPng: Charge une image dans le frameBuffer depuis un png
 * std::string filename: Chemin du fichier
 * ServiceCore *service: Pointeur vers ServiceCore
 * FREE_IMAGE_FORMAT format: Format de l'image (FIF_PNG normalement)
 * retourne un std::string: Message à retourner à l'UI
 */
std::string loadImageFromPng(std::string filename, ServiceCore *service, FREE_IMAGE_FORMAT format){
	FIBITMAP *image = FreeImage_Load(format, filename.c_str());
	if(!image){
		return "[loadImageFromPng] Unable to open image";
	}
	uint16_t imageHeight = FreeImage_GetHeight(image);
	uint16_t imageWidth = FreeImage_GetWidth(image);
	uint16_t screenX = service->getFrameBufferX();
	uint16_t screenY = service->getFrameBufferY();
	if(screenX < imageWidth || screenY < imageHeight){
		FreeImage_Unload(image);
		return "[loadImageFromPng] For stability reason, you can't load image bigger than your screen.";
	}
	std::vector<uint8_t> frameBuffer = std::vector<uint8_t>(imageWidth * imageHeight * 3);
	for(uint16_t hi = 0; hi < imageHeight; hi++){
		for(uint16_t wi = 0; wi < imageWidth; wi++){
			// FreeImage stocke les images a l'envers verticalement
			// realImageHi correspond au réel coordonée Y du pixel
			uint16_t realImageHi = imageHeight - hi;
			uint32_t passedLine = (realImageHi - 1) * imageWidth * 3;
			uint32_t index = passedLine + (wi * 3);
			if(index + 2 > frameBuffer.size()){
				// Protection car en cas de dépassement sur le vector
				// Corruption de la heap et FreeImage_Unload produira un segfault
				std::cout << "[loadImageFromPng] Segfault protection" << std::endl;
			}
			else{
				RGBQUAD color;
				FreeImage_GetPixelColor(image, wi, hi, &color);
				frameBuffer[index + 0] = color.rgbRed;
				frameBuffer[index + 1] = color.rgbGreen;
				frameBuffer[index + 2] = color.rgbBlue;
			}
		}
	}
	service->setFrameXY(imageWidth, imageHeight);
	service->pushFrame(resizeImage(frameBuffer, imageWidth, imageHeight, screenX, screenY));
	service->setFramesDelay(1000);
	FreeImage_Unload(image);
	return "[loadImageFromPng] New image loaded successfully";
}