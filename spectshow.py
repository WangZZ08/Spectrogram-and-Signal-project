import sys
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import wave

if len(sys.argv) != 4:
    print("Usage: python3 spectshow.py <input_wav> <input_txt> <output_pdf>")
    sys.exit(1)

in_wav, in_text, out_pdf = sys.argv[1], sys.argv[2], sys.argv[3]
wave_file = wave.open(in_wav, 'rb')
params = wave_file.getparams()
num_frames = params.nframes
audio_data = np.frombuffer(wave_file.readframes(num_frames), dtype=np.int16)
wave_file.close()

# 讀取文本數據，將空字符串填充為 NaN
text_data = np.genfromtxt(in_text, dtype=float, delimiter='\t')
#因在spectrogram.c用"\n"做隔開，要扣除最後一行"\n"才是正確的data行數
text_data = text_data[:, :-1]

# 產生繪圖物件
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(15, 8), dpi=72, sharex=True)


# 時刻 - 振幅關係圖
t = np.linspace(0, len(audio_data) / params.framerate, len(audio_data))
normalized_audio_data = audio_data / 32767.0  # 歸一化到 [-1, 1]
ax1.plot(t, normalized_audio_data, linestyle='-', color='darkgreen')
ax1.set_ylabel('normalized amplitude', fontsize=14)



# 頻譜圖
if text_data.shape[1] < 130 :
    freq_range = np.linspace(0, 4000, text_data.shape[1])  # Update frequency range
else :
    freq_range = np.linspace(0, 8000, text_data.shape[1])  # Update frequency range
time_points = np.linspace(0, len(audio_data) / params.framerate, text_data.shape[0])
im = ax2.pcolormesh(time_points, freq_range, text_data.T, cmap='plasma', shading='auto')
ax2.set_xlabel('time (s)', fontsize=14)
ax2.set_ylabel('frequency (Hz)', fontsize=14)
ax2.set_facecolor('gray')

plt.savefig(out_pdf)