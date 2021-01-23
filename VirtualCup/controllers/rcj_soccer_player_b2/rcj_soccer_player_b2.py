# rcj_soccer_player controller - ROBOT B1

# Feel free to import built-in libraries
import math

# You can also import scripts that you put into the folder with controller
from rcj_soccer_robot import RCJSoccerRobot, TIME_STEP
from utils import *


class MyRobot(RCJSoccerRobot):
    
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
        #print(robot_pos["orientation"])

        # If the robot has the ball right in front of it, go forward,
        # rotate otherwise
        if direction == 0:
            left_speed = -5
            right_speed = -5
        else:
            left_speed = direction * 4
            right_speed = direction * -4

        # Set the speed to motors
        #self.left_motor.setVelocity(left_speed)
        #self.right_motor.setVelocity(right_speed)

    # move in front of enemy, try to move ball out of the way. 
    def steal(self, data):
        # TODO: if directly in front of target, move forwards to steal.
        # TODO: if backwards, move backwards instead of rotating
        # who to stop
        target = Vector2(fromDict=data['Y3'])
        ball = Vector2(fromDict=data['ball'])

        # line from target to ball
        # target_dir = (target['y'] - ball['y']) / (target['x'] - ball['x'])
        ball_dist = ball - target
        target_dir = ball_dist.getNormalized()
        target_pos = target + target_dir * 0.3

        #print(target, ball, target_pos)

        self.attemptMoveTowards(target_pos)                

        # ally_goal_pos = {'x': 0.75, 'min_y': -0.75, 'max_y': 0.75}
        # intersect = target_dir * (ally_goal_pos - ball['x']) + ball['y']

        # if -1 <= intersect <= 1:
        #     pass

        

        if True:
            pass

    def attemptMoveTowards(self, targetPos):
        # targetPos = ONE * -0.5
        # if robot closer to ball, allow angle to be higher
        target_dist = self.position.distFrom(targetPos)
        

        if target_dist < 0.002:
            return 0

        tolerance = max(10, - target_dist * 10 + 25) # placeholder
        # print(tolerance)
        angle = modP(self.get_angles_v2(targetPos, self.position, self.orientation)[0] - 180, 360)
        direction = get_direction(angle, tolerance)
        print(angle)
        # if turned too far from ball rotate around quickly
        acceptable_tolerance = 80
        if acceptable_tolerance < angle < 360 - acceptable_tolerance:
            self.left_speed = direction * 10
            self.right_speed = direction * -10
            return direction

        print(target_dist, direction)
        

        # go towards ball
        if direction == 0: 
            self.left_speed = 10
            self.right_speed = 10
            return direction

        # if very close to ball, turn quickly
        if target_dist < 0.3: # placeholder
            self.left_speed = direction * 10
            self.right_speed = direction * -10
            return direction

        # if further away turn  but still move forwards
        self.left_speed = 8 + 2 * direction # * distance / f ?
        self.right_speed = 8 - 2 * direction

        return direction


    def run(self):
        self.update = {
            "chase": self.chase,
            "steal": self.steal
        }

        self.state = "steal"
        
        self.left_speed = 0
        self.right_speed = 0

        while self.robot.step(TIME_STEP) != -1:
            if self.is_new_data():
                data = self.get_new_data()

                self.position = Vector2(fromDict=data[self.name])
                self.orientation = data[self.name]['orientation']

                self.update[self.state](data)

                 # Set the speed to motors
                self.left_motor.setVelocity(clamp(self.left_speed, -10, 10))
                self.right_motor.setVelocity(clamp(self.right_speed, -10, 10))
                


my_robot = MyRobot()
my_robot.run()