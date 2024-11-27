#!/bin/bash
#!/usr/bin/env python3
gcc -o sinegen sinegen.c 
./sinegen

gcc -o cascade cascade.c 
./cascade scp.txt s-8k.wav
./cascade scp.txt s-16k.wav

gcc -o spectrogram spectrogram.c
window_sizes=(32 32 30 30)
window_types=("rectangular" "hamming" "rectangular" "hamming")
dft_sizes=32
frame_intervals=10
input_files=("s-8k.wav" "s-16k.wav" "aeueo-8kHz.wav" "aeueo-16kHz.wav")
output_files=("s-8k.Set1.txt" "s-8k.Set2.txt" "s-8k.Set3.txt" "s-8k.Set4.txt" "s-16k.Set1.txt" "s-16k.Set2.txt" "s-16k.Set3.txt" "s-16k.Set4.txt" "aeueo-8kHz.Set1.txt" "aeueo-8kHz.Set2.txt" "aeueo-8kHz.Set3.txt" "aeueo-8kHz.Set4.txt" "aeueo-16kHz.Set1.txt" "aeueo-16kHz.Set2.txt" "aeueo-16kHz.Set3.txt" "aeueo-16kHz.Set4.txt")

for i in {0..3};do
	for j in {0..3};do
		./spectrogram ${window_sizes[j]} ${window_types[j]} ${dft_sizes} ${frame_intervals} ${input_files[i]} ${output_files[4*i+j]}
	done
done

input_wav=("s-8k.wav" "s-16k.wav" "aeueo-8kHz.wav" "aeueo-16kHz.wav")
input_txt=("s-8k.Set1.txt" "s-8k.Set2.txt" "s-8k.Set3.txt" "s-8k.Set4.txt" "s-16k.Set1.txt" "s-16k.Set2.txt" "s-16k.Set3.txt" "s-16k.Set4.txt" "aeueo-8kHz.Set1.txt" "aeueo-8kHz.Set2.txt" "aeueo-8kHz.Set3.txt" "aeueo-8kHz.Set4.txt" "aeueo-16kHz.Set1.txt" "aeueo-16kHz.Set2.txt" "aeueo-16kHz.Set3.txt" "aeueo-16kHz.Set4.txt")
output_pdf=("s-8k.Set1.pdf" "s-8k.Set2.pdf" "s-8k.Set3.pdf" "s-8k.Set4.pdf" "s-16k.Set1.pdf" "s-16k.Set2.pdf" "s-16k.Set3.pdf" "s-16k.Set4.pdf" "aeueo-8kHz.Set1.pdf" "aeueo-8kHz.Set2.pdf" "aeueo-8kHz.Set3.pdf" "aeueo-8kHz.Set4.pdf" "aeueo-16kHz.Set1.pdf" "aeueo-16kHz.Set2.pdf" "aeueo-16kHz.Set3.pdf" "aeueo-16kHz.Set4.pdf")

for i in {0..3};do
        for j in {0..3};do
	                python3 spectshow.py ${input_wav[i]} ${input_txt[i*4+j]} ${output_pdf[i*4+j]}
	done
done





