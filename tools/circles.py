import cv2
import numpy as np
import sys

img = cv2.imread(sys.argv[1],0)
img = cv2.resize(img, (320,240))
#img = cv2.medianBlur(img,5)
cimg = cv2.cvtColor(img,cv2.COLOR_GRAY2BGR)

circles = cv2.HoughCircles(img,cv2.HOUGH_GRADIENT,1,20,
                           param1=40,param2=25,minRadius=5,maxRadius=20)

if circles == None:
    print ("No circles!")
    cv2.imshow('detected circles',cimg)
    cv2.waitKey(0)
else:
    print("Found %s circles" % len(circles[0,:]))
    circles = np.uint16(np.around(circles))
    for i in circles[0,:]:
        # draw the outer circle
        cv2.circle(cimg,(i[0],i[1]),i[2],(0,255,0),2)
        # draw the center of the circle
        cv2.circle(cimg,(i[0],i[1]),2,(0,0,255),3)

        cv2.imshow('detected circles',cimg)
        cv2.waitKey(0)

cv2.destroyAllWindows()
