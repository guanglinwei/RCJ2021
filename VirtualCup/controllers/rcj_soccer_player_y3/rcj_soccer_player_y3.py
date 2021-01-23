# rcj_soccer_player controller - ROBOT B1

# Feel free to import built-in libraries
from math import *

# You can also import scripts that you put into the folder with controller
from rcj_soccer_robot import RCJSoccerRobot, TIME_STEP
import utils


class MyRobot(RCJSoccerRobot):
    def run(self):
       
        
        while self.robot.step(TIME_STEP) != -1:
            
            if self.is_new_data():
                data = self.get_new_data()
                

                # Get the position of our robot
                robot_pos = data[self.name]
                ball_pos = data['ball']
                #print(self.goalAngle(data))
                enemyAngle, friendAngle = self.get_angles(self.enemyGoalPos(), robot_pos)
                
                targetPos = self.targetPos(ball_pos)
                
                if targetPos["x"] > 0.9:
                   targetPos["x"]=0.9
                   
                if targetPos["x"] <-0.9:
                   targetPos["x"]=-0.9  
                    
                if targetPos["y"] > 0.6:
                   targetPos["y"]=0.6
                   
                if targetPos["y"] < -0.6:
                   targetPos["y"]=-0.6
                   
                #print(enemyAngle)
                # print("Tpos: ", targetPos)
             
                # Get the position of the ball
                

                # Get angle between the robot and the ball
                # and between the robot and the north
                ball_angle, robot_angle = self.get_angles(ball_pos, robot_pos)
                #print('Ball: ' + str(ball_angle))
                #print("Robot B: " + str(robot_angle))

                # Compute the speed for motors
                if (abs(robot_pos["x"] - targetPos["x"]) <.1) and (abs(robot_pos["y"] - targetPos["y"]) <.07): 
                    self.goto(self.enemyGoalPos(),robot_pos)
                
                else:
                    
                    #self.goto(self.targetPos(ball_pos),robot_pos)
                    self.goto(targetPos,robot_pos)
                # If the robot has the ball right in front of it, go forward,
                # rotate otherwise
                  

    def myGoal(self):
        return self.name[0]
        
    def enemyGoal(self):
        if self.name[0]=="B":
            return "Y"
        else:
            return "B"
            
    def enemyGoalPos(self):

        if self.enemyGoal() == "B":        
            return {"x": 0.75, "y": 0, "orientation": 0}
        else:
            return {"x": -0.75, "y": 0, "orientation": 0}
            
    def targetPos(self, ball_pos: dict):
        if self.enemyGoal() == "B":
            if ball_pos["y"] < 0:
                angle = 360 - self.ballGoalAngle(ball_pos)
                x = ball_pos["x"] - .1*sin(radians(angle))
                y = ball_pos["y"] + .1*cos(radians(angle)) 
                return {"x": x, "y": y, "orientation": 0}
                
            elif ball_pos["y"] > 0:
                angle = abs(self.ballGoalAngle(ball_pos))
                x = ball_pos["x"] - .1*sin(radians(angle))
                y = ball_pos["y"] - .1*cos(radians(angle))
                return {"x": x, "y": y, "orientation": 0}
                
            else:
                return {"x": ball_pos["x"]-.1,"y": 0}
        else:
            if ball_pos["y"] < 0:
                angle = 360 - self.ballGoalAngle(ball_pos)
                # print("0",angle)
                # print("Ball0", ball_pos)
                x = ball_pos["x"] + .1*sin(radians(angle))
                y = ball_pos["y"] + .1*cos(radians(angle))
                return {"x": x, "y": y, "orientation": 0}
                
            elif ball_pos["y"] > 0:
                
                angle = abs(self.ballGoalAngle(ball_pos))
                # print("1",angle)
                # print("Ball1", ball_pos)
                x = ball_pos["x"] + .1*sin(radians(angle))
                y = ball_pos["y"] - .1*cos(radians(angle))
                return {"x": x, "y": y, "orientation": 0}
                
            else:
                return {"x": ball_pos["x"]-.1,"y": 0}
            
    def ballGoalAngle(self,ball_pos):
        angle, nothing=self.get_angles(ball_pos, self.enemyGoalPos())
        return angle
            
    def goto(self, coords,robot_pos):
        goAngle, nothing = self.get_angles(coords, robot_pos)
        direction = utils.get_direction(goAngle)
        if direction == 0:
            left_speed = -4
            right_speed = -5
        else:
            if direction == -1:
                left_speed = -10
                right_speed = -2
            else:
                left_speed = -2
                right_speed = -10

        # Set the speed to motors
        self.left_motor.setVelocity(left_speed)
        self.right_motor.setVelocity(right_speed)
        
        
my_robot = MyRobot()
my_robot.run()
