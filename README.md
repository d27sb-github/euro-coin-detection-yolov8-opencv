# Detekcija i klasifikacija euro kovanica pomoću OpenCV-a i YOLOv8n modela

Ovaj projekt izrađen je kao praktični dio završnog rada na temu **segmentacije, detekcije, prebrojavanja i klasifikacije euro kovanica na slici**. Cilj projekta je prikazati dvije različite programske realizacije istog zadatka: klasični pristup temeljen na digitalnoj obradi slike i moderni pristup temeljen na modelu dubokog učenja **YOLOv8n**.

Klasični pristup koristi programski jezik C, knjižnicu `stb_image.h`, OpenCV i Houghovu transformaciju za pronalazak kružnih oblika na slici. Takav pristup omogućuje segmentaciju, detekciju i prebrojavanje kovanica, ali ne razlikuje njihove nominalne vrijednosti. Moderni pristup koristi Python, pripremu skupa podataka u YOLO formatu, augmentaciju slika, treniranje YOLOv8n modela i detekciju novih slika pomoću istreniranog modela. Za svaku detektiranu kovanicu model prikazuje granični okvir, naziv klase i koeficijent pouzdanosti.

## Korišteni pristupi

U projektu su implementirana dva pristupa:

1. **Klasični pristup** – implementiran u jeziku C. Program učitava sliku, provodi predobradu, detektira kružnice Houghovom transformacijom, izdvaja segment slike i označava pronađene kovanice.
2. **Moderni YOLOv8n pristup** – implementiran u jeziku Python. Postupak uključuje augmentaciju skupa podataka, podjelu podataka na skup za treniranje i validaciju, treniranje YOLOv8n modela te detekciju i klasifikaciju kovanica.

Klasični pristup prikazuje objašnjiv algoritamski način rješavanja problema, dok moderni pristup prikazuje rješenje temeljeno na učenju značajki iz označenih primjera.

## Struktura projekta

Predložena struktura repozitorija je:

```text
ZAVRSNI_RAD/
├── dataset/
│   ├── images/
│   └── labels/
├── dataset_aug/
│   ├── images/
│   └── labels/
├── dataset_final/
│   ├── images/
│   │   ├── train/
│   │   └── val/
│   └── labels/
│       ├── train/
│       └── val/
├── AugmentacijaDataseta.py
├── PodjelaDataseta.py
├── TreniranjeYOLO.py
├── Detektiranje.py
├── Klasicna_Metoda.c
├── dataset.yaml
└── README.md
```

Mape `dataset_aug/`, `dataset_final/` i direktoriji s rezultatima treniranja mogu se generirati pokretanjem odgovarajućih skripti. Ako se ne nalaze u repozitoriju, potrebno ih je stvoriti redoslijedom opisanim u nastavku.

## Opis datoteka

* `Klasicna_Metoda.c` – implementacija klasičnog pristupa u jeziku C. Program učitava `.ppm` sliku, provodi predobradu, detektira kružnice Houghovom transformacijom, izdvaja segment slike i sprema označeni rezultat.
* `AugmentacijaDataseta.py` – skripta za augmentaciju slika i pripadajućih YOLO oznaka. Koristi Albumentations transformacije i stvara prošireni skup podataka u mapi `dataset_aug`.
* `PodjelaDataseta.py` – skripta koja nasumično dijeli augmentirani skup podataka u omjeru 80:20 na skup za treniranje i skup za validaciju te ga sprema u mapu `dataset_final`.
* `dataset.yaml` – konfiguracijska datoteka za Ultralytics YOLO treniranje. Sadrži putanju do konačnog skupa podataka i popis klasa euro kovanica.
* `TreniranjeYOLO.py` – skripta za treniranje YOLOv8n modela na pripremljenom skupu podataka.
* `Detektiranje.py` – skripta za učitavanje istreniranog modela `best.pt` i pokretanje detekcije nad odabranom slikom.

## Izvori skupa podataka

Za treniranje i testiranje korištene su slike euro kovanica iz javno dostupnih izvora:

* Kaggle EURO coins dataset: <https://www.kaggle.com/datasets/janstaffa/euro-coins-dataset>
* DepositPhotos – euro coins: <https://depositphotos.com/photos/euro-coins.html>
* Getty Images – euro coins: <https://www.gettyimages.com/photos/euro-coins>

## Klasični pristup

### Okruženje

Klasični pristup izrađen je i testiran u sljedećem okruženju:

* virtualno okruženje unutar Oracle VirtualBoxa,
* Linux OS, Ubuntu distribucija,
* Linux terminal,
* `stb_image.h` knjižnica za učitavanje slika,
* OpenCV knjižnica za obradu slike i detekciju kružnica.

### Opis metode

Klasični pristup temelji se na pretpostavci da su kovanice približno kružnog oblika. Program ne uči iz skupa podataka, nego koristi unaprijed određene algoritamske korake i parametre.

Tijek obrade slike je:

1. učitavanje ulazne slike u PPM P6 formatu pomoću `stb_image.h`,
2. stvaranje OpenCV matrice iz učitanih piksela,
3. pretvorba slike iz RGB u BGR format,
4. pretvorba slike u sivu sliku,
5. primjena Gaussovog zamućenja radi smanjenja šuma,
6. detekcija kružnica pomoću Houghove transformacije,
7. određivanje područja slike koje obuhvaća sve pronađene kovanice,
8. izdvajanje segmenta slike,
9. označavanje detektiranih kružnica,
10. spremanje segmentirane i označene slike.

Klasični pristup može detektirati i prebrojati kovanice, ali ne određuje njihovu nominalnu vrijednost.

