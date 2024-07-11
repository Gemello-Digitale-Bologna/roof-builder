import sys
import cv2 as cv
import numpy as np
from skimage.feature import peak_local_max

SHOW = False

def image_from_np(file_path):
    try:
        np_array = np.load(file_path)
    except FileNotFoundError:
        print(f"File {file_path} non trovato.")
        return

    i = np_array.astype(np.float32)

    return i


def get_image(mat):
    mask = mat > 0.0

    minZ = np.min(mat[mask])
    maxZ = np.max(mat)

    norm_z = np.where(mat != 0, (mat - minZ) / (maxZ - minZ), 0)

    img = (norm_z * 255.0).astype(np.uint8)

    img = cv.rotate(img, cv.ROTATE_90_COUNTERCLOCKWISE)

    return img 

def loc_max(image):
    lm = peak_local_max(image, min_distance=10, threshold_abs=150)

    lm = np.fliplr(lm)

    if SHOW:
        maxS = max(image.shape[0], image.shape[1])
        cs = (maxS // 300) + 2

        res_img = image.copy()
        res_img = cv.cvtColor(image, cv.COLOR_GRAY2RGB)

        for point in lm:
            cv.circle(res_img, (point[0], point[1]), cs, (224, 183, 74), -1)  # BGR

        cv.imshow('Debug', res_img)
        cv.waitKey(0)

    return lm

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <io_numpy_file.npy>")
    else:
        io_file = sys.argv[1]
        im = image_from_np(io_file)

        norm_img = get_image(im)
        if SHOW:
            cv.imshow('Debug', norm_img)
            cv.waitKey(0)

        points = loc_max(norm_img)
        
        np_points = np.array(points, np.float64)
        np.save(io_file, np_points)