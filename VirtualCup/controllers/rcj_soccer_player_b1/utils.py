from math import *
def get_direction(ball_angle: float) -> int:
    """Get direction to navigate robot to face the ball

    Args:
        ball_angle (float): Angle between the ball and the robot

    Returns:
        int: 0 = forward, -1 = right, 1 = left
    """
    if ball_angle >= 340 or ball_angle <= 20:
        return 0
    return -1 if ball_angle < 180 else 1

def get_distance(dist1: dict, dist2: dict):
    x1 = dist1["x"]
    x2 = dist2["x"]
    y1 = dist1["y"]
    y2 = dist2["y"]

    distance = abs(sqrt((x2-x1)**2+(y2-y1)**2))
    return distance
        
