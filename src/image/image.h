#ifndef TPS_IMAGE_IMAGE_H_
#define TPS_IMAGE_IMAGE_H_

#include <vector>
#include <string>
#include <cmath>

namespace tps {

class Image {
public:
	Image() {};
	Image(short* matImage, std::vector<int> dimensions) :
		dimensions_(dimensions) {
			image = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));
			for (int i = 0; i < dimensions[0]*dimensions[1]*dimensions[2]; i++)
				image[i] = matImage[i];
	}
	Image(std::vector<int> dimensions) :
		dimensions_(dimensions) {
			image = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));
			for (int i = 0; i < dimensions[0]*dimensions[1]*dimensions[2]; i++)
				image[i] = 0;
	}
    bool isTwoDimensional();
    short* getPixelVector();
    float* getFloatPixelVector();
	std::vector<short> getMinMax();
    void setPixelVector(short* vector);
	short getPixelAt(int x, int y, int z);
	short NNInterpolation(float x, float y, float z);
    void changePixelAt(int x, int y, int z, short value);
    short trilinearInterpolation(float x, float y, float z);
    std::vector<int> getDimensions() { return dimensions_; };

private:
	short* image;
	std::vector<int> dimensions_;
	int getNearestInteger(float number) {
		if ((number - std::floor(number)) <= 0.5) return std::floor(number);
		return std::floor(number) + 1.0;
	}
};

} // namespace

#endif
