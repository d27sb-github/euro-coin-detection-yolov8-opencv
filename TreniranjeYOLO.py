from ultralytics import YOLO

def main():
    #učitavanje pretrained COCO modela
    model = YOLO("yolov8n.pt")
    #treniranje po parametrima
    model.train(
        data="dataset.yaml",
        epochs=120,
        imgsz=640,
        batch=16, 
        device=0,   #za GPU
        workers=8,
        patience=30, 
        save=True,   
        project="coin_training",
        name="euro_coins"
    )

#pokretanje ako je ovo glavna skripta (iz CMD-a)
if __name__ == "__main__":
    main()
