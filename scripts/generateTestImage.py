import sys
import cv2
import math
import scipy
import nibabel
import numpy as np

def deformSinusiodal(imagePixels):
    deformedPixels = np.zeros(imagePixels.shape, np.uint8)
    deformedPixels.fill(0)

    for x in range(imagePixels.shape[1]):
        for y in range(imagePixels.shape[0]):
            newCol = x + 2.0*math.sin(y/32.0)
            newRow = y + 8.0*math.sin(x/32.0)
            if newCol <= 0 or newCol >= imagePixels.shape[1]:
                continue
            if newRow <= 0 or newRow >= imagePixels.shape[0]:
                continue
            newPixel = bilinear(imagePixels, newCol, newRow)
            deformedPixels.itemset((y,x), newPixel)
    return deformedPixels

def bilinear(imagePixels, x, y):
    u = math.floor(x)
    v = math.floor(y)
    interpolation = (u+1-x)*(v+1-y)*getPixel(imagePixels,u,v) + (x-u)*(v+1-y)*getPixel(imagePixels,u+1,v) + (u+1-x)*(y-v)*getPixel(imagePixels,u,v+1) + (x-u)*(y-v)*getPixel(imagePixels,u+1,v+1)
    return interpolation

def getPixel(pixels, x, y):
    # print width, height, x, y
    h = pixels.shape[0]
    w = pixels.shape[1]
    if x >= w or x < 0:
        return 0.0
    elif y >= h or y < 0:
        return 0.0
    else:
        return pixels.item(y, x)

def generateGridImage(size, step):
    shape = (size, size)
    image = np.zeros((size,size), np.uint8)
    image.fill(255)

    for y in range(step, size, step):
        for x in range (0, size):
            image.itemset((y, x), 0)

    for x in range(step, size, step):
        for y in range(0, size):
            image.itemset((y, x), 0)

    return image


gridSize = int(sys.argv[1])
step = int(sys.argv[2])
staticImage = generateGridImage(gridSize, step)
movingImage = deformSinusiodal(staticImage)
cv2.imwrite(sys.argv[3], staticImage)
cv2.imwrite(sys.argv[4], movingImage)
