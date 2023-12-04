//IMPORTING ESSESNTIAL LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>



#define EDGE_THRES 100
#define pi 3.141592654

int Image_Process(char src[50]);
void Greyscale(unsigned char *img_matrix, unsigned char * grey_matrix, int img_size);
int * G_blur(unsigned char *grey_matrix, int img_h, int img_w);
int* Sobble(int * blur_matrix, int img_h, int img_w);
int * Canny(int *sobble_matrix, double * orient, int img_h, int img_w);
int * Hys_Thres(int *cany_matrix, int h_thres, int l_thres, int img_h, int img_w);
int* Patch(int* sobble_matrix, int img_h, int img_w, int patch_s );


int main(){
	char src[50];

	//READING IMG PATH FROM TERMINAL
	printf("ENTER PATH TO IMAGE FILE:");
	scanf("%s", &src);
	//strcpy(src, "images\\color\\lena_color.bmp");

	//CALLING THE IMG PROCESSING FUNCTION
	Image_Process(src);


}

//CHANGES COLOR IMAGE DATA INTO GREYSCALE FOR FURTHER PROCESSING
void Greyscale(unsigned char * img_data, unsigned char * grey_matrix, int img_size){
	for(int i  = 0; i <img_size;i++){
		unsigned char I = __max(img_data[3*i],img_data[3*i+1]);
		I = __max(I, img_data[3*i+2]);
		grey_matrix[i] = I;
	}

}

//APPLIES GAUSIAN BLUR TO GREYSCALE DATA FOR FURTHER PROCESSING
int * G_blur(unsigned char * grey_matrix, int img_h, int img_w){
	int img_size = img_h*img_w;
	int * blur_matrix = (void*)malloc(sizeof(int)*img_size);

	int blur_size = 3;
	float v = 1.0;
	float sum_v = 0.0;
	float kernel[blur_size][blur_size];
	for(int i = 0; i<blur_size;i++)
		for(int j = 0; j<blur_size;j++){
			int val = abs(i-blur_size/2) + abs(j-blur_size/2);
			sum_v += v/pow(2,val);
			kernel[i][j] = v/pow(2,val);
		}
	for(int i = 0;i<img_size;i++){
		float s = 0.0;
		//DETECTS THAT PIXEL IS NOT ON BORDERS
		if(i>img_w && i < (img_size-img_w) && (i%img_w)%(img_w-1)){
			for(int j = 0; j<3; j++){
				for(int k = 0; k<3;k++){
					int idx = i + img_w*(j-1) + (k-1);
					s+= (float)grey_matrix[idx]*kernel[j][k];
				}
			}
			s = s/sum_v;

		}
		else{
			s = grey_matrix[i];
		}
		blur_matrix[i] = s;

	}
	return blur_matrix;
	
}

int* Hys_Thres(int *canny_matrix, int h_thres, int l_thres, int img_h, int img_w){
	int img_size = img_h*img_w;
	int* hys_matrix = (void*)malloc(sizeof(int)*img_size);
	for(int i = 0; i < img_size; i++){
		if(canny_matrix[i]>h_thres){
			hys_matrix[i] = canny_matrix[i];
		}
		else if(canny_matrix[i]<l_thres){
			hys_matrix[i] = 0;
		}
		else{
			hys_matrix[i] = 0;
			for(int j = 0; j<3; j++){
				for(int k = 0; k<3;k++){
					int idx = i + img_w*(j-1) + (k-1);
					if(canny_matrix[idx]>h_thres){
						hys_matrix[i] = canny_matrix[i];
					}
				}
			}

		}

		}
		return hys_matrix;
	}



