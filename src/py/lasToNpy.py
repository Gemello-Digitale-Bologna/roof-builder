import sys
import laspy
import numpy as np

def las_to_npy(las_path, npy_path):
    try:
        las = laspy.read(las_path)
        point_data = np.stack((las.x, las.y, las.z), axis=0).transpose((1, 0))
        np.save(npy_path, point_data)

        print(f"Data from {las_path} saved in {npy_path}")
    except Exception as e:
        print(f"An error occurred on load or save: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <las_file_path> <npy_file_path>")
        sys.exit(1)

    las_file_path = sys.argv[1]
    npy_file_path = sys.argv[2]

    las_to_npy(las_file_path, npy_file_path)