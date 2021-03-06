import math
def get_direction(ball_angle: float, tolerance=15) -> int:
    """Get direction to navigate robot to face the ball

    Args:
        ball_angle (float): Angle between the ball and the robot
        tolerance (float): Will return 0 if angle is 0 += tolerance

    Returns:
        int: 0 = forward, -1 = right, 1 = left
    """
    if ball_angle >= 360 - tolerance or ball_angle <= tolerance:
        return 0
    return -1 if ball_angle < 180 else 1

def clamp(val, lower, upper):
    return max(lower, min(val, upper))

def modP(x, m):
    """
    modulo without negatives
    """

    return (x % m + m) % m

# def modN(x, m):
#     """
#     modulo with negatives
#     """
#     return (x % m) if x > 0 else m - (x % m)

# (x, y)
class Vector2:
    """
    A vector with an X and Y coordinate.

    Parameters:
        x (float, default 0): X component
        y (float, default 0): Y component
        fromDict ({'x', 'y'}, default None): A dict object that create a Vector2 from.
    """
    def __init__(self, x=0, y=0, fromDict=None):
        
        if fromDict is not None:
            self.x = fromDict['x']
            self.y = fromDict['y']
            return

        self.x = x
        self.y = y
    
    def getMagnitude(self):
        """
        Get the length of a vector.

        Returns: float
        """
        return math.sqrt(self.x * self.x + self.y * self.y)

    def getNormalized(self):
        """
        Get a vector with the same direction but a magnitude of 1.

        Returns: Vector2
        """
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
        """
        Returns the distance to another point.

        Parameters:
            p2 (Vector2): The other point.

        Returns: float
        """
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
    
    def __iter__(self):
        return iter([self.x, self.y])

    def todict(self):
        return {'x': self.x, 'y': self.y}


ZERO = Vector2(0, 0)
UP = Vector2(0, 1)
RIGHT = Vector2(1, 0)
ONE = Vector2(1, 1)
BLUE_GOAL = Vector2() #TODO
YELLOW_GOAL = Vector2()

# https://stackoverflow.com/questions/3838329/how-can-i-check-if-two-segments-intersect
def ccw(A,B,C):
    return (C.y-A.y) * (B.x-A.x) > (B.y-A.y) * (C.x-A.x)

# Return true if line segments AB and CD intersect
def intersect(A,B,C,D):
    return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)

do_print = False
def cPrint(*args):
    if do_print:
        print(**args)
# class QueuedAction:
#     # func: function to call
#     # delay: call after how many ticks
#     def __init__(self, func, delay, repeat=False):
#         self.count = 0
#         self.delay = delay
#         self.func = func
#         self.repeat = repeat
#         self.next = None
#         self.prev = None

#     def update(self, ticks):
#         if self.delay == -1:
#             return False

#         self.count += ticks
#         if self.count > self.delay:
#             if func is not None:
#                 func()
#             if repeat:
#                 self.count = 0
#             else:
#                 remove_queued_action(self)


# QUEUED_ACTIONS = []
# def add_queued_action(action):
#     QUEUED_ACTIONS.append(action)

# def update_queued():
#     for q in QUEUED_ACTIONS:
#         q.update(1)

# def remove_queued_action(action):
#     QUEUED_ACTIONS.remove(action)
