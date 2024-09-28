import cv2
import numpy as np
import math

img = np.zeros((512, 512, 3), np.uint8)
windowName = 'Lab2-2'
cv2.namedWindow(windowName)

# Store points for the line
control_points = []

def draw_rectangle(control_points):
    
    num_lines = 0  # compute number of lines given the control_points
    for nl in range(num_lines):
        P0, P1 = [] # Extract control points
        # write down you code here.


def mouse_callback(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        control_points.append((x, y))
        if len(control_points) >= 2:
            draw_rectangle(control_points)

cv2.setMouseCallback(windowName, mouse_callback)

while True:
    cv2.imshow(windowName, img)
    if cv2.waitKey(20) & 0xFF == 27:
        break

cv2.destroyAllWindows()
