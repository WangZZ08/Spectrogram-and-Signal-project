#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#define fs_8k 8000
#define fs_16k 16000

void concatenateFiles( char* folder, char* scpFileName, char* outputFileName,int fs);
void writeWavHeader(FILE* file, int fs, int m, double T) ;
void cascadeWav(char *scpFileName, char *outputFileName,int fs) ;


//後來提交版本僅增加備註
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    char scpFileName[50] ;
    char outputFileName[50] ;
    strcpy(scpFileName, argv[1]);
    strcpy(outputFileName, argv[2]);
    int fs = 0 ;
    //scanf("%s %s",scpFileName,outputFileName);
    
    if(strcmp(outputFileName , "s-8k.wav") == 0 ){
        fs = fs_8k ;
    }else if(strcmp(outputFileName , "s-16k.wav") == 0){
        fs = fs_16k ;
    }
    cascadeWav(scpFileName,outputFileName,fs);

    return 0;
}

//藉由取樣頻率來讀取對應的資料夾
void cascadeWav(char *scpFileName, char *outputFileName,int fs){
    char* folder_8k = "8k";
    char* folder_16k = "16k";
    
    if(fs == fs_8k){
        concatenateFiles(folder_8k, scpFileName, outputFileName,fs_8k);
    }else if(fs == fs_16k){
        concatenateFiles(folder_16k, scpFileName, outputFileName,fs_16k);
    }
       
}

void concatenateFiles(char* folder, char* scpFileName, char* outputFileName,int fs) {
    FILE *output = fopen(outputFileName, "wb");

    if (output == NULL) {
        perror("Error creating output file");
        return;
    }
    writeWavHeader(output, fs, 16, 4.0);

    FILE* scpFile = fopen(scpFileName, "a+");

    if (scpFile == NULL) {
        perror("Error creating SCP file");
        fclose(output);
        return;
    }

    for (int i = 1; i <= 40; i++) {
        char inputFileName[50];
        
        //每經過一次迴圈抓取資料夾內對應的.wav檔
        if(fs == fs_8k){
            sprintf(inputFileName, "%s/s-8k_%d.wav", folder, i); 
        }else if(fs == fs_16k){
            sprintf(inputFileName, "%s/s-16k_%d.wav", folder, i);  

        }

        FILE* inputFile = fopen(inputFileName, "rb");

        if (inputFile == NULL) {
            perror("Error opening input file");
            fclose(output);
            fclose(scpFile);
            return;
        }

        
        fseek(inputFile, 44, SEEK_SET);  // Skip the WAVE header
        
        // Read and write file content
        while (!feof(inputFile)) {
            int16_t sample;
            fread(&sample, sizeof(int16_t), 1, inputFile);
            fwrite(&sample, sizeof(int16_t), 1, output);
        }

        if(fs == fs_8k){
            fprintf(scpFile, "s-8k_%d.wav\n", i);   // 將檔名寫入scp.txt   
        }else if(fs == fs_16k){
            fprintf(scpFile, "s-16k_%d.wav\n", i);
        }

        fclose(inputFile);
    }

    fclose(output);
    fclose(scpFile);
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