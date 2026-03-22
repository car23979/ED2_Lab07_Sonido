import numpy as np
from scipy.io import wavfile
import os

def convert_wav_to_c_header(input_file, output_header, array_name="audio_data"):
    # 1. Cargar el archivo .wav
    sampling_rate, data = wavfile.read(input_file)
    
    # 2. Convertir a Mono si es estéreo
    if len(data.shape) > 1:
        data = data.mean(axis=1)
        print("-> Convertido de Estéreo a Mono")

    # 3. Normalizar y convertir a 8 bits (0 - 255)
    # El DAC del STM32 en modo 8 bits espera valores unsigned de 0 a 255
    data_min = np.min(data)
    data_max = np.max(data)
    
    # Re-escalar datos al rango 0-255
    normalized_data = ((data - data_min) / (data_max - data_min) * 255).astype(np.uint8)
    
    # 4. Generar el archivo .h
    with open(output_header, 'w') as f:
        f.write(f"#ifndef AUDIO_DATA_H\n")
        f.write(f"#define AUDIO_DATA_H\n\n")
        f.write(f"#include <stdint.h>\n\n")
        f.write(f"// Audio: {input_file}\n")
        f.write(f"// Frecuencia original: {sampling_rate} Hz\n")
        f.write(f"// Tamaño: {len(normalized_data)} muestras\n")
        f.write(f"#define AUDIO_SIZE {len(normalized_data)}\n\n")
        f.write(f"const uint8_t {array_name}[AUDIO_SIZE] = {{\n    ")
        
        # Escribir los datos en formato hexadecimal para ahorrar espacio visual
        for i, val in enumerate(normalized_data):
            f.write(f"0x{val:02x}")
            if i < len(normalized_data) - 1:
                f.write(", ")
            if (i + 1) % 12 == 0:
                f.write("\n    ")
                
        f.write(f"\n}};\n\n")
        f.write(f"#endif // AUDIO_DATA_H\n")

    print(f" ¡Éxito! Archivo '{output_header}' generado.")
    print(f" Tamaño total en Flash: {len(normalized_data) / 1024:.2f} KB")

# --- CONFIGURACIÓN ---
# Asegúrate de que tu archivo wav sea de 8000Hz para que no pese demasiado
input_wav = "ben10_theme.wav" 
output_h = "audio_data.h"
convert_wav_to_c_header(input_wav, output_h)