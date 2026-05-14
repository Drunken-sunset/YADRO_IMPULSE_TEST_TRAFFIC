import pandas as pd
import matplotlib.pyplot as plt
import sys
from pathlib import Path

def process(file_path):
    data = pd.read_csv(file_path)
    if data.empty: return

    # Расчет задержек (diff между метками времени)
    data['Delay'] = data['Time'].diff().fillna(data['Time'].iloc[0])
    
    model_name = data['ModelName'].iloc[0]
    avg_size = data['Size'].mean()
    avg_delay = data['Delay'].mean()
    
    # Битрейт: (сумма байт * 8 бит) / общее время
    total_time = data['Time'].iloc[-1]
    bitrate = (data['Size'].sum() * 8) / total_time if total_time > 0 else 0

    print(f"\n--- Statistics for {model_name} ---")
    print(f"Average Packet Size: {avg_size:.2f} bytes")
    print(f"Average Delay:       {avg_delay:.4f} s")
    print(f"Average Bitrate:     {bitrate:.2f} bps")

    # Графики
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4))
    
    ax1.hist(data['Delay'], bins=30, color='teal', alpha=0.7)
    ax1.set_title('Delays Distribution')
    
    ax2.hist(data['Size'], bins=30, color='indianred', alpha=0.7)
    ax2.set_title('Size Distribution')
    
    plt.savefig('metrics.png')
    plt.show()

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "output.csv"
    process(path)
