import math
def get_direction(ball_angle: float) -> int:
    """Get direction to navigate robot to face the ball

    Args:
        ball_angle (float): Angle between the ball and the robot

    Returns:
        int: 0 = forward, -1 = right, 1 = left
    """
    if ball_angle >= 345 or ball_angle <= 15:
        return 0
    return -1 if ball_angle < 180 else 1

# (x, y)
class Vector2:
    def __init__(self, x=0, y=0, fromDict=None):
        if fromDict is not None:
            self.x = fromDict['x']
            self.y = fromDict['y']
            return

        self.x = x
        self.y = y
    
    def getMagnitude(self):
        return math.sqrt(self.x * self.x + self.y * self.y)

    def getNormalized(self):
        mag = self.getMagnitude()
        return Vector2(self.x / mag, self.y / mag)

    def isLeftOf(self, p2, inclusive=False):
        return self.x < p2.x or (inclusive and self.x == p2.x)

    def isRightOf(self, p2, inclusive=False):
        return self.x > p2.x or (inclusive and self.x == p2.x)

    def isAbove(self, p2, inclusive=False):
        return self.y > p2.y or (inclusive and self.y == p2.y)

    def isBelow(self, p2, inclusive=False):
        return self.y < p2.y or (inclusive and self.y == p2.y)

    def distFrom(self, p2):
        dx = p2.x - self.x
        dy = p2.y - self.y
        return math.sqrt(dx * dx + dy * dy)

    def isCloseTo(self, p2):
        return math.isclose(self.x, p2.x) and math.isclose(self.y, p2.y)

    def __str__(self):
        return "({0},{1})".format(self.x,self.y)

    def __eq__(self, p2):
        return self.x == p2.x and self.y == p2.y

    def __add__(self, p2):
        return Vector2(self.x + p2.x, self.y + p2.y)

    def __sub__(self, p2):
        return Vector2(self.x - p2.x, self.y - p2.y)

    def __mul__(self, m: float):
        return Vector2(self.x * m, self.y * m)
    
    def __truediv__(self, m: float):
        return Vector2(self.x / m, self.y / m)


ZERO = Vector2(0, 0)
UP = Vector2(0, 1)
RIGHT = Vector2(1, 0)
ONE = Vector2(1, 1)
