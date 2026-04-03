# This code allow to controller a turttle in ROS2 read the instruction in readme

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import sys, termios, tty

class KeyboardController(Node):

    def __init__(self):
        super().__init__('keyboard_controller')
        self.publisher = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)

    def get_key(self):
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        tty.setraw(fd)
        key = sys.stdin.read(1)
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return key

    def run(self):
        print("Contrôle : ZQSD (ou WASD), X pour quitter")

        while True:
            key = self.get_key()
            msg = Twist()

            if key == 'z':  # avant
                msg.linear.x = 2.0
            elif key == 's':  # arrière
                msg.linear.x = -2.0
            elif key == 'q':  # gauche
                msg.angular.z = 2.0
            elif key == 'd':  # droite
                msg.angular.z = -2.0
            elif key == 'x':
                break

            self.publisher.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = KeyboardController()
    node.run()
    node.destroy_node()
    rclpy.shutdown()