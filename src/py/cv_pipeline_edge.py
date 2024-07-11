import sys
import cv2 as cv
import numpy as np

#----- DEBUG SHOW
SHOW = False

#----- CUSTOM PARAMETERS
BLUR_SIGMA = 6.5 # 3.5 in cpp

CANNY_THR_L = 20.0
CANNY_THR_H = 80.0

ST_QL = 0.1
ST_MIN_DIST = 10
ST_BLOCK_SIZE = 9

def image_from_np(file_path):
    try:
        np_array = np.load(file_path)
    except FileNotFoundError:
        print(f"File {file_path} non trovato.")
        return

    image = np_array.astype(np.uint8)

    return image

def find_points(image, max_features):
    image[image != 0] = 255
    image = cv.rotate(image, cv.ROTATE_90_COUNTERCLOCKWISE)

    if SHOW:
        cv.imshow('Debug', image)
        cv.waitKey(0)

    ### Gaussian Blur
    k_size = int(np.ceil((3*BLUR_SIGMA))*2 + 1)

    image_gb = cv.GaussianBlur(image, (k_size, k_size), BLUR_SIGMA)

    if SHOW:
        cv.imshow('Debug', image_gb)
        cv.waitKey(0)

    ### Canny
    image_cn = cv.Canny(image_gb, CANNY_THR_L, CANNY_THR_H)

    if SHOW:
        cv.imshow('Debug', image_cn)
        cv.waitKey(0)

    ### Morphology 
    kernel_dil = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]], dtype=np.uint8)

    image_mrp = cv.dilate(image_cn, kernel_dil)

    image_bool = image_mrp.copy()

    image_mrp = cv.morphologyEx(image_mrp, cv.MORPH_CLOSE, cv.getStructuringElement(cv.MORPH_ELLIPSE, (5, 5)), anchor=(-1,-1), iterations = 4)

    contours, _ = cv.findContours(image_mrp, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    maxArea = 0
    maxAreaInd = -1
    for i, contour in enumerate(contours):
        area = cv.contourArea(contour)
        if area > maxArea:
            maxArea = area
            maxAreaInd = i

    if maxAreaInd != -1:
        image_mrp = np.zeros(image_mrp.shape, dtype=np.uint8)
        image_bool = np.zeros(image_mrp.shape, dtype=np.uint8)
        cv.drawContours(image_mrp, contours, maxAreaInd, 255, 1)
        cv.drawContours(image_bool, contours, maxAreaInd, 255, cv.FILLED)

    if SHOW:
        cv.imshow('Debug', image_mrp)
        cv.waitKey(0)
        cv.imshow('Debug', image_bool)
        cv.waitKey(0)

    ### Features 
    points = cv.goodFeaturesToTrack(image_mrp, max_features, ST_QL, ST_MIN_DIST, mask = None, blockSize = ST_BLOCK_SIZE, k = 0.04)

    if SHOW:
        if points is not None:
            maxS = max(image_mrp.shape[0], image_mrp.shape[1])
            cs = (maxS // 300) + 2

            res_img = image_mrp.copy()
            res_img = cv.cvtColor(image_mrp, cv.COLOR_GRAY2RGB)

            for point in points:
                x, y = point.ravel()
                cv.circle(res_img, (int(x), int(y)), cs, (224, 183, 74), -1)  # BGR

            cv.imshow('Debug', res_img)
            cv.waitKey(0)

    return points, image_bool


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python script.py <io_numpy_edges_file.npy> <o_numpy_matrix_file.npy> <max_features>")
    else:
        io_file = sys.argv[1]
        out_mat_file = sys.argv[2]
        max_features = int(sys.argv[3])
        img = image_from_np(io_file)

        points, mat = find_points(img, max_features)
        
        np_points = np.array(points, np.float64)
        np.save(io_file, np_points)

        mat = cv.rotate(mat, cv.ROTATE_90_CLOCKWISE)
        np_mat = np.array(mat, np.float64)
        np.save(out_mat_file, np_mat)
