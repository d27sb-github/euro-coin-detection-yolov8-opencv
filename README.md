# Detekcija i klasifikacija euro kovanica

Ovaj repozitorij sadrži programsku realizaciju završnog rada na temu segmentacije, detekcije, prebrojavanja i klasifikacije euro kovanica na slici. U radu su izrađena i uspoređena dva pristupa:

1. **klasični pristup** temeljen na digitalnoj obradi slike, OpenCV knjižnici i Houghovoj transformaciji za detekciju kružnica,
2. **moderni pristup** temeljen na pripremi skupa podataka, augmentaciji, treniranju i inferenciji pomoću modela **YOLOv8n**.

Klasični pristup pronalazi kružne oblike i koristi se za segmentaciju, detekciju i prebrojavanje kovanica. Moderni pristup koristi istrenirani YOLOv8n model koji za svaku detektiranu kovanicu određuje granični okvir, pripadajuću klasu i koeficijent pouzdanosti.

---

## Izvori podataka i rezultantnih slika

Za razvoj i testiranje korišteni su sljedeći izvori slika:

- Kaggle baza podataka euro kovanica: <https://www.kaggle.com/datasets/janstaffa/euro-coins-dataset>
- DepositPhotos slike euro kovanica: <https://depositphotos.com/photos/euro-coins.html>
- Getty Images slike euro kovanica: <https://www.gettyimages.com/photos/euro-coins>

Direktorij `Detekcije_Klasik/` sadrži rezultantne slike dobivene klasičnim pristupom. Direktorij `Detekcije_Modern/` sadrži rezultantne slike dobivene YOLOv8n pristupom. Uz klasične detekcije nalazi se i datoteka `Parametri_Detekcija.txt`, u kojoj su zapisani parametri Houghove transformacije korišteni za pojedine slike.

---

## Predložena struktura repozitorija

```text
.
├── README.md
├── Klasicna_Metoda.c
├── AugmentacijaDataseta.py
├── PodjelaDataseta.py
├── TreniranjeYOLO.py
├── Detektiranje.py
├── dataset.yaml
├── Detekcije_Klasik/
│   ├── KDetekcija1.PNG
│   ├── KDetekcija2.PNG
│   ├── KDetekcija3.PNG
│   ├── KDetekcija4.PNG
│   ├── KDetekcija5.PNG
│   ├── KDetekcija6.PNG
│   ├── KDetekcija7.PNG
│   ├── KDetekcija8.PNG
│   ├── KDetekcija9.PNG
│   └── Parametri_Detekcija.txt
└── Detekcije_Modern/
    ├── MDetekcija1.PNG
    ├── MDetekcija2.PNG
    ├── MDetekcija3.PNG
    ├── MDetekcija4.PNG
    ├── MDetekcija5.PNG
    ├── MDetekcija6.PNG
    ├── MDetekcija7.PNG
    ├── MDetekcija8.PNG
    └── MDetekcija9.PNG
```

Ako se direktoriji raspakiraju pod nazivima `Detekcije Klasik` i `Detekcije Modern`, potrebno je prilagoditi putanje u prikazu slika ili preimenovati mape u `Detekcije_Klasik` i `Detekcije_Modern` radi lakšeg korištenja na GitHubu.

---

## 1. Klasični pristup

### Okruženje

Klasični pristup izrađen je i testiran u sljedećem okruženju:

- virtualno okruženje unutar **Oracle VirtualBoxa**,
- **Linux OS**, Ubuntu distribucija,
- Linux terminal,
- `stb_image.h` knjižnica za učitavanje slika,
- **OpenCV** knjižnica za obradu slike i detekciju kružnica.

### Opis metode

Klasični pristup implementiran je u datoteci `Klasicna_Metoda.c`. Iako datoteka ima nastavak `.c`, program se prevodi pomoću `g++` jer koristi OpenCV C++ funkcionalnosti.

Tijek obrade slike:

