from ultralytics import YOLO
import cv2

# Charge le modèle YOLOv8 nano (le plus léger)
model = YOLO('yolov8n.pt')

# Ouvre la webcam
cap = cv2.VideoCapture(0)

print("YOLO démarré — appuie sur 'q' pour quitter")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Détection sur le frame
    results = model(frame, verbose=False)

    # Affiche les résultats avec bounding boxes
    annotated = results[0].plot()

    cv2.imshow('YOLO Detection', annotated)

    # Quitter avec 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
