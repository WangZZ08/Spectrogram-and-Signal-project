#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define PI 3.14159265359

double u(double t, double t1);
double s(int j, double f, size_t n, double Ts);
void writeWavHeader(FILE* file, int fs, int m, double T);

int main() {
    int fs_16k = 16000;
    int fs_8k = 8000;
    int m = 16;
    int a[10] = {100, 2000, 1000, 500, 250, 100, 2000, 1000, 500, 250};
    double f[10] = {0, 31.25, 500, 2000, 6000, 44, 220, 440, 1760, 3960};
    int T = 4;

    double Ts_16k = 1.0 / fs_16k;
    double Ts_8k = 1.0 / fs_8k;
    size_t n;
    size_t N_8k_part = (size_t)(1 * 0.1 * fs_8k); //0.1秒8k頻率總共有多少個取樣點
    size_t N_16k_part = (size_t)(1 * 0.1 * fs_16k); //0.1秒16k頻率總共有多少個取樣點

    double tmp;
    double t;
    double t1;
    double t2;
    int i, j;

    char output_filename_8k[50];
    char output_filename_16k[50];

    const char* folder_8k = "8k";
    const char* folder_16k = "16k";

    if (mkdir(folder_8k, 0777) == -1) {
        perror("Error creating 8k directory");
        return 1;
    }

    if (mkdir(folder_16k, 0777) == -1) {
        perror("Error creating 16k directory");
        return 1;
    }

// 三層的迴圈，同時生成兩個文件的音訊數據
for (j = 0; j <= 3; j++) {
    for (i = 0; i <= 9; i++) {
        //16k
        sprintf(output_filename_16k, "%s/s-16k_%d.wav",folder_16k, j*10+i+1); 
        FILE* file_16k_part = fopen(output_filename_16k, "wb");
        writeWavHeader(file_16k_part, fs_16k, m, 0.1);
        short* x16_16k_part = (short*)malloc(sizeof(short) * N_16k_part);
        //每隔0.1秒將音檔輸入到16k的資料夾內
        for (n = 0; n < N_16k_part; n++) {
            t = n * Ts_16k + 0.1 * (j*10+i) ; // 0.1 * (j*10+i)讓t能每經過一次迴圈增加0.1秒
            t1 = 0.1 * i + j;
            t2 = 0.1 * (i + 1) + j;
            tmp = a[i] * (u(t, t1) - u(t, t2)) * s(j, f[i], n, Ts_16k);
            x16_16k_part[n] = (short)floor(tmp + 0.5);  
        }

        fwrite(x16_16k_part, sizeof(short), N_16k_part, file_16k_part);
        fclose(file_16k_part);
        free(x16_16k_part);

        //8k
        sprintf(output_filename_8k, "%s/s-8k_%d.wav", folder_8k, j*10+i+1); 
        FILE* file_8k_part = fopen(output_filename_8k, "wb");
        writeWavHeader(file_8k_part, fs_8k, m, 0.1);
        short* x16_8k_part = (short*)malloc(sizeof(short) * N_8k_part);
        //每隔0.1秒將音檔輸入到8k的資料夾內
        for (n = 0; n < N_8k_part; n++) {
            t = n * Ts_8k + 0.1 * (j*10+i) ; 
            t1 = 0.1 * i + j;
            t2 = 0.1 * (i + 1) + j;
            tmp = a[i] * (u(t, t1) - u(t, t2)) * s(j, f[i], n, Ts_8k);
            x16_8k_part[n] = (short)floor(tmp + 0.5);
            
        }
        fwrite(x16_8k_part, sizeof(short), N_8k_part, file_8k_part);
        fclose(file_8k_part);
        free(x16_8k_part);
    }
}

return 0;
}

void writeWavHeader(FILE* file, int fs, int m, double T) {
    uint8_t header[44] = {
        'R', 'I', 'F', 'F',
        0, 0, 0, 0,
        'W', 'A', 'V', 'E',
        'f', 'm', 't', ' ',
        16, 0, 0, 0,
        1, 0,
        1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0,
        0, 0,
        'd', 'a', 't', 'a',
        0, 0, 0, 0
    };

    int byteRate = (fs * 1 * m) / 8;
    int subChunk2Size = byteRate * T;
    int ChunkSize = subChunk2Size + 36;

    header[4] = ChunkSize & 0xFF;
    header[5] = (ChunkSize >> 8) & 0xFF;
    header[6] = (ChunkSize >> 16) & 0xFF;
    header[7] = (ChunkSize >> 24) & 0xFF;
    header[24] = fs & 0xFF;
    header[25] = (fs >> 8) & 0xFF;
    header[26] = (fs >> 16) & 0xFF;
    header[27] = (fs >> 24) & 0xFF;
    header[28] = byteRate & 0xFF;
    header[29] = (byteRate >> 8) & 0xFF;
    header[30] = (byteRate >> 16) & 0xFF;
    header[31] = (byteRate >> 24) & 0xFF;
    header[32] = (1 * m) / 8;
    header[34] = m;
    header[40] = subChunk2Size & 0xFF;
    header[41] = (subChunk2Size >> 8) & 0xFF;
    header[42] = (subChunk2Size >> 16) & 0xFF;
    header[43] = (subChunk2Size >> 24) & 0xFF;

    fwrite(header, sizeof(uint8_t), 44, file);
}

double u(double t, double t1) { //unit function
    if (t >= t1) {
        return 1.0;
    } else {
        return 0.0;
    }
}
double s(int j , double f ,size_t n , double Ts)
{
    double tmp = 0 ;
    if(j == 0 ){ // sin
            tmp = sin(2 * PI * f * n * Ts) ; 
        } else if(j == 2){ //square
            if(fmod(n * Ts * f, 1.0) < 0.5){ 
                tmp = 1 ;
            } else{
                tmp = -1  ;
            }       
        } else if(j == 3){ //triangle
            if(fmod(n * Ts * f, 1.0) <= 0.25 ) {
                tmp =  (4 * fmod(n * Ts * f, 1.0));
            } else if(fmod(n * Ts * f, 1.0) <= 0.75) {
                tmp = (-4 * (fmod(n * Ts * f, 1.0) - 0.25) + 1);
            } else {
                tmp =  (4  * (fmod(n * Ts * f, 1.0) - 1.0));
            }
        } else if(j == 1){ //sawtooth
            tmp = 2 *f * n * Ts - 2 *(int)(f * n * Ts) - 1 ;
        }
    return tmp ;
}