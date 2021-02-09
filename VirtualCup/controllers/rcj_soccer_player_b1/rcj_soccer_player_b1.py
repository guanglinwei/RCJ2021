# rcj_soccer_player controller - ROBOT B1

# Feel free to import built-in libraries

# TODO: orientation based isStuckOnWall()

from math import *
from utils import *
import random
import time
# You can also import scripts that you put into the folder with controller
from rcj_soccer_robot import RCJSoccerRobot, TIME_STEP
global isPrinting
isPrinting = False

class MyRobot(RCJSoccerRobot):
    def run(self):
        global gap
        gap = .1
        global degDirection
        self.ball = 0
        self.stop = False
        
        self.isPrinting = False
        self.reversing = False
        
        self.goalie = False;
        self.attacker = True;
        self.annoyer = False;

        while self.robot.step(TIME_STEP) != -1:




            if self.is_new_data():
                data = self.get_new_data()
                degDirection = degrees(data[self.name]["orientation"])
                if degDirection<0:
                    degDirection=180+(180-abs(degDirection))
                #self.myprint(degDirection)

                # Get the position of our robot
                self.robot_pos = data[self.name]
                
                ball_pos = data['ball']
                
                if self.ball == 0:
                    self.ball = ball_pos
                
                #Posprint
                self.myprint(self.robot_pos)
                
                enemyAngle, friendAngle = self.get_angles(self.enemyGoalPos(), self.robot_pos)
                
                futureBall = self.futureBallPos(ball_pos)
                
                
                targetPos = self.targetPos(futureBall,gap)

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
                if self.attacker:
                    ball_angle, robot_angle = self.get_angles(ball_pos, self.robot_pos)
                    #print('Ball: ' + str(ball_angle))
                    #print("Robot B: " + str(robot_angle))
    
                    # Compute the speed for motors
                    goalAngle, nothing = self.get_angles(self.enemyGoalPos(), self.robot_pos)
    
                    targetPos, isBackward = self.avoid(data,targetPos)
                    
                    if self.isStuckOnWall(data):
                        self.reversing = True
                    elif isBackward:
                        self.goBackwards(8)
                        self.myprint("Going Backwards")
                        
                    elif get_distance(self.robot_pos,targetPos) <=0.1:
                        #if abs(goalAngle-ball_angle) <= 20:
    
                        self.goto(self.enemyGoalPos(),self.robot_pos)
                        self.myprint("Going to Goal")
                        #else:
                            #direction = get_direction(goalAngle)
                            #self.rotate(direction)
                            #self.myprint("Turning to goal")
    
                    else:
    
                        #self.goto(self.targetPos(ball_pos),robot_pos)
                        self.myprint("Going to target")
                        self.goto(targetPos,self.robot_pos)
                    # If the robot has the ball right in front of it, go forward,
                    # rotate otherwise
                    
                    self.ball = ball_pos
                    
                    if self.reversing:
                        self.goBackwards(10,True)
                        time.sleep(2)
                        self.reversing = False
                        
                elif self.goalie:
                    targetPos = self.goalieTarget(ball_pos)
                    self.gotoGoalie(targetPos, self.robot_pos)
                
                elif self.annoyer:
                    self.annoy(data)
                
                
                
                
                if self.stop:
                    self.left_motor.setVelocity(0)
                    self.right_motor.setVelocity(0)

    def rotate(self, direction):

        if direction == 0:
            self.goto(self.enemyGoalPos(), self.robot_pos)
        elif direction == 1:
            self.left_motor.setVelocity(-5*direction)
            self.right_motor.setVelocity(-10*direction)
        else:
            self.left_motor.setVelocity(10*direction)
            self.right_motor.setVelocity(5*direction)

        self.myprint("rotating")



    def myGoal(self):
        return self.name[0]
    
    def enemyGoal(self):
        if self.name[0] == "B":
            return "Y"
        else:
            return "B"
    
    
    
    def enemyGoalPos(self):

        if self.enemyGoal() == "B":
            return {"x": 0.75, "y": 0, "orientation": 0}
        else:
            return {"x": -0.75, "y": 0, "orientation": 0}


    def futureBallPos(self, ball_pos: dict):
             
        k = 10
        #New - old
        dx = ball_pos["x"] - self.ball["x"]
        dy = ball_pos["x"] - self.ball["x"]
        

        futurePos = {"x": ball_pos["x"] + k*dx, "y": ball_pos["y"] + k*dy}
        
        return futurePos
    
    
    
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
                right_speed = -4
            else:
                left_speed = -4
                right_speed = -10

        # Set the speed to motors
        self.left_motor.setVelocity(left_speed)
        self.right_motor.setVelocity(right_speed)

    def gotoGoalie(self, coords, robot_pos):
        goAngle, nothing = self.get_angles(coords, robot_pos)
        direction = get_direction(goAngle)
        
            
        if get_distance(coords, robot_pos) > .11:
            if direction == 0:
                left_speed = -10
                right_speed = -10
            else:
                if direction == -1:
                    left_speed = -10
                    right_speed = 5
                else:
                    left_speed = 5
                    right_speed = -10

            # Set the speed to motors
            self.left_motor.setVelocity(left_speed)
            self.right_motor.setVelocity(right_speed)
        else:
            self.left_motor.setVelocity(0)
            self.right_motor.setVelocity(0)
    
    def goBackwards(self, curve, reallyBackward = False):
        if reallyBackward:
            self.left_motor.setVelocity(curve)
            self.right_motor.setVelocity(curve)
            return

        rightS = 9
        #leftS = random.choice([10,10-curve])
        leftS = 3
        if leftS == 10:
            rightS = 10-curve

        self.left_motor.setVelocity(leftS)
        self.right_motor.setVelocity(rightS)



    def approxAngle(self, angle, angle2, mojo):
    
        ang1 = abs(angle-angle2)
        ang2 = 360 - ang1
        
        if ang1 > ang2:
            realang = ang2
        else:
            realang = ang1
    
        return realang <= mojo



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
            self.myprint("Stuck!")
            return True

        elif robotx < wallL and self.approxAngle(orientation,270,ang):
            self.myprint("Stuck!")
            return True

        elif roboty > wallB and self.approxAngle(orientation,0,ang):
            self.myprint("Stuck!")
            return True

        elif roboty < wallT and self.approxAngle(orientation,180,ang):
            self.myprint("Stuck!")
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


        #Stuck on robot
        if closestDistance < .10:
            angle, nothing = self.get_angles(enemyPos,robot_pos)
            if self.approxAngle(angle,0,70):
                return targetPos, True





        if abs(angTarget-angEnemy) <= 20:

            newGap = .2-closestDistance
            if newGap <0:
                newGap=0
            newTarget = self.targetPos(ball_pos,gap+newGap)
            return newTarget, False

        return targetPos, False
        
    def annoy(self, data):
        
        robot_pos = data[self.name]
        ball_pos = data["ball"]
        
        
        
        farthestEnemy = self.enemyGoal() + "1"
        farthestDistance = 0
        ballDistance = get_distance(robot_pos,ball_pos)

        for robot in data:
            if robot != "ball" and robot != self.name and robot[0] == self.enemyGoal():

                dist = get_distance(ball_pos,data[robot])
                if dist > farthestDistance:
                    farthestDistance = dist
                    farthestEnemy = robot
        enemyPos = data[farthestEnemy]
        angEnemy,nothing = self.get_angles(enemyPos,robot_pos)
        
        self.goto(enemyPos, robot_pos)
    
    def goalieTarget(self, ball_pos):
        ball_pos["orientation"] = 0
        goaltol = 0.68
        targtol = .4
        
        angle, nothing = self.get_angles(self.ball, ball_pos)
        if angle < 0:
            angle = 270 + (90 - abs(angle))
         
        if self.myGoal() == "Y":
        
            goalx = -goaltol
        else:
            goalx = goaltol
        
        #If goalie far away from gaol
        targx = goalx
        if abs(self.robot_pos["x"] - targx) > .1:
            
            targy = 0
        
        #If ball is far away
        elif get_distance(ball_pos, self.robot_pos) > 0.8:
            
            if self.myGoal() == "Y":
                targx += .4
            else:
                targx -= .4
            
            targy = ball_pos["y"]

        #Normal case
        else:
        
            distToBall = abs(goalx - ball_pos["x"])
            targy = ball_pos["y"]
            
            
            if angle > 350 or angle < 10 or (angle < 190 and angle > 170):
                targy = ball_pos["y"]
                
                self.myprint("Dumb")
            else:
                y = distToBall * tan(angle)
                targy = ball_pos["y"] + y
                
                self.myprint("Intelligent Goalie")
        
        if targy > targtol:
            targy = targtol
        elif targy < -targtol:
            targy = -targtol
        target = {"x": targx, "y": targy, "orientation": 0}
    

        self.myprint(target)
        return target
    
           

    def myprint(self, text):
        if self.isPrinting:
            print(self.name, ": ", text)

my_robot = MyRobot()
my_robot.run()
