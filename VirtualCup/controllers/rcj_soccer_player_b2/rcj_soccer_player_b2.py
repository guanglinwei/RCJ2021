# rcj_soccer_player controller - ROBOT B1

# Feel free to import built-in libraries
import math

# You can also import scripts that you put into the folder with controller
from rcj_soccer_robot import RCJSoccerRobot, TIME_STEP
from utils import *


class MyRobot(RCJSoccerRobot):


    def setState(self, state):
        self.state = state
        self.angle_was_out = False
        self.exit_count = 0
    
    # Get to ball
    def chase(self, data):
        # Get the position of our robot
        robot_pos = data[self.name]
        # Get the position of the ball
        ball_pos = data['ball']

        # Get angle between the robot and the ball
        # and between the robot and the north
        ball_angle, robot_angle = self.get_angles(ball_pos, robot_pos)

        # Compute the speed for motors
        direction = utils.get_direction(ball_angle)

        # If the robot has the ball right in front of it, go forward,
        # rotate otherwise
        if direction == 0:
            left_speed = -5
            right_speed = -5
        else:
            left_speed = direction * 4
            right_speed = direction * -4

        # Set the speed to motors
        # self.left_motor.setVelocity(left_speed)
        # self.right_motor.setVelocity(right_speed)


    def in_goal_zone(self, tolerance):
        # cPrint(self.position.x, self.allied_goal_x)
        return abs(self.position.x - self.allied_goal_x) < tolerance and self.position.y < .3 and self.position.y > -.3

    # exit state
    def exit_goal_zone(self, data):
        ball = Vector2(fromDict=data['ball'])
        ball_dist = (self.position - ball).getMagnitude()
        if self.exit_count > 25 or ball_dist < 0.5:
            return self.setState("block")
        self.exit_count += 1

        if not self.in_goal_zone(0.1):
            return self.setState("block")
        if self.faceForwards():
            self.angle_was_out = False
            self.setSpeeds(*self.getBestSpeedsTowardsPosition(self.position - RIGHT))
            cPrint(self.left_speed, self.right_speed)
        
    # goalie
    # if the ball is moving towards the goal but there is no enemy close, just block
    def block(self, data):
        self.exit_count = 0
        # return self.setSpeeds(*self.getBestSpeedsTowardsPosition(self.position + ONE))
        # return self.setSpeeds(*self.getBestSpeedsTowardsPosition(self.position + UP + ONE * .4))
        # return self.moveToGoal()
        self.block_counter += 1
        if self.block_counter > 200:
            self.block_counter = 0

            closest = self.get_closest_yellow(data)
            if (self.position - closest).getMagnitude() < .5:
                return
            if self.in_goal_zone(0.1):
                return self.setState("exit")

            

        ball = Vector2(fromDict=data['ball'])
        ball_dist = (self.position - ball).getMagnitude()
        # cPrint(self.position, " | ", ball_dist)
        goal_tolerance = .045 if ball_dist < 0.1 else 0.03
        # cPrint(self.position)
        if not self.in_goal_zone(goal_tolerance):
            # cPrint("not in goal")
            # if self.in_goal_zone(0.08):
                # cPrint("close")
                # return self.setSpeeds(*self.simpleMoveTowards(self.allied_goal_x, ball.y, 2.5, 1))
            return self.moveToGoal(True)

        # get ball if close
        if ball_dist < 0.015 and abs(self.position.y - ball.y) < 0.01:
            # if already facing forwards get the ball
            cPrint("close")
            if self.faceForwards():
                return self.setState("defend")


        #TODO: make sure this is parallel to goal
        
        if not self.radiansWithin(0, 0.4):
            # cPrint("not parallel")
            self.setSpeeds(*self.getBestSpeedsTowardsPosition(self.position + UP))
            return

        # cPrint("aa")

        # close enough to correct x
        if abs(self.position.y - ball.y) < 0.004:
            return
        
        # going too far from goal x
        # if self.position.y < -.3 or self.position.y > .3:
        #     return
        # cPrint(*self.simpleMoveTowards(.635, ball.y, 2))
        self.setSpeeds(*self.simpleMoveTowards(self.allied_goal_x, ball.y, 2))

    # returns whether already facing forwards
    def faceForwards(self):        
        # - 0 +
        #   pi

        if not self.radiansWithin(math.pi / 2, 0.2):
            self.setSpeeds(*self.getBestSpeedsTowardsPosition(self.position + RIGHT))
            return False

        return True

    def radUtil(self, angle, threshold):
        PI = math.pi
        cPrint("test", angle, self.orientation)
        return 2 * PI - abs(self.orientation - angle) % (2 * PI) < threshold or abs(self.orientation - angle) % (2 * PI) < threshold
    
    def radiansWithin(self, angle, threshold):
        angle = (angle + 2 * math.pi) % math.pi
        return self.radUtil(angle, threshold) or self.radUtil((angle + math.pi) if angle < 0 else (angle - math.pi), threshold)
        # a2 = abs(self.orientation)
        # return a1 < threshold or 
        # a = modP(self.orientation, math.pi)
        # if a
        # cPrint("angle", a, "within", angle)
        # return a < threshold or a > math.pi - threshold

    # steal ball if enemy is close and ball is also
    def defend(self, data):
        # get the closest yellow robot
        target = self.get_closest_yellow(data)
        ball = Vector2(fromDict=data['ball'])
        cPrint("def")

        if(ball - self.position).getMagnitude() > 0.15:
            return self.setState("block")

        # if (target - self.position).getMagnitude() < 0:
        #     pass

        # move to ball
        self.directMoveTowards(ball)

    def get_closest_yellow(self, data):
        return Vector2(fromDict=sorted([data['Y1'], data['Y2'], data['Y3']], key=lambda d: (Vector2(fromDict=d) - self.position).getMagnitude())[0])


    def moveToGoal(self, center=False):
        cPrint("move to goal")
        target = Vector2(self.allied_goal_x, 0 if center else clamp(self.position.y, -.3, .3))
        # move towards goal
        angle = modP(self.get_angles_v2(target, self.position, self.orientation)[0] - 180, 360)
         # if the angle was outside of tolerance try to rotate to 0
        if self.angle_was_out:
            self.angle_was_out = self.getDirBothSides(angle, 7) != 0

        self.directMoveTowards(target)

    # move in front of enemy, try to move ball out of the way. 
    def steal(self, data):
        # TODO: if directly in front of target, move forwards to steal.
        # TODO: if backwards, move backwards instead of rotating
        # who to stop
        target = Vector2(fromDict=data['Y3'])
        ball = Vector2(fromDict=data['ball'])

        # cPrint(self.setSpeeds(*self.getBestSpeeds(330, 29, True)))
        # return
        # line from target to ball
        # target_dir = (target['y'] - ball['y']) / (target['x'] - ball['x'])
        ball_dist = ball - target
        target_dir = ball_dist.getNormalized()
        target_pos = target + target_dir * 0.3



        #cPrint(target, ball, target_pos)

        # if the enemy is moving towards us take the ball
        if intersect(target, target + target_dir * 1.7, self.position - RIGHT * 0.03, self.position + RIGHT * 0.03) or intersect(target, target + target_dir * 1.7, self.position - UP * 0.03, self.position + UP * 0.03):
            if ball_dist.getMagnitude() < 0.01:
                # rotate away from own goal
                # TODO: test this
                self.rotate(1 if self.position.x < 0 else -1)
                return
            
            self.driftTowards(ball)
            return

        cPrint(target_pos, self.position, ball, ball_dist.getMagnitude())
        # if very close to ball, move to it
        if ball_dist.getMagnitude() < 0.11:
            return self.attemptMoveTowards(ball + target_dir * 0.1)

        self.attemptMoveTowards(target_pos)                

        # ally_goal_pos = {'x': 0.75, 'min_y': -0.75, 'max_y': 0.75}
        # intersect = target_dir * (ally_goal_pos - ball['x']) + ball['y']

        # if -1 <= intersect <= 1:
        #     pass

        

        if True:
            pass

    def rotate(self, direction):
        d = math.copysign(10, direction)
        self.left_speed = d
        self.right_speed = -d


    def getDirBothSides(self, angle, tolerance=15):
        flipped = angle > 90 and angle < 270
        # cPrint(flipped)
        new_angle = modP(angle + 180, 360) if flipped else angle
        return get_direction(new_angle, tolerance) 

    # given an angle, return whether to move forwards, backwards, or rotate
    # returns (xspeed, yspeed)
    def getBestSpeeds(self, angle, tolerance=15, drift=False):
        d = self.getDirBothSides(angle, tolerance)
        flipped = angle > 90 and angle < 270
        if d == 0 and not self.angle_was_out:
            return (-10, -10) if flipped else (10, 10)

        d = self.getDirBothSides(angle, -1 if self.angle_was_out else tolerance)
        
        self.angle_was_out = True
        f = -1 if flipped else 1
        # rotate or drift
        if drift:
            return ((7.5 + 2.5 * d * f) * f, (7 - 2.5 * d * f) * f)
        return (10 * d, -10 * d)

    def getBestSpeedsTowardsPosition(self, targetPos, tolerance=15):
        angle = modP(self.get_angles_v2(targetPos, self.position, self.orientation)[0] - 180, 360)
        return self.getBestSpeeds(angle, tolerance)

    def driftTowards(self, targetPos):
        angle = modP(self.get_angles_v2(targetPos, self.position, self.orientation)[0] - 180, 360)
        self.setSpeeds(*self.getBestSpeeds(angle, 20, True))
        # direction = get_direction(angle, 15)

        # # go towards ball
        # if direction == 0: 
        #     self.left_speed = 10
        #     self.right_speed = 10
        #     return direction

        # # if further away turn  but still move forwards
        # self.left_speed = 8 + 2 * direction # * distance / f ?
        # self.right_speed = 8 - 2 * direction

        # return direction

    def directMoveTowards(self, targetPos):
        angle = modP(self.get_angles_v2(targetPos, self.position, self.orientation)[0] - 180, 360)
        self.setSpeeds(*self.getBestSpeeds(angle, 20, True))

    def attemptMoveTowards(self, targetPos):
        # targetPos = ONE * -0.5
        # if robot closer to ball, allow angle to be higher
        target_dist = self.position.distFrom(targetPos)
        
        
        if target_dist < 0.0005:
            return 0

        tolerance = max(10, - target_dist * 10 + 25) # placeholder
        # cPrint(tolerance)
        angle = modP(self.get_angles_v2(targetPos, self.position, self.orientation)[0] - 180, 360)
        acceptable_direction = self.getDirBothSides(angle, 80)
        cPrint(angle, self.angle_was_out)

        # if the angle was outside of tolerance try to rotate to 0
        if self.angle_was_out:
            self.angle_was_out = self.getDirBothSides(angle, 7) != 0

        # if turned too far from ball rotate around quickly 
        acceptable_tolerance = 80
        if acceptable_direction != 0:
            self.setSpeeds(*self.getBestSpeedsTowardsPosition(targetPos, acceptable_tolerance))
            return
        # if acceptable_tolerance < angle < 360 - acceptable_tolerance:
        #     # self.left_speed = direction * 10
        #     # self.right_speed = direction * -10
        #     self.setSpeeds(*self.getBestSpeeds(angle, acceptable_tolerance))
        #     return direction

        # cPrint(target_dist, direction)

        self.left_speed, self.right_speed = self.getBestSpeeds(angle, tolerance, target_dist > .9)
        # cPrint(self.left_speed, self.right_speed, target_dist)
        # cPrint("best", self.left_speed, self.right_speed)

        # return direction

    def simpleMoveTowards(self, x, y, r=1, tolerance=4):
        angle = modP(self.get_angles_v2(Vector2(x, y), self.position, self.orientation)[0] - 180, 360)
        d = self.getDirBothSides(angle, tolerance)
        # cPrint("move", angle, d)
        flipped = angle > 90 and angle < 270

        if d == 0:
            return (-10, -10) if flipped else (10, 10)

        f = -1 if flipped else 1

        return (f * ((10 - r) + r * d), f * ((10 - r) - r * d))


    def setSpeeds(self, x, y):
        self.left_speed, self.right_speed = x, y
        return (x, y)

    def run(self):
        self.update = {
            "chase": self.chase,
            "steal": self.steal,
            "block": self.block,
            "defend": self.defend,
            "exit": self.exit_goal_zone
        }

        self.state = "block"
        
        self.left_speed = 0
        self.right_speed = 0

        self.angle_was_out = False

        self.block_counter = 0
        self.block_counter_reset = False

        self.allied_goal_x = 0.658

        while self.robot.step(TIME_STEP) != -1:
            if self.is_new_data():
                data = self.get_new_data()

                self.position = Vector2(fromDict=data[self.name])
                self.orientation = data[self.name]['orientation']

                self.left_speed = 0
                self.right_speed = 0
                cPrint(self.state)
                # self.setSpeeds(*self.getBestSpeeds(270))
                self.update[self.state](data)



                 # Set the speed to motors
                self.left_motor.setVelocity(clamp(self.left_speed, -10, 10))
                self.right_motor.setVelocity(clamp(self.right_speed, -10, 10))
                


my_robot = MyRobot()
my_robot.run()