import cv2
import numpy as np
import math

img = np.zeros((512, 512, 3), np.uint8)
windowName = 'Lab2-2'
cv2.namedWindow(windowName)

# Store points for the line
control_points = []

def draw_rectangle(control_points):
    if len(control_points) < 2:
        return

    # Extract the two points
    x1, y1 = control_points[-2]
    x2, y2 = control_points[-1]
    x = min(x1, x2)
    y = min(y1, y2)
    width = abs(x2 - x1)
    height = abs(y2 - y1)
    cv2.line(img, (x, y), (x + width, y), (255, 255, 255))
    cv2.line(img, (x, y), (x, y + height), (255, 255, 255))
    cv2.line(img, (x + width, y), (x + width, y + height), (255, 255, 255))
    cv2.line(img, (x, y + height), (x + width, y + height), (255, 255, 255))
    cv2.imshow(windowName, img)

    



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