### Priprema okruženja

Na Ubuntu sustavu potrebno je instalirati OpenCV i alate za prevođenje:

```bash
sudo apt update
sudo apt install build-essential pkg-config libopencv-dev netpbm
```

Datoteka `stb_image.h` mora se nalaziti u istom direktoriju kao i `Klasicna_Metoda.c`.

Ulazna slika mora biti u **PPM P6** formatu. Ako je početna slika u `.jpg` formatu, može se pretvoriti naredbom:

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

Slika koja se obrađuje mijenja se u programu izmjenom vrijednosti varijable:

```c
char image_file[] = "027.ppm";
```

Nakon pokretanja programa nastaju izlazne datoteke:

* `segmented_image.ppm` – izrezani segment slike na kojem se nalaze detektirane kovanice,
* `labeled_image.ppm` – segmentirana slika s označenim kružnicama.

## Moderni pristup YOLOv8n modelom

### Okruženje

Moderni pristup izrađen je i testiran na sljedećoj konfiguraciji:

* Windows 10 OS,
* Windows terminal,
* 16 GB RAM-a,
* AMD Ryzen 7 3800X,
* NVIDIA GeForce RTX 2070 SUPER,
* Python 3.11.9,
* Ultralytics 8.4.14,
* NumPy 1.26.4,
* OpenCV 4.8.1.78,
* PyTorch 2.7.1+cu118,
* Torchvision 0.22.1+cu118,
* Matplotlib 3.10.8.

Za augmentaciju i prikaz napretka izvođenja koriste se i knjižnice **Albumentations** i **tqdm**.

### Priprema Python okruženja

Preporučuje se korištenje virtualnog okruženja.

Izrada virtualnog okruženja:

```powershell
python -m venv venv
```

Aktivacija virtualnog okruženja u Windows PowerShellu:

```powershell
.\venv\Scripts\Activate.ps1
```

Ažuriranje `pip` alata:

```powershell
python -m pip install --upgrade pip
```

Instalacija glavnih biblioteka:

```powershell
pip install ultralytics==8.4.14 numpy==1.26.4 opencv-python==4.8.1.78 matplotlib==3.10.8 albumentations tqdm
```

Instalacija PyTorch i Torchvision paketa s CUDA 11.8 podrškom:

```powershell
pip install torch==2.7.1+cu118 torchvision==0.22.1+cu118 --index-url https://download.pytorch.org/whl/cu118
```

## Priprema skupa podataka

Početni skup podataka treba biti organiziran u sljedećem obliku:

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

Svaka slika mora imati pripadajuću `.txt` datoteku s oznakama u YOLO formatu:

```text
class x_center y_center width height
```

Vrijednosti koordinata i dimenzija graničnog okvira normalizirane su u odnosu na širinu i visinu slike.

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

## Pokretanje modernog pristupa

Moderni pristup izvodi se kroz četiri glavna koraka: augmentaciju, podjelu podataka, treniranje i detekciju.

### 1. Augmentacija podataka

Skripta `AugmentacijaDataseta.py` čita slike iz `dataset/images` i oznake iz `dataset/labels`. Izvorne i augmentirane slike spremaju se u mapu `dataset_aug`.

Pokretanje:

```powershell
python AugmentacijaDataseta.py
```

Korištene transformacije uključuju:

* rotaciju,
* promjenu svjetline i kontrasta,
* Gaussov šum,
* zamućenje pokretom,
* promjenu perspektive,
* horizontalno zrcaljenje,
* nasumične sjene,
* promjenu veličine slike.

Posebno je važno da se zajedno sa slikom transformiraju i pripadajući granični okviri. To je ostvareno pomoću postavke:

```python
bbox_params=A.BboxParams(format='yolo', label_fields=['class_labels'])
```

### 2. Podjela podataka

Skripta `PodjelaDataseta.py` uzima slike i oznake iz mape `dataset_aug`, nasumično ih miješa i dijeli u omjeru 80:20. Konačni skup podataka sprema se u mapu `dataset_final`.

Pokretanje:

```powershell
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

### 3. Treniranje YOLOv8n modela

Skripta `TreniranjeYOLO.py` učitava prethodno trenirani model `yolov8n.pt` i dodatno ga trenira na pripremljenom skupu podataka.

Pokretanje:

```powershell
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

Nakon treniranja Ultralytics sprema rezultate i težine modela. Za kasniju detekciju koristi se najbolji spremljeni model:

```text
best.pt
```

Ako se naziv izlazne mape razlikuje od putanje u skripti `Detektiranje.py`, potrebno je prilagoditi putanju do modela.

### 4. Detekcija pomoću istreniranog modela

Skripta `Detektiranje.py` učitava sliku i najbolji spremljeni YOLOv8n model te pokreće detekciju s pragom pouzdanosti `conf=0.5`.

Pokretanje:

```powershell
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

* granični okvir svake kovanice,
* naziv klase,
* koeficijent pouzdanosti.



## Napomena

Model `yolov8n.pt` pri prvom pokretanju može biti preuzet automatski putem Ultralytics paketa ako nije već dostupan lokalno. Nakon treniranja za inferenciju se preporučuje koristiti datoteku `best.pt`, koja predstavlja najbolji model prema validacijskim rezultatima.

Direktoriji `dataset_aug/`, `dataset_final/`, `coin_training/` i datoteke težina modela mogu zauzimati više prostora. Ako se ne uključuju u repozitorij, potrebno ih je generirati pokretanjem skripti redoslijedom opisanim u ovom README dokumentu.

## Autor

Dario Šaša, student Tehničkog fakulteta Sveučilišta u Rijeci
