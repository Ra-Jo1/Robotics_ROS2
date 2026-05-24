#!/usr/bin/env python3
"""
yolo_node.py
Nœud ROS 2 — détection d'objets en temps réel avec YOLOv8.
- Souscrit à : /camera/image_raw  (sensor_msgs/Image)
- Publie sur  : /detections       (vision_msgs/Detection2DArray)
- Publie sur  : /camera/annotated (sensor_msgs/Image) — image avec bounding boxes
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from vision_msgs.msg import Detection2DArray, Detection2D, ObjectHypothesisWithPose
from cv_bridge import CvBridge
from ultralytics import YOLO


class YoloNode(Node):
    def __init__(self):
        super().__init__('yolo_node')

        # Paramètres configurables sans recompiler
        self.declare_parameter('model_path', 'yolov8n.pt')
        self.declare_parameter('confidence_threshold', 0.5)
        self.declare_parameter('device', 'cpu')

        model_path = self.get_parameter('model_path').value
        self.conf = self.get_parameter('confidence_threshold').value
        device = self.get_parameter('device').value

        # Chargement du modèle YOLO
        self.model = YOLO(model_path)
        self.bridge = CvBridge()

        # Subscriber — reçoit les images de la caméra
        self.sub = self.create_subscription(
            Image,
            '/camera/image_raw',
            self.image_callback,
            10
        )

        # Publisher — envoie les détections structurées
        self.pub_detections = self.create_publisher(
            Detection2DArray,
            '/detections',
            10
        )

        # Publisher — envoie l'image annotée avec bounding boxes
        self.pub_annotated = self.create_publisher(
            Image,
            '/camera/annotated',
            10
        )

        self.get_logger().info(
            f'YoloNode démarré — modèle: {model_path} | '
            f'confiance: {self.conf} | device: {device}'
        )

    def image_callback(self, msg: Image):
        # Convertit le message ROS en image OpenCV
        frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')

        # Inférence YOLO
        results = self.model(frame, conf=self.conf, verbose=False)[0]

        # --- Publie les détections structurées ---
        detection_array = Detection2DArray()
        detection_array.header = msg.header

        for box in results.boxes:
            det = Detection2D()
            det.header = msg.header

            # Coordonnées bounding box
            x1, y1, x2, y2 = box.xyxy[0].tolist()
            det.bbox.center.position.x = (x1 + x2) / 2
            det.bbox.center.position.y = (y1 + y2) / 2
            det.bbox.size_x = float(x2 - x1)
            det.bbox.size_y = float(y2 - y1)

            # Classe et confiance
            hyp = ObjectHypothesisWithPose()
            hyp.hypothesis.class_id = results.names[int(box.cls)]
            hyp.hypothesis.score = float(box.conf)
            det.results.append(hyp)

            detection_array.detections.append(det)

            # Log dans le terminal
            self.get_logger().info(
                f'Détecté : {hyp.hypothesis.class_id} '
                f'({hyp.hypothesis.score:.0%})'
            )

        self.pub_detections.publish(detection_array)

        # --- Publie l'image annotée ---
        annotated = results.plot()
        annotated_msg = self.bridge.cv2_to_imgmsg(annotated, encoding='bgr8')
        annotated_msg.header = msg.header
        self.pub_annotated.publish(annotated_msg)


def main(args=None):
    rclpy.init(args=args)
    node = YoloNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
