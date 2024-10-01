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
    center = control_points[-1]
    x = center[0]
    y = center[1]
    radius1 = int(input("Enter the radius1: "))
    radius2 = int(input("Enter the radius2: "))
    num_lines = 100
    for i in range(num_lines):
        angle1 = i * 2 * math.pi / num_lines
        angle2 = (i + 1) * 2 * math.pi / num_lines
        a1 = x+radius1*math.cos(angle1)
        b1 = y+radius2*math.sin(angle1)
        a2 = x+radius1*math.cos(angle2)
        b2 = y+radius2*math.sin(angle2)
        cv2.line(img, (int(a1), int(b1)), (int(a2), int(b2)), (255, 255, 255))        
        




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
