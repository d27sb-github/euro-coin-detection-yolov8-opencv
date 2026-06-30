import os
import random #nasumični odabir
import shutil #kopiranje datoteka

src_img = "dataset_aug/images"
src_lbl = "dataset_aug/labels"

dst_dir = "dataset_final"

#omjer podataka treniranje/validacija
train_val_ratio = 0.8

#direktoriji za podjelu
for p in [
    f"{dst_dir}/images/train",
    f"{dst_dir}/images/val",
    f"{dst_dir}/labels/train",
    f"{dst_dir}/labels/val"
]:
    os.makedirs(p, exist_ok=True)

files = os.listdir(src_img) #sve slike
random.shuffle(files) #nasumično miješanje rasporeda slika

split_idx = int(len(files) * train_val_ratio) 
train_files = files[:split_idx] #prvih 80% slika
val_files = files[split_idx:] #ostatak slika

def copy(files, subset): #funkcija za kopiranje slika i labela
    for f in files:
        name = os.path.splitext(f)[0] #uzimanje imena bez tipa datoteke

        shutil.copy( #kopiranje slike
            os.path.join(src_img, f),
            f"{dst_dir}/images/{subset}/{f}"
        )

        shutil.copy( #kopiranje labele
            os.path.join(src_lbl, name + ".txt"),
            f"{dst_dir}/labels/{subset}/{name}.txt"
        )

copy(train_files, "train")
copy(val_files, "val")

print("Train:", len(train_files)) #broj slika za treniranje
print("Val:", len(val_files)) #broj slika za validaciju
