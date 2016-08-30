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

def deformSinusiodal3D(imagePixels):
    deformedPixels = np.zeros(imagePixels.shape, np.uint8)
    deformedPixels.fill(0)

    for x in range(imagePixels.shape[0]):
        for y in range(imagePixels.shape[1]):
            for z in range(imagePixels.shape[2]):
                newCol = x + 2.0*math.sin(y/32.0) - 2.0*math.cos(z/16.0)
                newRow = y + 8.0*math.sin(x/32.0) - 4.0*math.sin(z/8.0)
                newDepth = z + 2.0*math.sin(x/16.0) - 4.0*math.cos(y/16.0)
                if newCol <= 0 or newCol >= imagePixels.shape[0]:
                    continue
                if newRow <= 0 or newRow >= imagePixels.shape[1]:
                    continue
                if newDepth <= 0 or newDepth >= imagePixels.shape[2]:
                    continue
                newPixel = trilinear(imagePixels, newCol, newRow, newDepth)
                deformedPixels.itemset((x,y,z), newPixel)

    affine = np.diag([1, 1, 1, 1])
    array_img = nibabel.Nifti1Image(deformedPixels, affine)
    return array_img

def generateKeypoints3D(imagePixels, percentage):
    deformedPixels = np.zeros(imagePixels.shape, np.uint8)
    deformedPixels.fill(0)

    for x in range(imagePixels.shape[0]):
        for y in range(imagePixels.shape[1]):
            for z in range(imagePixels.shape[2]):
                newCol = x - 2.0*math.sin(y/32.0) + 2.0*math.cos(z/16.0)
                newRow = y - 8.0*math.sin(x/32.0) + 4.0*math.sin(z/8.0)
                newDepth = z - 2.0*math.sin(x/16.0) + 4.0*math.cos(y/16.0)
                if newCol <= 0 or newCol >= imagePixels.shape[0]:
                    continue
                if newRow <= 0 or newRow >= imagePixels.shape[1]:
                    continue
                if newDepth <= 0 or newDepth >= imagePixels.shape[2]:
                    continue
                deformedPixels.itemset((newCol,newRow,newDepth), 255)

    affine = np.diag([1, 1, 1, 1])
    array_img = nibabel.Nifti1Image(deformedPixels, affine)
    return array_img

def trilinear(imagePixels, x, y, z):
    u = math.floor(x)
    v = math.floor(y)
    w = math.floor(z)

    xd = (x - u)
    yd = (y - v)
    zd = (z - w)

    c00 = getPixel3D(imagePixels, u, v, w)*(1 - xd) + getPixel3D(imagePixels, u+1, v, w)*xd
    c10 = getPixel3D(imagePixels, u, v+1, w)*(1 - xd) + getPixel3D(imagePixels, u+1, v+1, w)*xd
    c01 = getPixel3D(imagePixels, u, v, w+1)*(1 - xd) + getPixel3D(imagePixels, u+1, v, w+1)*xd
    c11 = getPixel3D(imagePixels, u, v+1, w+1)*(1 - xd) + getPixel3D(imagePixels, u+1, v+1, w+1)*xd

    c0 = c00*(1-yd) + c10*yd
    c1 = c01*(1-yd) + c11*yd

    newValue = c0*(1-zd) + c1*zd
    if (newValue < 0):
        newValue = 0

    return newValue

def getPixel3D(pixels, x, y, z):
        h = pixels.shape[0]
        w = pixels.shape[1]
        d = pixels.shape[2]
        if x >= w or x < 0:
            return 0.0
        elif y >= h or y < 0:
            return 0.0
        elif z >= d or z < 0:
            return 0.0
        else:
            return pixels.item(x, y, z)

def generateGridImage3D(size, step):
    shape = (size, size, size)
    array_data = np.full(shape, 255, np.uint8)
    affine = np.diag([1, 1, 1, 1])

    for y in range(step, size, step):
        for x in range (0, size):
            for z in range (0, size):
                array_data.itemset((x, y, z), 0)

    for x in range(step, size, step):
        for y in range(0, size):
            for z in range (0, size):
                array_data.itemset((x, y, z), 0)

    for z in range(step, size, step):
        for x in range(0, size):
            for y in range (0, size):
                array_data.itemset((x, y, z), 0)

    array_img = nibabel.Nifti1Image(array_data, affine)
    return array_img

gridSize = int(sys.argv[1])
step = int(sys.argv[2])
dimension = int(sys.argv[3])
if (dimension == 2):
    staticImage = generateGridImage(gridSize, step)
    movingImage = deformSinusiodal(staticImage)
    cv2.imwrite(sys.argv[4], staticImage)
    cv2.imwrite(sys.argv[5], movingImage)
else:
    staticImage = generateGridImage3D(gridSize, step)
    movingImage = deformSinusiodal3D(staticImage.get_data())
    keypointImage = generateKeypoints3D(staticImage.get_data())
    nibabel.save(staticImage, sys.argv[4])
    nibabel.save(movingImage, sys.argv[5])
    nibabel.save(keypointImage, "keypoint.nii.gz")
