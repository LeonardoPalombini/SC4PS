import pandas as pd
import matplotlib.pyplot as plt
import argparse

def process_and_plot_csv(input_filename, output_png_filename, ll):
    # Read the CSV file into a pandas DataFrame
    try:
        df = pd.read_csv(input_filename)
    except FileNotFoundError:
        print(f"Error: The file '{input_filename}' was not found.")
        return
    except Exception as e:
        print(f"Error reading the file: {e}")
        return

    required_columns = {'x', 'fwd128', 'fwd32', 'bkw32'}
    if not required_columns.issubset(df.columns):
        print(f"Error: CSV is missing one of the required columns: {required_columns}")
        return

    df['fwd_diff'] = (df['fwd32'] - df['fwd128']).abs()
    df['bkw_diff'] = (df['bkw32'] - df['fwd128']).abs()

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6), sharex=True)

    ax1.plot(df['x'], df['fwd_diff'], color='blue', label='|fwd32 - fwd128|', linewidth=1.5)
    ax1.set_yscale('log')
    ax1.set_title('Forward Difference: Exact VS Float L='+str(ll))
    ax1.set_xlabel('x')
    ax1.set_ylabel('Absolute Difference')
    ax1.grid(True, linestyle='--', alpha=0.6)
    ax1.legend()

    ax2.plot(df['x'], df['bkw_diff'], color='red', label='|bkw32 - fwd128|', linewidth=1.5)
    ax2.set_yscale('log')
    ax2.set_title('Backward Difference: Exact VS Float L='+str(ll))
    ax2.set_xlabel('x')
    ax2.set_ylabel('Absolute Difference')
    ax2.grid(True, linestyle='--', alpha=0.6)
    ax2.legend()

    plt.tight_layout()

    plt.savefig(output_png_filename, dpi=300)
    plt.close()
    
    print(f"Success! Processed data and saved plot to '{output_png_filename}'.")


# ============================================================

parser = argparse.ArgumentParser()
parser.add_argument(
    '--file', 
    type=str, 
    help="File to plot"
)
args = parser.parse_args()
filename = args.file

name_part = filename.split('.')[0]
number_string = name_part.split('_')[-1]
ll = int(number_string)

process_and_plot_csv(filename, 'difference_plots.png', ll)