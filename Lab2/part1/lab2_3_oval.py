import cv2
import numpy as np
import math

img = np.zeros((512, 512, 3), np.uint8)
windowName = 'Lab2-3'
cv2.namedWindow(windowName)

# Store points
control_points = []
radius1 = 30
radius2 = 60
num_lines = 100

def draw_oval(control_points):
    
    num_ovals = 0  # number of ovals given the control_points
    for no in range(num_ovals):
        P0 = [] # Extract control points
        # write down your code here.


def mouse_callback(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        control_points.append((x, y))
        draw_oval(control_points)

cv2.setMouseCallback(windowName, mouse_callback)

while True:
    cv2.imshow(windowName, img)
    if cv2.waitKey(20) & 0xFF == 27:
        break

cv2.destroyAllWindows()
