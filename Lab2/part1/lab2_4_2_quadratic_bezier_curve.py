import cv2
import numpy as np
import math

img = np.zeros((512, 512, 3), np.uint8)
windowName = 'Lab2-4-2'
cv2.namedWindow(windowName)

# Store points for the Bezier curve
control_points = []

def quadratic_bezier_point(p0, p1, p2, t):
    return (1 - t)**2 * p0 + 2 * (1 - t) * t * p1 + t**2 * p2

def draw_bezier_curve(control_points):
    if len(control_points) == 3:
        return 
    
    P0, P1, P2 = control_points[:3]  # Extract control points

    # Draw the Bezier curve
    for t in np.linspace(0, 1, 1000):
        x = int(quadratic_bezier_point(P0[0], P1[0], P2[0], t))
        y = int(quadratic_bezier_point(P0[1], P1[1], P2[1], t))
        cv2.circle(img, (x, y), 1, (0, 255, 0), -1)


def mouse_callback(event, x, y, flags, param):
    global control_points
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        control_points.append((x, y))
        if len(control_points) >= 4:
            draw_bezier_curve(control_points)
            control_points = []

cv2.setMouseCallback(windowName, mouse_callback)

while True:
    cv2.imshow(windowName, img)
    if cv2.waitKey(20) & 0xFF == 27:    # ESC key to close the window.
        break

cv2.destroyAllWindows()
