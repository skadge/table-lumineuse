import sys
import os.path
import numpy as np
import cv2
import yaml

if not sys.argv[1].endswith("yml"):
    print("Usage: undistort.py calib.yml images...")
    sys.exit(1)

calib = open(sys.argv[1],'r')
calibdata = yaml.load(calib)

K = np.array(calibdata["camera_matrix"]["data"]).reshape((3,3))
d = np.array(calibdata["distortion_coefficients"]["data"])

print("Camera matrix:")
print(K)
print("Distortion coefficients:")
print(d)

for f in sys.argv[2:]:
    # read one of your images
    img = cv2.imread(f)
    h, w = img.shape[:2]

    # undistort
    newcamera, roi = cv2.getOptimalNewCameraMatrix(K, d, (w,h), 0) 
    newimg = cv2.undistort(img, K, d, None, newcamera)

    name = os.path.basename(f).split(".")[0]
    cv2.imwrite("%s_undistorted.jpg" % name, newimg)

print("Done undistorting %s images." % len(sys.argv[2:]))
