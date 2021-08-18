#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define nombre_threads 8
#define nombre_end_line nombre_threads+1
#define taille_horizontale 4716
#define taille_verticale 5000
#define BYTES_PER_PIXEL 3
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;
const int ITERATION_MAX=300;
/*
const double PLAN_COMPLEXE_LIM_GAUCHE=-2;
const double PLAN_COMPLEXE_LIM_DROITE=2;
const double PLAN_COMPLEXE_LIM_HAUT=1;
const double PLAN_COMPLEXE_LIM_BAS=-1;
*/
const double PLAN_COMPLEXE_LIM_GAUCHE=-1.758;
const double PLAN_COMPLEXE_LIM_DROITE=-1.75775;
const double PLAN_COMPLEXE_LIM_HAUT=-0.01624;
const double PLAN_COMPLEXE_LIM_BAS=-0.01677;

unsigned char fractale_mandelbrott1[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
unsigned char fractale_mandelbrott2[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
unsigned char fractale_mandelbrott3[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
unsigned char fractale_mandelbrott4[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
unsigned char fractale_mandelbrott5[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
unsigned char fractale_mandelbrott6[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
unsigned char fractale_mandelbrott7[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
unsigned char fractale_mandelbrott8[taille_verticale][taille_horizontale][BYTES_PER_PIXEL];
int tab_repartition[nombre_threads];
int tab_endline[nombre_end_line];

void generateBitmapImage(char* imageFileName);
int Mandelbrott_Divergence(int ligne,int colonne);
unsigned char* createBitmapFileHeader(int stride);
unsigned char* createBitmapInfoHeader();
void* thread_calc1(void* arg);
void* thread_calc2(void* arg);
void* thread_calc3(void* arg);
void* thread_calc4(void* arg);
void* thread_calc5(void* arg);
void* thread_calc6(void* arg);
void* thread_calc7(void* arg);
void* thread_calc8(void* arg);


struct timespec start, finish;
double elapsed;

int main (int argc,char** argv)
{
    int thread_Number=0;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int itmax=ITERATION_MAX;
    char* imageFileName = (char*) "fractale de Mandelbrot no threading.bmp";
	if(strcmp(*(argv+1),"thread")==0)
	{
        imageFileName = (char*) "fractale de Mandelbrot threading.bmp";
        thread_Number=nombre_threads;
		tab_endline[0]=0;
		pthread_t thread_tab[nombre_threads];
		int i;
		for(i=0;i<nombre_threads;i++)
		{
		    tab_repartition[i]=i;
			tab_endline[i+1]=taille_verticale*(tab_repartition[i]+1)/nombre_threads;
		}
		pthread_create(&thread_tab[0],NULL,&thread_calc1,&tab_repartition[0]);
		pthread_create(&thread_tab[1],NULL,&thread_calc2,&tab_repartition[1]);
		pthread_create(&thread_tab[2],NULL,&thread_calc3,&tab_repartition[2]);
		pthread_create(&thread_tab[3],NULL,&thread_calc4,&tab_repartition[3]);
		pthread_create(&thread_tab[4],NULL,&thread_calc5,&tab_repartition[4]);
		pthread_create(&thread_tab[5],NULL,&thread_calc6,&tab_repartition[5]);
		pthread_create(&thread_tab[6],NULL,&thread_calc7,&tab_repartition[6]);
		pthread_create(&thread_tab[7],NULL,&thread_calc8,&tab_repartition[7]);
		for(i=0;i<nombre_threads;i++)
		{
		    pthread_join(thread_tab[i],NULL);
		}
	}
	else
	{

		tab_endline[1]=taille_verticale;
		int ligne,colonne;
    	for(ligne=0;ligne<taille_verticale;ligne++)
    	{
  	    	for(colonne=0;colonne<taille_horizontale;colonne++)
       	 	{
           		int deg_divergence=Mandelbrott_Divergence(ligne,colonne);
           		int couleur_pixel=255.0*deg_divergence/ITERATION_MAX;
                fractale_mandelbrott1[ligne][colonne][2] = couleur_pixel;///red
                fractale_mandelbrott1[ligne][colonne][1] = couleur_pixel;///green
                fractale_mandelbrott1[ligne][colonne][0] = couleur_pixel;///blue
            }
        }
    }

	generateBitmapImage(imageFileName);
	clock_gettime(CLOCK_MONOTONIC, &finish);
 	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;;
	printf("Avec %d threads : Image generated in %f seconds\n",thread_Number,elapsed);

}
int Mandelbrott_Divergence(int ligne,int colonne)
{
    int iteration=0;
    int divergence=0;
    double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
    double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
    double partie_reelle=partie_reelle_depart;
    double partie_imaginaire=partie_imaginaire_depart;
    while(iteration<ITERATION_MAX && divergence==0)
    {
        if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
        {
            divergence=1;
        }
        double save_partie_reelle=partie_reelle;
        partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
        partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
        iteration++;
    }
    return iteration;

}
void* thread_calc1(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott1[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott1[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott1[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void* thread_calc2(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott2[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott2[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott2[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void* thread_calc3(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott3[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott3[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott3[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void* thread_calc4(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott4[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott4[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott4[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void* thread_calc5(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott5[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott5[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott5[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void* thread_calc6(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott6[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott6[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott6[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void* thread_calc7(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott7[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott7[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott7[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void* thread_calc8(void* arg)
{
    int* pointer_repartition=(int*)arg;
    int repartition=*pointer_repartition;
	int start_line=tab_endline[repartition];
	int end_line=tab_endline[repartition+1]-1;
	int ligne,colonne;
    for(ligne=start_line;ligne<=end_line;ligne++)
    {
        for(colonne=0;colonne<taille_horizontale;colonne++)
        {
            int iteration=0;
            int divergence=0;
            double partie_reelle_depart = ((double)(PLAN_COMPLEXE_LIM_DROITE - PLAN_COMPLEXE_LIM_GAUCHE)*(double) colonne )/ ((double) taille_horizontale) + PLAN_COMPLEXE_LIM_GAUCHE;
            double partie_imaginaire_depart = ((double)(PLAN_COMPLEXE_LIM_BAS-PLAN_COMPLEXE_LIM_HAUT)*(double)ligne)/ ((double) taille_verticale) + PLAN_COMPLEXE_LIM_HAUT;
            double partie_reelle=partie_reelle_depart;
            double partie_imaginaire=partie_imaginaire_depart;
            while(iteration<ITERATION_MAX && divergence==0)
            {
                if(partie_reelle*partie_reelle+partie_imaginaire*partie_imaginaire>=4)
                {
                    divergence=1;
                }
                double save_partie_reelle=partie_reelle;
                partie_reelle=partie_reelle*partie_reelle-partie_imaginaire*partie_imaginaire+partie_reelle_depart;
                partie_imaginaire=2*save_partie_reelle*partie_imaginaire+partie_imaginaire_depart;
                iteration++;
            }
            int couleur_pixel=255-255.0*iteration/ITERATION_MAX;
            fractale_mandelbrott8[ligne][colonne][2] = couleur_pixel;///red
            fractale_mandelbrott8[ligne][colonne][1] = couleur_pixel;///green
            fractale_mandelbrott8[ligne][colonne][0] = couleur_pixel;///blue
        }
    }
    pthread_exit(NULL);
}
void generateBitmapImage (char* imageFileName)
{
    int widthInBytes = taille_horizontale * BYTES_PER_PIXEL;

    unsigned char padding[3] = {167, 167, 167};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;
    int stride = (widthInBytes) + paddingSize;

	printf("width in bytes : %d\npadding size : %d\nstride : %d\ntaille_verticale : %d\ntaille_horizontale : %d\n",widthInBytes,paddingSize,stride,taille_verticale,taille_horizontale);
    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader();
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);
	int l,c,p;
    for(l=taille_verticale-1;l>=0;l--)
    {
        for(c=0;c<taille_horizontale;c++)
        {
            for(p=0;p<BYTES_PER_PIXEL;p++)
            {
                if(l<tab_endline[1])
                {
                    fwrite(&fractale_mandelbrott1[l][c][p],1,1,imageFile);
                }
                else if(l<tab_endline[2])
                {
                    fwrite(&fractale_mandelbrott2[l][c][p],1,1,imageFile);
                }
                else if(l<tab_endline[3])
                {
                    fwrite(&fractale_mandelbrott3[l][c][p],1,1,imageFile);
                }
                else if(l<tab_endline[4])
                {
                    fwrite(&fractale_mandelbrott4[l][c][p],1,1,imageFile);
                }
                else if(l<tab_endline[5])
                {
                    fwrite(&fractale_mandelbrott5[l][c][p],1,1,imageFile);
                }
                else if(l<tab_endline[6])
                {
                    fwrite(&fractale_mandelbrott6[l][c][p],1,1,imageFile);
                }
                else if(l<tab_endline[7])
                {
                    fwrite(&fractale_mandelbrott7[l][c][p],1,1,imageFile);
                }
                else
                {
                    fwrite(&fractale_mandelbrott8[l][c][p],1,1,imageFile);
                }

            }
        }
    }
    if(paddingSize!=0)
    {
        fwrite(padding, 1, paddingSize, imageFile);
    }
    fclose(imageFile);
}


unsigned char* createBitmapFileHeader (int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * taille_verticale);
	printf("file size : %d\n",fileSize);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader()
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(taille_horizontale      );
    infoHeader[ 5] = (unsigned char)(taille_horizontale >>  8);
    infoHeader[ 6] = (unsigned char)(taille_horizontale >> 16);
    infoHeader[ 7] = (unsigned char)(taille_horizontale >> 24);
    infoHeader[ 8] = (unsigned char)(taille_verticale      );
    infoHeader[ 9] = (unsigned char)(taille_verticale >>  8);
    infoHeader[10] = (unsigned char)(taille_verticale >> 16);
    infoHeader[11] = (unsigned char)(taille_verticale >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}

    /*
    int taille_horizontale=0;
    int taille_verticale=5000;
    if(taille_verticale==0)
    {
        taille_verticale=taille_horizontale*(PLAN_COMPLEXE_LIM_HAUT-PLAN_COMPLEXE_LIM_BAS)/(PLAN_COMPLEXE_LIM_DROITE-PLAN_COMPLEXE_LIM_GAUCHE);
    }
    if(taille_horizontale==0)
    {
        taille_horizontale=taille_verticale*((PLAN_COMPLEXE_LIM_DROITE-PLAN_COMPLEXE_LIM_GAUCHE)/(PLAN_COMPLEXE_LIM_HAUT-PLAN_COMPLEXE_LIM_BAS));
    }
    if(taille_horizontale<0)
    {
        taille_horizontale=taille_horizontale*-1;
    }
    if(taille_verticale<0)
    {
        taille_verticale=taille_verticale*-1;
    }
    */


