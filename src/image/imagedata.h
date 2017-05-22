#ifndef TPS_IMAGEDATA_IMAGE_H_
#define TPS_IMAGEDATA_IMAGE_H_

#include <vector>

namespace tps {
  class ImageData {
    public:
      ImageData(std::vector<int> dimension);
      ImageData(short* data, std::vector<int> dimension);
      ~ImageData();

      void addReference();
      void removeReference();
      bool hasZeroReference();
      short* getPixelVector();
      short getPixelAt(int pos);
      void changePixelAt(int pos, short value);

      int getCount() {return count;}

    private:
      int count;
      short* imageData_;
  };
}

#endif
