#! /usr/bin/env python

import cv2
import numpy as np
import sys

from undistort import undistort

total = 0
for f in sys.argv[1:]:
    img = cv2.imread(f,0)
    img = undistort("../share/table_calib_640x480.yml", img)
    img = cv2.resize(img, (320,240))
    #img = cv2.adaptiveThreshold(img,255,cv2.ADAPTIVE_THRESH_MEAN_C,\
    #                            cv2.THRESH_BINARY,15,2)


    #img = cv2.medianBlur(img,5)
    cimg = cv2.cvtColor(img,cv2.COLOR_GRAY2BGR)

    
    circles = cv2.HoughCircles(img,cv2.HOUGH_GRADIENT,1,20,
                            param1=30,param2=20,minRadius=5,maxRadius=20)

    if circles == None:
        #print (f + ": No circles!")
        #cv2.imshow('detected circles',cimg)
        #cv2.waitKey(0)
        pass
    else:
        total += len(circles[0,:])
        #print(f + ": Found %s circles" % len(circles[0,:]))
        circles = np.uint16(np.around(circles))
        for i in circles[0,:]:
            # draw the outer circle
            cv2.circle(cimg,(i[0],i[1]),i[2],(0,255,0),2)
            # draw the center of the circle
            cv2.circle(cimg,(i[0],i[1]),2,(0,0,255),3)

        cv2.imshow('detected circles',cimg)
        cv2.waitKey(0)

cv2.destroyAllWindows()
print("Found in total %s circles" % total)
