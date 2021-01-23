# rcj_soccer_player controller - ROBOT B1

# Feel free to import built-in libraries

# TODO: orientation based isStuckOnWall()

from math import *
from utils import *
import random

# You can also import scripts that you put into the folder with controller
from rcj_soccer_robot import RCJSoccerRobot, TIME_STEP
global isPrinting 
isPrinting = True

class MyRobot(RCJSoccerRobot):
    def run(self):
        global gap
        gap = .1
        global degDirection
        
        
        
        while self.robot.step(TIME_STEP) != -1:
            
            
             
            
            if self.is_new_data():
                data = self.get_new_data()
                degDirection = degrees(data[self.name]["orientation"])
                if degDirection<0:
                    degDirection=180+(180-abs(degDirection))
                myprint(degDirection)

                # Get the position of our robot
                self.robot_pos = data[self.name]
                ball_pos = data['ball']
                myprint(self.robot_pos)
                enemyAngle, friendAngle = self.get_angles(self.enemyGoalPos(), self.robot_pos)
                
                targetPos = self.targetPos(ball_pos,gap)
                
                if targetPos["x"] > 0.9:
                   targetPos["x"]=0.9
                   
                if targetPos["x"] <-0.9:
                   targetPos["x"]=-0.9  
                    
                if targetPos["y"] > 0.6:
                   targetPos["y"]=0.6
                   
                if targetPos["y"] < -0.6:
                   targetPos["y"]=-0.6
                   
                #print(enemyAngle)
                #print("Tpos: ", targetPos)
             
                # Get the position of the ball
                

                # Get angle between the robot and the ball
                # and between the robot and the north
                ball_angle, robot_angle = self.get_angles(ball_pos, self.robot_pos)
                #print('Ball: ' + str(ball_angle))
                #print("Robot B: " + str(robot_angle))

                # Compute the speed for motors
                goalAngle, nothing = self.get_angles(self.enemyGoalPos(), self.robot_pos)
                
                targetPos, isBackward = self.avoid(data,targetPos)
                
                if self.isStuckOnWall(data):
                    self.goBackwards(10,True)
                elif isBackward:
                    self.goBackwards(8)
                    myprint("Going Backwards")
                elif get_distance(self.robot_pos,targetPos) <=0.1:
                    if abs(goalAngle-ball_angle) <= 10:
                    
                        self.goto(self.enemyGoalPos(),self.robot_pos)
                        myprint("Going to Goal")
                    else:
                        direction = get_direction(goalAngle)
                        self.rotate(direction)
                        myprint("Turning to goal")
                
                else:
                    
                    #self.goto(self.targetPos(ball_pos),robot_pos)
                    myprint("Going to target")
                    self.goto(targetPos,self.robot_pos)
                # If the robot has the ball right in front of it, go forward,
                # rotate otherwise
    
    
                 
    def rotate(self, direction):
        
        if direction == 0:
            self.goto(self.enemyGoalPos(), self.robot_pos)
        else:
            self.left_motor.setVelocity(7*direction)
            self.right_motor.setVelocity(-7*direction)
        
        myprint("rotating")
        
        
    
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
            
            
    def targetPos(self, ball_pos: dict, gap):
        x = 0
        y = 0
        if self.enemyGoal() == "B":
            if ball_pos["y"] < 0:
                angle = 360 - self.ballGoalAngle(ball_pos)
                x = ball_pos["x"] - gap*sin(radians(angle))
                y = ball_pos["y"] + gap*cos(radians(angle)) 
                
            elif ball_pos["y"] > 0:
                angle = abs(self.ballGoalAngle(ball_pos))
                x = ball_pos["x"] - gap*sin(radians(angle))
                y = ball_pos["y"] - gap*cos(radians(angle))
                
            else:
                return {"x": ball_pos["x"]-gap,"y": 0}
        else:
            if ball_pos["y"] < 0:
                angle = 360 - self.ballGoalAngle(ball_pos)
                x = ball_pos["x"] + gap*sin(radians(angle))
                y = ball_pos["y"] + gap*cos(radians(angle))
                
            elif ball_pos["y"] > 0:
                angle = abs(self.ballGoalAngle(ball_pos))
                x = ball_pos["x"] + gap*sin(radians(angle))
                y = ball_pos["y"] - gap*cos(radians(angle))
                
            else:
                return {"x": ball_pos["x"]-gap,"y": 0}
        return {"x": x, "y": y, "orientation": 0}
        
        
    def ballGoalAngle(self,ball_pos):
        angle, nothing=self.get_angles(ball_pos, self.enemyGoalPos())
        return angle
        
    def targetRobotAngle(self,targetPos, robot_pos):
        robotPos = {"x": robot_pos["x"], "y": robot_pos["y"], "orientation": 0}
        angle, nothing=self.get_angles(targetPos, robotPos)
        return angle
            
    def goto(self, coords,robot_pos):
        goAngle, nothing = self.get_angles(coords, robot_pos)
        direction = get_direction(goAngle)
        if direction == 0:
            left_speed = -10
            right_speed = -10
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
        
    
    def goBackwards(self, curve, reallyBackward = False):
        if reallyBackward:
            self.left_motor.setVelocity(curve)
            self.right_motor.setVelocity(curve)
            return
        
        rightS = 10
        #leftS = random.choice([10,10-curve])
        leftS = -10
        if leftS == 10:
            rightS = 10-curve
        
        self.left_motor.setVelocity(leftS)
        self.right_motor.setVelocity(rightS)
        
        
        
    def approxAngle(self, angle, angle2, mojo):
        return abs(angle-angle2) <= mojo
           
        
    
    def isStuckOnWall(self,data):
        robotx = data[self.name]["x"]
        roboty = data[self.name]["y"]
        orientation = degrees(data[self.name]["orientation"])
        if orientation<0:
            orientation=180+(180-abs(orientation))
            
        wallR = 0.67
        wallL = -0.67
        wallB = 0.57
        wallT = -0.57
        
        ang = 35
        
        if robotx > wallR and self.approxAngle(orientation,90,ang):  
            myprint("Stuck!")
            return True
            
        elif robotx < wallL and self.approxAngle(orientation,270,ang):
            myprint("Stuck!")
            return True
            
        elif roboty > wallB and self.approxAngle(orientation,0,ang):
            myprint("Stuck!")
            return True
            
        elif roboty < wallT and self.approxAngle(orientation,180,ang):
            myprint("Stuck!")
            return True
            
        return False
        
    
    def avoid(self,data, targetPos):
        newTarget = targetPos
        robot_pos = data[self.name]
        ball_pos = data["ball"]
        angTarget = self.targetRobotAngle(targetPos,robot_pos)
        
        
        closestEnemy = "B2"
        closestDistance = 2
        ballDistance = get_distance(robot_pos,ball_pos)
        
        for robot in data:
            if robot != "ball" and robot != self.name:
                
                dist = get_distance(robot_pos,data[robot])
                if dist < closestDistance:
                    closestDistance = dist
                    closestEnemy = robot
        enemyPos = data[closestEnemy]
        angEnemy,nothing = self.get_angles(enemyPos,robot_pos)        
        
        if ballDistance < closestDistance:
             return targetPos, False
        
        if closestDistance < .15:
            angle, nothing = self.get_angles(enemyPos,robot_pos)
            if (angle > 290 and angle < 360) or (angle < 70 and angle > 0):
                return targetPos, True
            
        
        
        
        
        if abs(angTarget-angEnemy) <= 20:
            
            newGap = .4-closestDistance
            if newGap <0:
                newGap=0
            newTarget = self.targetPos(ball_pos,gap+newGap)
            return newTarget, False
            
        return targetPos, False
            
def myprint(text):
    if isPrinting:
        print(text)            
        
my_robot = MyRobot()
my_robot.run()
