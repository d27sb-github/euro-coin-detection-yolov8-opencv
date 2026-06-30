import cv2
from ultralytics import YOLO

image = cv2.imread("096.jpg") #učitana slika za detekciju

model = YOLO("runs/detect/coin_training/euro_coins_nano/weights/best.pt")

results = model(image, conf=0.5) #pokretanje modela

results[0].show()
