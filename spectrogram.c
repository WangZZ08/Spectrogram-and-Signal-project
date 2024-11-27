#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdint.h>
#define PI 3.14159265359 

void generateRectangular(double w[],int P ,int N);
void generateHamming(double w[], int P,int N);

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Usage: %s <w_size> <w_type> <dft_size> <f_itv> <wav_in> <spec_out>\n", argv[0]);
        return 1;
    }
    
    int w_size = atoi(argv[1]); //一次取下來有window size秒
    char w_type[50] ;
    int dft_size = atoi(argv[3]) ; //FFT windeow size
    int f_itv = atoi(argv[4]) ;//frame interval
    char wav_in[50] ;
    char spec_out[50]  ;
    strncpy(w_type, argv[2], sizeof(w_type) - 1);
    w_type[sizeof(w_type) - 1] = '\0';  
    strncpy(wav_in, argv[5], sizeof(wav_in) - 1);
    wav_in[sizeof(wav_in) - 1] = '\0';  
    strncpy(spec_out, argv[6], sizeof(spec_out) - 1);
    spec_out[sizeof(spec_out) - 1] = '\0';  

    int fs ;
    double T ;
    double exp_cos[512] = {0};
    double exp_sin[512] = {0};
    double value ;
     
    int k ;//frequency index
    int i , j ,n ;
    double tmp = 0.0 ;
    double magnitude ;
    double dB ;

    //scanf("%d %s %d %d %s %s", &w_size,w_type,&dft_size,&f_itv,wav_in,spec_out);

    if(strcmp(wav_in , "s-8k.wav") == 0){
        fs = 8000 ;
        T = 4.0 ;
    }else if(strcmp(wav_in , "s-16k.wav")== 0 ){
        fs = 16000 ;
        T = 4.0 ;
    }else if(strcmp(wav_in , "aeueo-8kHz.wav")== 0){
        fs = 8000 ;
        T = 16.0 ;
    }else if(strcmp(wav_in , "aeueo-16kHz.wav")== 0){
        fs = 16000 ;
        T = 16.0 ;
    }

    size_t P = fs * w_size /1000 ;//該時間下有P個取樣點
    size_t N = fs * dft_size /1000 ;//該時間下FFT有N個取樣點
    size_t frame_size = fs * f_itv / 1000 ;//frame下有幾個取樣點
    size_t total_frame = T * 1000 / f_itv ;//總共有幾個frame
    size_t total_sample_point = (size_t)(1 * T * fs);//總共有幾點

    FILE *wave = fopen(wav_in , "rb");
    if(wave == NULL){
        perror("Error creating output file");
        return 0 ;
    }
    
    FILE *output = fopen(spec_out , "w");
    if(output == NULL){
        perror("Error creating output file");
        return 0 ;
    }
    
    fseek(wave , 44, SEEK_SET);
    // 取得每個取樣點振幅
    short* sample_point = (short*)malloc(sizeof(short) * total_sample_point);
    size_t num = 0;
    int16_t sample;

    while (fread(&sample, sizeof(short), 1, wave) == 1) {
    //printf("Amplitude: %d\n", sample);
    sample_point[num] = sample;
    num++;
    // 檢查是否超過最大樣本數
        if (num >= total_sample_point) {
            //printf("Reached maximum sample count.\n");
            break;
        }
    }

    double* w = (double*)malloc(sizeof(double) * N );
    double** x = (double**)malloc(sizeof(double*) * total_frame);
    double* X_re = (double*)malloc(sizeof(double) * (N/2+1));
    double* X_im = (double*)malloc(sizeof(double) * (N/2+1));
    for ( i = 0; i < total_frame; i++) {
        x[i] = (double *)malloc(N * sizeof(double));
    }
    for(i = 0; i < total_frame; i++){
        for(j = 0; j < N ; j++){
            x[i][j] = 0.0 ;
        }
    }

    if(strcmp(w_type , "rectangular") == 0){
        generateRectangular(w,P,N);
    }else if(strcmp(w_type , "hamming") == 0){
        generateHamming(w,P,N);
    }

    //第i偵frame裡的每一點經過filter後存放在x[i][n]
    for(i = 0 ; i< total_frame ;i++){
        for( n=0 ; n< N ;n++ ){
            x[i][n] =  sample_point[i * frame_size + n] * w[n] ; //f_itv = M 
        }       
    }
    //FFT
    for(n=0;n<N;n++){
	    exp_cos[n] = cos(2.0 * PI / N  * n); //先計算exp後的值減少計算複雜度
        exp_sin[n] = sin(2.0 * PI / N  * n);
    }
    for(i = 0 ; i< total_frame ;i++){
	    for(k=0;k<(N/2+1);k++) {
	        X_re[k] = 0;
	        X_im[k] = 0;
	        for(n=0;n<N;n++) {	    
		    tmp = x[i][n] * exp_cos[(n*k)% N] ;
		    X_re[k] += tmp;
		    tmp =  x[i][n] * -exp_sin[(n*k)%N];
		    X_im[k] += tmp;
	        }
            X_re[k] /= N; //normalize
		    X_im[k] /= N; //normalize
    	}

    
    	//寫入資訊
    	for(k = 0; k < (N/2+1); k ++ ) {  
	    magnitude = 20.0 * log10(X_re[k] * X_re[k] + X_im[k] * X_im[k] );
	    if(magnitude < 0)
		magnitude = 0.0 ;
		
	    fprintf(output, "%.15f\t", magnitude);
	    fflush(output);
	}
    fprintf(output,"\n");    
    }   

    free(sample_point);
    free(w);
    for ( i = 0; i < total_frame; i++) {
        free(x[i]) ;     
    }
    free(x);
    free(X_re);
    free(X_im);
    fclose(wave);
    fclose(output);
    return 0 ;
}

void generateRectangular(double w[],int P ,int N) {
    for (int n = 0; n < N; n++) {
        if(n < P)
            w[n] = 1.0 ;
        else
            w[n] = 0.0 ;   
    }       
}

void generateHamming(double w[],int P,int N) {
    for (int n = 0; n < N; n++) {
        if(n < P)
            w[n] = 0.54 - 0.46 * cos(2 * PI * n / (P - 1));
        else
            w[n] = 0.0 ;   
    }
}