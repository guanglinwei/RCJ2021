# rcj_soccer_player controller - ROBOT B1

# Feel free to import built-in libraries
import math

# You can also import scripts that you put into the folder with controller
from rcj_soccer_robot import RCJSoccerRobot, TIME_STEP
import utils


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
        print(robot_pos["orientation"])

        # If the robot has the ball right in front of it, go forward,
        # rotate otherwise
        if direction == 0:
            left_speed = -5
            right_speed = -5
        else:
            left_speed = direction * 4
            right_speed = direction * -4

        # Set the speed to motors
        self.left_motor.setVelocity(left_speed)
        self.right_motor.setVelocity(right_speed)

    # move in front of enemy, try to move ball out of the way. 
    def steal(self, data):
        # who to stop
        target = Vector2(fromDict=data['Y1'])
        ball = Vector2(fromDict=data['ball'])

        # line from target to ball
        # target_dir = (target['y'] - ball['y']) / (target['x'] - ball['x'])
        target_dir = target - ball

        # ally_goal_pos = {'x': 0.75, 'min_y': -0.75, 'max_y': 0.75}
        # intersect = target_dir * (ally_goal_pos - ball['x']) + ball['y']

        # if -1 <= intersect <= 1:
        #     pass

        

        if True:
            pass



    def run(self):
        self.update = {
            "chase": self.chase

        }
        self.state = "chase"

        while self.robot.step(TIME_STEP) != -1:
            if self.is_new_data():
                self.update[self.state](self.get_new_data())
                


my_robot = MyRobot()
my_robot.run()