1. učitavanje ulazne `.ppm` slike pomoću `stb_image.h`,
2. stvaranje OpenCV matrice iz učitanih piksela,
3. pretvorba slike iz RGB u BGR format,
4. pretvorba u sivu sliku,
5. primjena Gaussovog zamućenja,
6. detekcija kružnica pomoću Houghove transformacije,
7. određivanje područja slike koje obuhvaća detektirane kovanice,
8. izrezivanje segmenta slike,
9. označavanje detektiranih kružnica,
10. spremanje segmentirane i označene slike.

Klasični pristup ne klasificira nominalne vrijednosti kovanica. Sve kovanice promatra kao kružne objekte, pa je njegova glavna namjena detekcija i prebrojavanje.

### Preduvjeti

Na Ubuntu sustavu potrebno je instalirati OpenCV i alate za prevođenje:

```bash
sudo apt update
sudo apt install build-essential pkg-config libopencv-dev netpbm
```

Datoteka `stb_image.h` mora se nalaziti u istom direktoriju kao i `Klasicna_Metoda.c`.

Ulazna slika mora biti u **PPM P6** formatu. Ako se koristi `.jpg` slika, može se pretvoriti naredbom:

```bash
jpegtopnm naziv_slike.jpg > naziv_slike.ppm
```

### Prevođenje i pokretanje

Program se prevodi naredbom:

```bash
g++ Klasicna_Metoda.c $(pkg-config --cflags --libs opencv4) -o Klasicna_Metoda
```

Pokretanje programa:

```bash
./Klasicna_Metoda
```

Slika koja se obrađuje mijenja se u programu promjenom vrijednosti varijable:

```c
char image_file[] = "027.ppm";
```

Nakon izvođenja programa nastaju izlazne datoteke:

- `segmented_image.ppm` – segment slike na kojem se nalaze detektirane kovanice,
- `labeled_image.ppm` – segmentirana slika s označenim kružnicama.

### Parametri Houghove transformacije

Za usporedbu klasičnog pristupa korišteni su različiti parametri Houghove transformacije. Prve četiri slike obrađene su istim parametrima, dok su za složenije slike parametri prilagođeni zasebno.

| Slika | Minimalna udaljenost središta | Parametri osjetljivosti | Minimalni i maksimalni polumjer |
|---|---:|---:|---:|
| KDetekcija1–4 | `gray.rows / 9` | `120, 20` | `35, 100` |
| KDetekcija5 | `gray.rows / 5` | `140, 20` | `125, 250` |
| KDetekcija6 | `gray.rows / 15` | `120, 25` | `55, 100` |
| KDetekcija7 | `gray.rows / 6` | `230, 23` | `50, 100` |
| KDetekcija8 | `gray.rows / 9` | `110, 20` | `50, 100` |
| KDetekcija9 | `gray.rows / 9` | `100, 25` | `50, 100` |

Promjena parametara pokazuje da je klasični pristup osjetljiv na veličinu kovanica, udaljenost kamere, pozadinu, preklapanje objekata i opću složenost scene.

---

## 2. Moderni pristup YOLOv8n modelom

### Okruženje

Moderni pristup izrađen je i testiran u sljedećem okruženju:

- **Windows 10 OS**,
- Windows terminal,
- 16 GB RAM-a,
- AMD Ryzen 7 3800X,
- NVIDIA GeForce RTX 2070 SUPER,
- Python 3.11.9,
- Ultralytics 8.4.14,
- NumPy 1.26.4,
- OpenCV 4.8.1.78,
- PyTorch 2.7.1+cu118,
- Torchvision 0.22.1+cu118,
- Matplotlib 3.10.8.

Za augmentaciju i prikaz napretka koriste se i knjižnice **Albumentations** i **tqdm**.

### Instalacija Python okruženja

Preporučeno je koristiti virtualno okruženje:

```bash
python -m venv venv
venv\Scripts\activate
python -m pip install --upgrade pip
```

Primjer instalacije glavnih paketa:

```bash
pip install ultralytics==8.4.14 numpy==1.26.4 opencv-python==4.8.1.78 matplotlib==3.10.8 albumentations tqdm
pip install torch==2.7.1+cu118 torchvision==0.22.1+cu118 --index-url https://download.pytorch.org/whl/cu118
```

