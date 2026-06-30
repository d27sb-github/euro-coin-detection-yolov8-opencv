import os #rukovanje datotekama
import cv2 #opencv
import albumentations as A #knjižica za augmentacije (+ alias)
from tqdm import tqdm #progress bar

#direktoriji koji se čitaju
img_dir = "dataset/images"
lbl_dir = "dataset/labels"
#direktoriji u koje se sprema
img_out = "dataset_aug/images"
lbl_out = "dataset_aug/labels"

os.makedirs(img_out, exist_ok=True)
os.makedirs(lbl_out, exist_ok=True)

#augmentacije po slici
aug_num = 10

#pipeline augmentacije
transform = A.Compose(
    [
    A.Rotate(limit=360, border_mode=cv2.BORDER_CONSTANT, p=0.9), #nasumična rotacija
    A.RandomBrightnessContrast(p=0.6), #promjena osvjetljenja
    A.GaussNoise(p=0.3), #dodavanje šuma
    A.MotionBlur(blur_limit=5, p=0.2), #dodavanje zamućenja
    A.Perspective(scale=(0.05, 0.1), p=0.4), #geometrijska distorzija
    A.HorizontalFlip(p=0.5), #horizontlano zrcaljenje slike
    A.RandomShadow(p=0.3), #staranje lažnih sjena na slici
    A.Resize(720, 960) #zadržavanje dimenzija
    ],
    bbox_params=A.BboxParams(format='yolo', label_fields=['class_labels'])
)

#čitanje YOLO vrijednosti .txt label datoteka
def load_yolo_labels(label_path):
    bboxes = [] #buffer za bbox vrijednosti
    class_labels = [] #buffer za vrijednost klase

    if not os.path.exists(label_path):
        return bboxes, class_labels

    with open(label_path, "r") as f:
        for line in f.readlines(): #čitanje svake linije
            line = line.strip()
            #preskakanje prazne linije
            if line == "":
                print("Prazna linija")
                continue

            parts = line.split()
            #preskakanje "neispravne" linije
            if len(parts) != 5: #ako nema 5 vrijednosti, nije YOLO format
                print("Neispravna linija")
                continue

            cls, x, y, w, h = map(float, parts) #izvlačenje vrijednosti iz "dijelova"
            class_labels.append(int(cls))
            bboxes.append([x, y, w, h])

    return bboxes, class_labels

#pisanje vrijednosti u .txt datoteku u YOLO formatu
def save_yolo_labels(path, bboxes, class_labels):
    with open(path, "w") as f:
        for cls, bbox in zip(class_labels, bboxes):
            f.write(f"{cls} {' '.join(map(str, bbox))}\n")

image_files = [f for f in os.listdir(img_dir) if f.endswith(".jpg")] #sakupljanje svih originalnih slika
counter = 0 #prebrojavnaje obrađenih slika

#glavna petlja za obradu svih slika s "progress bar-om"
for img_name in tqdm(image_files):

    img_path = os.path.join(img_dir, img_name)
    label_path = os.path.join(lbl_dir, img_name.replace(".jpg", ".txt"))

    image = cv2.imread(img_path)
    bboxes, class_labels = load_yolo_labels(label_path)

    #spremanje originalne slike
    base_name = f"img_{counter:05d}"
    cv2.imwrite(os.path.join(img_out, base_name + ".jpg"), image)
    save_yolo_labels(os.path.join(lbl_out, base_name + ".txt"), bboxes, class_labels)
    counter += 1

    #generiranje 10 augmentacija
    for i in range(aug_num):
        transformed = transform( #transformiranje slika i bboxeva
            image=image,
            bboxes=bboxes,
            class_labels=class_labels
        )

        aug_img = transformed["image"]
        aug_boxes = transformed["bboxes"]
        aug_classes = transformed["class_labels"]

        if len(aug_boxes) == 0: #preskakanje slike ako nema bboxeva
            continue
        
        #spremanje augmentirane slike
        base_name = f"img_{counter:05d}"
        cv2.imwrite(os.path.join(img_out, base_name + ".jpg"), aug_img) 
        save_yolo_labels(os.path.join(lbl_out, base_name + ".txt"), aug_boxes, aug_classes)

        counter += 1

print("Augmentirano slika:", counter)