import sys
import cv2
import math
import scipy
import numpy as np

def deformSinusiodal(imagePixels):
    deformedPixels = np.zeros(imagePixels.shape, np.uint8)
    deformedPixels.fill(0)

    for col in range(imagePixels.shape[1]):
        for row in range(imagePixels.shape[0]):
            newCol = col + 2.0*math.sin(row/32.0)
            newRow = row + 8.0*math.sin(col/32.0)
            if newCol <= 0 or newCol >= imagePixels.shape[1]:
                continue
            if newRow <= 0 or newRow >= imagePixels.shape[0]:
                continue
            newPixel = bilinear(imagePixels, newCol, newRow)
            deformedPixels.itemset((row,col), newPixel)
    return deformedPixels

def bilinear(imagePixels, col, row):
    u = math.floor(col)
    v = math.floor(row)
    interpolation = (u+1-col)*(v+1-row)*getPixel(imagePixels,u,v) + (col-u)*(v+1-row)*getPixel(imagePixels,u+1,v) + (u+1-col)*(row-v)*getPixel(imagePixels,u,v+1) + (col-u)*(row-v)*getPixel(imagePixels,u+1,v+1)
    return interpolation

def getPixel(pixels, col, row):
    # print width, height, x, y
    h = pixels.shape[0]
    w = pixels.shape[1]
    if col >= w or col < 0:
        return 0.0
    elif row >= h or row < 0:
        return 0.0
    else:
        return pixels.item(row, col)

def generateGridImage(size, step):
    shape = (size, size)
    image = np.zeros((size,size), np.uint8)
    image.fill(255)

    for row in range(step, size, step):
        for col in range (0, size):
            image.itemset((row, col), 0)

    for col in range(step, size, step):
        for row in range(0, size):
            image.itemset((row, col), 0)

    return image


gridSize = int(sys.argv[1])
step = int(sys.argv[2])
staticImage = generateGridImage(gridSize, step)
movingImage = deformSinusiodal(staticImage)
cv2.imwrite(sys.argv[3], staticImage)
cv2.imwrite(sys.argv[4], movingImage)