### Struktura skupa podataka

Početni skup podataka treba biti organiziran ovako:

```text
dataset/
├── images/
│   ├── slika_1.jpg
│   ├── slika_2.jpg
│   └── ...
└── labels/
    ├── slika_1.txt
    ├── slika_2.txt
    └── ...
```

Oznake su zapisane u YOLO formatu:

```text
class x_center y_center width height
```

Vrijednosti koordinata i dimenzija graničnog okvira normalizirane su na raspon od 0 do 1.

Datoteka `dataset.yaml` definira konačni skup podataka:

```yaml
path: dataset_final

train: images/train
val: images/val

names:
  0: 1cent
  1: 2cent
  2: 5cent
  3: 10cent
  4: 20cent
  5: 50cent
  6: 1euro
  7: 2euro
```

---

## 3. Redoslijed izvođenja modernog pristupa

### 3.1. Augmentacija skupa podataka

Skripta `AugmentacijaDataseta.py` čita slike iz `dataset/images` i oznake iz `dataset/labels`. Izvorne i augmentirane slike spremaju se u direktorij `dataset_aug`.

Pokretanje:

```bash
python AugmentacijaDataseta.py
```

Korištene transformacije uključuju:

- rotaciju,
- promjenu svjetline i kontrasta,
- Gaussov šum,
- zamućenje pokretom,
- promjenu perspektive,
- horizontalno zrcaljenje,
- nasumične sjene,
- promjenu veličine slike.

Posebno je važno da se transformiraju i granični okviri, što je ostvareno pomoću:

```python
bbox_params=A.BboxParams(format='yolo', label_fields=['class_labels'])
```

### 3.2. Podjela podataka na train i val skup

Skripta `PodjelaDataseta.py` dijeli augmentirani skup podataka iz `dataset_aug` u omjeru **80:20** i sprema ga u direktorij `dataset_final`.

Pokretanje:

```bash
python PodjelaDataseta.py
```

Nakon izvođenja nastaje struktura:

```text
dataset_final/
├── images/
│   ├── train/
│   └── val/
└── labels/
    ├── train/
    └── val/
```

### 3.3. Treniranje YOLOv8n modela

Skripta `TreniranjeYOLO.py` učitava prethodno trenirani model `yolov8n.pt` i dodatno ga trenira na pripremljenom skupu podataka.

Pokretanje:

```bash
python TreniranjeYOLO.py
```

Korišteni parametri treniranja:

| Parametar | Vrijednost |
|---|---:|
| model | `yolov8n.pt` |
| data | `dataset.yaml` |
| epochs | `120` |
| imgsz | `640` |
| batch | `16` |
| device | `0` |
| workers | `8` |
| patience | `30` |
| project | `coin_training` |
| name | `euro_coins` |

Nakon treniranja Ultralytics sprema rezultate i težine modela. Za inferenciju se koristi najbolji spremljeni model:

```text
best.pt
```

Ako se naziv izlaznog direktorija razlikuje, potrebno je prilagoditi putanju do modela u skripti `Detektiranje.py`.

### 3.4. Detekcija pomoću istreniranog modela

Skripta `Detektiranje.py` učitava sliku i najbolji spremljeni YOLOv8n model te pokreće detekciju s pragom pouzdanosti `conf=0.5`.

Pokretanje:

```bash
python Detektiranje.py
```

Ključni dio skripte:

```python
image = cv2.imread("096.jpg")
model = YOLO("runs/detect/coin_training/euro_coins_nano/weights/best.pt")
results = model(image, conf=0.5)
results[0].show()
```

Rezultat detekcije prikazuje:

- granični okvir svake kovanice,
- naziv klase,
- koeficijent pouzdanosti.

---

## 4. Rezultati detekcije

### 4.1. Rezultati klasičnog pristupa

U nastavku su prikazani rezultati klasične detekcije kružnica pomoću Houghove transformacije.

