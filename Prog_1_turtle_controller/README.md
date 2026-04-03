# Control a Robot with the Keyboard (TurtleSim – ROS 2 Jazzy)

## Description

This project allows you to control a simulated robot in **TurtleSim** using the keyboard with **ROS 2 Jazzy**.
You can move the robot in real-time and observe its movements in the simulation window.

---

## Features

* Move the robot forward, backward, left, and right.
* Keyboard control.
* Compatible with ROS 2 Jazzy.
* Easy to use for learning ROS 2 basics.

---

## Prerequisites

* **ROS 2 Jazzy** installed on your machine.
* **TurtleSim** for ROS 2:

```bash
sudo apt install ros-jazzy-turtlesim
```

* Python 3.x (if the control script is in Python)
* Linux terminal

---

## Installation

1. Clone the repository:

```bash
git clone https://github.com/your-username/controle-turtlesim-ros2.git
cd controle-turtlesim-ros2
```

2. Build the ROS 2 workspace:

```bash
colcon build
source install/setup.bash
```

---

## Launch

1. Start the TurtleSim simulator:

```bash
ros2 run turtlesim turtlesim_node
```

2. Run the keyboard control script:

```bash
ros2 run turtle_controller keyboard_control
```

---

## Usage

* **Movement keys:**

  * `Z` or `↑` : move forward
  * `S` or `↓` : move backward
  * `Q` or `←` : turn left
  * `D` or `→` : turn right

* **Ctrl+C** : stop the program or click the close button

---

## Project Structure

```bash
ros2_ws/
└─ src/
   └─ turtle_controller/
       ├─ resource/
       ├─ test/
       ├─ turtle_controller/  # folder containing Python scripts (e.g., keyboard_control.py)
            ├─ __init__.py
            ├─ keyboard_control.py
       ├─ package.xml
       ├─ setup.cfg
       └─ setup.py
```

---

## Contributions

Contributions are welcome:

* Add features (variable speed, automatic paths, etc.)
* Improve documentation
* Fix bugs

---

## License

Project licensed under MIT.

