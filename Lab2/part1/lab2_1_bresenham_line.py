import cv2
import numpy as np
import math

img = np.zeros((512, 512, 3), np.uint8)
windowName = 'Lab2-1'
cv2.namedWindow(windowName)

# Store points for the line
control_points = []

def draw_bresenham(control_points):
    if len(control_points) < 2:
        return

    # Extract the two points
    x1, y1 = control_points[-2]
    x2, y2 = control_points[-1]

    # Calculate dx, dy
    dx = abs(x2 - x1)
    dy = abs(y2 - y1)

    # Determine the direction of step (either +1 or -1)
    sx = 1 if x1 < x2 else -1
    sy = 1 if y1 < y2 else -1

    if dx > dy:
        d = 2 * dy - dx
        while x1 != x2:
            img[y1, x1] = (255, 255, 255)  # Color the pixel white
            if d > 0:
                y1 += sy
                d -= 2 * dx
            x1 += sx
            d += 2 * dy
            cv2.imshow(windowName, img)
    else:
        # For steep lines where dy > dx
        d = 2 * dx - dy
        while y1 != y2:
            img[y1, x1] = (255, 255, 255)  # Color the pixel white
            if d > 0:
                x1 += sx
                d -= 2 * dy
            y1 += sy
            d += 2 * dx
            cv2.imshow(windowName, img)

def mouse_callback(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        control_points.append((x, y))
        if len(control_points) >= 2:
            draw_bresenham(control_points)

cv2.setMouseCallback(windowName, mouse_callback)

while True:
    cv2.imshow(windowName, img)
    if cv2.waitKey(20) & 0xFF == 27:
        break

cv2.destroyAllWindows()
