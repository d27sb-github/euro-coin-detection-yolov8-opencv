/**
 * Napomene:
 *  - ako se želi mijenjati koja slika se obrađuje treba se izmijeniti 'image_file'
 *      --> unutar .rar datoteke nalazi se (zasebno) 20 .ppm datoteka za testiranje izvođenja
 *  - 'margin' integer se može mijenjati ako se želi više/manje "zoomirana" slika
 *  - učitana slika TREBA biti P6 .ppm formata jer samo takve podržava 'stb_image.h'
 *      --> 'jpegtopnm name.jpg > name.ppm' naredba u terminalu
 *  - program se kompajlirao u terminalu naredbom 'g++ ProjektniZadatak.c $(pkg-config --cflags --libs opencv4) -o ProjektniZadatak'
 *      --> razlog su C++ knjižice OpenCV-a
 *  - za uspješno izvođenje programa nužno je imati 'stb_image.h' datoteku u istom direktoriju kao i program
 * 
 * Tijek programa:
 *  - program učitava .ppm P6 sliku
 *  - stvara se kopija učitane slike u RGB formatu
 *  - RGB slika se prebacuje u BGR format
 *  - nad BGR slikom se prvo provodi grayscale transformacija
 *  - nad BGR slikom se potom provodi Gaussovo zamućenje
 *  - Hughovim transformacijama detekriaju se kružnice na slici
 *  - iterira se kroz vrijednosti svake kružnice i određuju krajnje točke
 *  - zadaje se margina te segmentira slika na kojoj se nalaze objekti
 *  - sprema se novonastala segmentirana slika
 *  - stvara se kopija zoomirane slike u RGB formatu
 *  - iterira se kroz vrijednosti svake kružnice i crtaju se na slici 
 *  - sprema se novonastala slika s oznakama
 *  - oslobađa se memorija
*/

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION //uključivanje prave implementacije funkcija iz "stb_image.h"
#include "stb_image.h" //knjižica za učitavanje slika različitih formata (uključujući .ppm)

#include <opencv2/opencv.hpp> //knjižica za obradu slika kako bi se detektirali objekti
using namespace cv; //namespace za OpenCV

//spremanje ppm
void save_ppm(const char *file, unsigned char *image, int img_w, int img_h) {
    FILE *fp = fopen(file, "wb");
    fprintf(fp, "P6\n%d %d\n255\n", img_w, img_h);
    fwrite(image, 3, img_w * img_h, fp);
    fclose(fp);
}

//zoom segment slike
unsigned char* segment_image(unsigned char *image, int img_w, int img_h, int x_min, int y_min, int x_max, int y_max, int *zoom_w_out, int *zoom_h_out)
{
    int zoom_w = x_max - x_min;
    int zoom_h = y_max - y_min; //dimenzije nove slike

    //alociranje memorije za zoomiranu sliku
    unsigned char *zoom = (unsigned char*)malloc(3 * zoom_w * zoom_h);

    for (int y = 0; y < zoom_h; y++) {
        for (int x = 0; x < zoom_w; x++) {
            int src_idx = 3 * ((y + y_min) * img_w + (x + x_min)); //indeks trenutnog piksela (na R poziciji) originalne slike
            int dst_idx = 3 * (y * zoom_w + x); //indeks trenutnog piksela (na R poziciji) zoomirane slike

            zoom[dst_idx] = image[src_idx]; //R
            zoom[dst_idx + 1] = image[src_idx + 1]; //G
            zoom[dst_idx + 2] = image[src_idx + 2]; //B
        }
    }

    *zoom_w_out = zoom_w;
    *zoom_h_out = zoom_h; //dimenzije nove slike za sliku s oznakama

    return zoom;
}

int main() {

    char image_file[] = "027.ppm"; //naziv slike koja se učitava

    int img_w, img_h, chr;

    //stbi funkcija za učitavanje .ppm slike
    unsigned char *image = stbi_load(image_file, &img_w, &img_h, &chr, 3);

    //stvaranje RGB slike dimenzija originalne slike
    Mat img_rgb1(img_h, img_w, CV_8UC3, image);

    //stvaranje BGR slike iz RGB slike
    Mat img_bgr1;
    cvtColor(img_rgb1, img_bgr1, COLOR_RGB2BGR);

    //primjena grayscalea na BGR sliku 
    Mat gray;
    cvtColor(img_bgr1, gray, COLOR_BGR2GRAY);

    //primjena Gaussovog zamućenja na grayscale sliku
    GaussianBlur(gray, gray, Size(9, 9), 2, 2);

    //Hugh transformacija za detekciju kružnica na slici
    std::vector<Vec3f> circles;
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1, //metoda i razmjer
                 gray.rows / 9, //minimalna udaljenost između središta kružnica
                 120, 20, //osjetljivost detekcije
                 35, 100); //minimalni i maksimalni raspon kružnica

    printf("Detektirano objekata: %lu\n", circles.size());

    //početne vrijednosti graničnih točaka
    int x_min = img_w, y_min = img_h;
    int x_max = 0,     y_max = 0;

    //čitanje vrijednosti svakog kruga, određivanje krajnjih točaka slike
    for (int i = 0; i < circles.size(); i++) {
        int circle_x = cvRound(circles[i][0]);
        int circle_y = cvRound(circles[i][1]);
        int radius  = cvRound(circles[i][2]);

        if (circle_x - radius < x_min) x_min = circle_x - radius;
        if (circle_y - radius < y_min) y_min = circle_y - radius; //gornji lijevi kut
        if (circle_x + radius > x_max) x_max = circle_x + radius;
        if (circle_y + radius > y_max) y_max = circle_y + radius; //donji desni kut
    }

    int margin = 10; //postavljanje margina za zoomiranu sliku

    x_min = (x_min - margin < 0) ? 0 : x_min - margin;
    y_min = (y_min - margin < 0) ? 0 : y_min - margin; // gornji lijevi kut zoomirane slike
    x_max = (x_max + margin > img_w) ? img_w : x_max + margin;
    y_max = (y_max + margin > img_h) ? img_h : y_max + margin; //donji desnji kut zoomirane slike

    int zoom_w, zoom_h;
    unsigned char *zoom = segment_image(image, img_w, img_h, x_min, y_min, x_max, y_max, &zoom_w, &zoom_h);

    //spremanje segmentirane slike
    save_ppm("segmented_image.ppm", zoom, zoom_w, zoom_h);

    //stvaranje RGB slike dimenzija zoomirane slike
    Mat img_rgb2(zoom_h, zoom_w, CV_8UC3, zoom);

    //iterira kroz svaku kružnicu i iscrtavanje iste
    for (int i = 0; i < circles.size(); i++) {
        Point center(cvRound(circles[i][0] - x_min), cvRound(circles[i][1]) - y_min); //izračunavanje središta
        int radius = cvRound(circles[i][2]); //izračunavanje radijusa
        circle(img_rgb2, center, radius, Scalar(0,255,0), 2); //crtanje kružnice
    }

    //spremanje slike s oznakama
    save_ppm("labeled_image.ppm", img_rgb2.data, zoom_w, zoom_h);

    //oslobađanje memorije
    stbi_image_free(image);
    free(zoom);

    return 0;
}