| Primjer | Rezultat |
|---|---|
| KDetekcija1 | ![KDetekcija1](Detekcije_Klasik/KDetekcija1.PNG) |
| KDetekcija2 | ![KDetekcija2](Detekcije_Klasik/KDetekcija2.PNG) |
| KDetekcija3 | ![KDetekcija3](Detekcije_Klasik/KDetekcija3.PNG) |
| KDetekcija4 | ![KDetekcija4](Detekcije_Klasik/KDetekcija4.PNG) |
| KDetekcija5 | ![KDetekcija5](Detekcije_Klasik/KDetekcija5.PNG) |
| KDetekcija6 | ![KDetekcija6](Detekcije_Klasik/KDetekcija6.PNG) |
| KDetekcija7 | ![KDetekcija7](Detekcije_Klasik/KDetekcija7.PNG) |
| KDetekcija8 | ![KDetekcija8](Detekcije_Klasik/KDetekcija8.PNG) |
| KDetekcija9 | ![KDetekcija9](Detekcije_Klasik/KDetekcija9.PNG) |

Klasični pristup može uspješno detektirati kovanice kada su kružni oblici jasno vidljivi, ali je osjetljiv na promjene parametara, pozadinu, veličinu kovanica i preklapanje objekata.

### 4.2. Rezultati modernog YOLOv8n pristupa

U nastavku su prikazani rezultati detekcije pomoću istreniranog YOLOv8n modela.

| Primjer | Rezultat |
|---|---|
| MDetekcija1 | ![MDetekcija1](Detekcije_Modern/MDetekcija1.PNG) |
| MDetekcija2 | ![MDetekcija2](Detekcije_Modern/MDetekcija2.PNG) |
| MDetekcija3 | ![MDetekcija3](Detekcije_Modern/MDetekcija3.PNG) |
| MDetekcija4 | ![MDetekcija4](Detekcije_Modern/MDetekcija4.PNG) |
| MDetekcija5 | ![MDetekcija5](Detekcije_Modern/MDetekcija5.PNG) |
| MDetekcija6 | ![MDetekcija6](Detekcije_Modern/MDetekcija6.PNG) |
| MDetekcija7 | ![MDetekcija7](Detekcije_Modern/MDetekcija7.PNG) |
| MDetekcija8 | ![MDetekcija8](Detekcije_Modern/MDetekcija8.PNG) |
| MDetekcija9 | ![MDetekcija9](Detekcije_Modern/MDetekcija9.PNG) |

YOLOv8n pristup uz detekciju omogućuje i klasifikaciju vrijednosti kovanica. Za svaku detektiranu kovanicu prikazuju se granični okvir, klasa i koeficijent pouzdanosti.

---

## Datoteke u repozitoriju

| Datoteka | Opis |
|---|---|
| `Klasicna_Metoda.c` | Klasična metoda detekcije kružnica pomoću OpenCV-a i Houghove transformacije |
| `AugmentacijaDataseta.py` | Augmentacija slika i YOLO oznaka pomoću Albumentations knjižnice |
| `PodjelaDataseta.py` | Nasumična podjela augmentiranog skupa u omjeru 80:20 |
| `dataset.yaml` | Konfiguracija YOLO skupa podataka i popis klasa |
| `TreniranjeYOLO.py` | Treniranje YOLOv8n modela |
| `Detektiranje.py` | Inferencija pomoću istreniranog YOLOv8n modela |
| `Detekcije_Klasik/` | Rezultati detekcije klasičnim pristupom |
| `Detekcije_Modern/` | Rezultati detekcije modernim YOLOv8n pristupom |

---

## Zaključak

U repozitoriju su prikazana dva pristupa za obradu slika euro kovanica. Klasični pristup jednostavan je i objašnjiv, ali ovisi o ručno odabranim parametrima i ne omogućuje klasifikaciju vrijednosti kovanica. Moderni YOLOv8n pristup zahtijeva pripremu podataka i treniranje modela, ali omogućuje detekciju, prebrojavanje i klasifikaciju kovanica u jednom postupku.