int * Canny(int *sobble_matrix, double * orient, int img_h, int img_w){
	int img_size = img_h*img_w;
	int* canny_matrix = (void*)malloc(sizeof(int)*img_size);
	for(int i = 0;i<img_size;i++){
		if(sobble_matrix[i]>0){
			canny_matrix[i] = 0;
			if(i>img_w && i < (img_size-img_w) && (i%img_w)%(img_w-1)){
				if(orient[i]>-1*pi/6 && orient[i]< pi/6){
					int id1 = i-img_w;
					int id2 =  i+img_w;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
				else if(orient[i]>-1*pi/3 && orient[i]< -1*pi/6){
					int id1 = i-img_w+1;
					int id2 =  i+img_w-1;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
				else if(orient[i]<pi/3 && orient[i]>pi/6){
					int id1 = i-img_w-1;
					int id2 =  i+img_w+1;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
				else{
					int id1 = i-1;
					int id2 =  i+1;
					if(sobble_matrix[i]>__max(sobble_matrix[id1],sobble_matrix[id2])){
						canny_matrix[i] = sobble_matrix[i];
					}
				}
			}
		}
	}
	return canny_matrix;
}

int * Sobble(int * blur_matrix, int img_h, int img_w){
	int img_size = img_h*img_w;
	int * sobble_matrix = (void*)malloc(sizeof(int)*img_size);
	double * gy = (void*)malloc(sizeof(double)*img_size);
	double * gx = (void*)malloc(sizeof(double)*img_size);
	double * orient = (void*)malloc(sizeof(double)*img_size);

	float v = 1.0; 
	float ker_x[3][3] = {{-1,0,1},
					  {-2,0,2},
				   	  {-1,0,1}};
	float ker_y[3][3] = {{-1,-2,-1},
					  {0,0,0},
				   	  {1,2,1}};

	for(int i = 0;i<img_size;i++){
		float sx = 0.1, sy = 0.1;
		//DETECTS THAT PIXEL IS NOT ON BORDERS
		if(i>img_w && i < (img_size-img_w) && (i%img_w)%(img_w-1)){
			for(int j = 0; j<3; j++){
				for(int k = 0; k<3;k++){
					int idx = i + img_w*(j-1) + (k-1);
					sx+= (float)blur_matrix[idx]*ker_x[j][k];
					sy+= (float)blur_matrix[idx]*ker_y[j][k];
				}
			}

		}
		
		gx[i] = sx;
		gy[i] = sy;
		
		sobble_matrix[i] = pow((sx*sx+sy*sy), 0.5);
		orient[i] = atan(gx[i]/gy[i]);
		

	//APPLYING  CANNY OPERATOR AND HYSTERESIS THRESHOLDING
		
	}
	int * canny_matrix = Canny(sobble_matrix, orient, img_h, img_w);
	int * hys_matrix = Hys_Thres(canny_matrix,100,50, img_h,img_w );
	return hys_matrix;
}

//MAKING PATCHES AND FINDING PATCHES HAVING EDGES
int* Patch(int* sobble_matrix, int img_h, int img_w, int patch_s ){
	int n = (img_h*img_w)/(patch_s*patch_s);
	int *patch_matrix = (int*)malloc(sizeof(int)*n);
	int img_size = img_h*img_w;
	for(int i = 0; i< img_size;i++){
			int x = (i%img_w)/patch_s;
			int y = (i/img_w)/patch_s;
			int pn = y*(img_w/patch_s)+x;

			if(patch_matrix[pn]<0)patch_matrix[pn]=0;
			if(sobble_matrix[i]>0) patch_matrix[pn] += 1;
	
	}
	return patch_matrix;
}

int Image_Process(char src[50]){
	FILE *fin, *fout, *fblur;
	fin = fopen(src, "rb");
	fout = fopen("results\\image_o.bmp", "wb");

	//READ IMAGE HEADER AND ASSIGN H,W,BITDEPTH
	unsigned char header[54];				
	fread(header, sizeof(unsigned char), 54, fin);
	fwrite(header, sizeof(unsigned char), 54, fout);


	int img_h = *(int *)&header[22];
	int img_w = *(int *)&header[18];
	int img_size = img_h*img_w;
	int bitDepth = *(int *)&header[28];

	//FINDING IF IT HAS A COLOR TABLE
	//IF BITDEPTH <= 8 COLOR TABLE EXISTS
	printf("\nHEIGHT-%d WIDTH-%d\nENTER PATCH SIZE:",img_h, img_w);
	if(bitDepth==0){
		printf("ERROR");
		exit(0);
	}
	int patch_s;
	scanf("%d", &patch_s);
	while(img_h%patch_s!=0 && img_w%patch_s!=0){
		printf("ENTER VALID PATCH SIZE:");
		scanf("%d", &patch_s);
	}
	if(bitDepth == 8){
		unsigned char colorTable[1024];
		fread(colorTable, sizeof(unsigned char), 1024, fin);
		fwrite(colorTable, sizeof(unsigned char), 1024, fout);
	}

	//BITDEPTH = 8 => GREYSCALE
	if(bitDepth <= 8){
		unsigned char *img_data ;
		img_data = (void*)malloc(img_size*sizeof(unsigned char));
		int *img_matrix = (void*)malloc(img_size*sizeof(int));
		for(int i = 0; i <img_size; i++){
			img_data[i] = fgetc(fin);
			int x = *(int*)&img_data[i];
			img_matrix[i] = x;
		}
		int *blur_matrix = G_blur(img_data, img_h, img_w);
		int *sobble_matrix = Sobble(blur_matrix, img_h, img_w);
		
		int *patch_matrix = Patch(sobble_matrix, img_h, img_w, patch_s);

		for(int i  = 0; i <img_size;i++){
			//CALCULATING PATCH
			int x = (i%img_w)/patch_s;
			int y = (i/img_w)/patch_s;
			
			if(((i)/img_w)%patch_s>1 && i%patch_s>1){
				continue;
			
			}
			else{
			int pn = y*(img_w/patch_s)+x;
			//printf("%d\n", patch_matrix[pn]);
			if(patch_matrix[pn]>8){
				img_matrix[i] = 0;
			}

			
			}
		}
		for(int i = 0; i <img_size;i++){
			fputc(img_matrix[i],fout);

		}
		printf("\nIMAGE HAS BEEN ANALYSED AND RESULT GENERATED");


	}

	//BITDEPTH = 24 => COLOR
	else if(bitDepth == 24){
		unsigned char *img_data = (void *)malloc(sizeof(int*)*3*img_size);
		unsigned char *grey_data = (void *)malloc(sizeof(int*)*img_size);
		int *img_matrix, *grey_matrix;
		img_matrix = (void *)malloc(sizeof(int*)*3*img_size);
		grey_matrix = (void *)malloc(sizeof(int*)*img_size);

		//READING FROM INPUT IMAGE TO IMG_DAT AND IMG_MATRIX
		for(int i  = 0; i <img_size;i++){
			img_data[3*i+2] = fgetc(fin);
			img_data[3*i+1] = fgetc(fin);
			img_data[3*i] = fgetc(fin);

			img_matrix[3*i] = *(int*)&img_data[3*i];
			img_matrix[3*i+1] = *(int*)&img_data[3*i+1];
			img_matrix[3*i+2] = *(int*)&img_data[3*i+2];

		}

		//MAKING A GREYSCALE IMAGE MATRIX
		Greyscale(img_data, grey_data, img_size);

		//APPLYING GAUSSIAN BLUR TO GREYSCALE IMG
		int *blur_matrix = G_blur(grey_data, img_h, img_w);

		//APPLYING SOBBLE OPERATOR TO BLURRED IMG
		int *sobble_matrix = Sobble(blur_matrix, img_h, img_w);

		//PATCHS?
		
		int *patch_matrix = Patch(sobble_matrix, img_h, img_w, patch_s);
		//WRITING INTO OUTPUT IMAGE
		for(int i  = 0; i <img_size;i++){
			//CALCULATING PATCH
			int x = (i%img_w)/patch_s;
			int y = (i/img_w)/patch_s;
			
			if(((i)/img_w)%patch_s>1 && i%patch_s>1){
				continue;
			
			}
			else{
			int pn = y*(img_w/patch_s)+x;

			if(patch_matrix[pn]>8){
				img_matrix[3*i+2] = 255;
				img_matrix[3*i+1]= 0;
				img_matrix[3*i] = 0;
			}
			else{
				if(img_matrix[3*i+2] != 255){
					img_matrix[3*i+2] = 0;
					img_matrix[3*i+1]= 0;
					img_matrix[3*i] = 0;
				}
			}
			
			}
		}
		for(int i = 0; i <img_size;i++){
			fputc(img_matrix[3*i+2],fout);
			fputc(img_matrix[3*i+1], fout);
			fputc(img_matrix[3*i],fout);
		}
	printf("\nIMAGE HAS BEEN ANALYSED AND RESULT GENERATED");
	
	}
	fclose(fin);
	fclose(fout);
}

