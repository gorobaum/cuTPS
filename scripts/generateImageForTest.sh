#!/bin/bash

python generateTestImage.py 32 16 2 gridRef32.png gridTar32.png
python generateTestImage.py 64 16 2 gridRef64.png gridTar64.png
python generateTestImage.py 128 16 2 gridRef128.png gridTar128.png
python generateTestImage.py 256 16 2 gridRef256.png gridTar256.png
python generateTestImage.py 512 16 2 gridRef512.png gridTar512.png
python generateTestImage.py 1024 16 2 gridRef1024.png gridTar1024.png
python generateTestImage.py 2048 16 2 gridRef2048.png gridTar2048.png

python generateTestImage.py 32 8 3 grid3DRef32.nii.gz grid3DTar32.nii.gz
python generateTestImage.py 64 8 3 grid3DRef64.nii.gz grid3DTar64.nii.gz
python generateTestImage.py 128 8 3 grid3DRef128.nii.gz grid3DTar128.nii.gz
python generateTestImage.py 256 8 3 grid3DRef256.nii.gz grid3DTar256.nii.gz
