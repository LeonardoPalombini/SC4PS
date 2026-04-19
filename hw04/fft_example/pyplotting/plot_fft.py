#!/usr/bin/env python3
"""
plot_fft.py  —  Plot the FFT spectrum produced by fft_example.

Usage:
    python plot_fft.py [fft_output.csv] [--max-freq 500] [--no-phase] [--save out.png]

Arguments:
    csv_file        Path to the CSV file (default: fft_output.csv)
    --max-freq HZ   Truncate the x-axis at this frequency (default: full range)
    --no-phase      Do not plot the phase spectrum
    --save FILE     Save the figure to a file instead of displaying it
"""

import argparse
import sys
from pathlib import Path

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


# ── CLI ───────────────────────────────────────────────────────────────────────

def parse_args():
    parser = argparse.ArgumentParser(
        description="Plot FFT spectrum from a CSV produced by fft_example."
    )
    parser.add_argument(
        "csv_file",
        nargs="?",
        default="fft_output.csv",
        help="Path to the FFT CSV file (default: fft_output.csv)",
    )
    parser.add_argument(
        "--max-freq",
        type=float,
        default=None,
        metavar="HZ",
        help="Upper frequency limit for the x-axis (Hz)",
    )
    parser.add_argument(
        "--no-phase",
        action="store_true",
        help="Omit the phase subplot",
    )
    parser.add_argument(
        "--save",
        metavar="FILE",
        default=None,
        help="Save the figure to this path (e.g. spectrum.png) instead of showing it",
    )
    return parser.parse_args()


# ── Helpers ───────────────────────────────────────────────────────────────────

def find_peaks(frequencies, magnitudes, n=5, min_magnitude=1e-6):
    """Return indices of the top-n magnitude peaks (simple local-max filter)."""
    from scipy.signal import find_peaks as sp_find_peaks
    # Use scipy if available, otherwise fall back to argsort
    try:
        indices, _ = sp_find_peaks(magnitudes, height=min_magnitude, distance=3)
        if len(indices) == 0:
            raise ValueError
        order = np.argsort(magnitudes[indices])[::-1]
        return indices[order[:n]]
    except Exception:
        order = np.argsort(magnitudes)[::-1]
        return order[:n]


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    args = parse_args()
    csv_path = Path(args.csv_file)

    if not csv_path.exists():
        print(f"Error: file not found: {csv_path}", file=sys.stderr)
        sys.exit(1)

    # ── Load data ─────────────────────────────────────────────────────────────
    df = pd.read_csv(csv_path)
    required = {"frequency_hz", "magnitude", "phase_rad"}
    if not required.issubset(df.columns):
        print(
            f"Error: CSV must contain columns {required}. "
            f"Found: {set(df.columns)}",
            file=sys.stderr,
        )
        sys.exit(1)

    freq = df["frequency_hz"].to_numpy()
    mag  = df["magnitude"].to_numpy()
    phase = df["phase_rad"].to_numpy()

    # ── Frequency mask ────────────────────────────────────────────────────────
    if args.max_freq is not None:
        mask = freq <= args.max_freq
        freq, mag, phase = freq[mask], mag[mask], phase[mask]

    nyquist     = freq[-1]
    freq_res    = freq[1] - freq[0] if len(freq) > 1 else 0.0
    n_bins      = len(freq)

    print(f"Loaded  : {csv_path}  ({n_bins} bins)")
    print(f"Range   : 0 – {nyquist:.1f} Hz  |  resolution: {freq_res:.4f} Hz/bin")

    # ── Detect peaks ──────────────────────────────────────────────────────────
    peak_idx = find_peaks(freq, mag, n=5)
    print("\nTop spectral peaks:")
    print(f"  {'Frequency (Hz)':>16}  {'Magnitude':>12}  {'Phase (rad)':>12}")
    for i in peak_idx:
        print(f"  {freq[i]:>16.4f}  {mag[i]:>12.6f}  {phase[i]:>12.6f}")

    # ── Figure layout ─────────────────────────────────────────────────────────
    n_rows = 1 if args.no_phase else 2
    fig, axes = plt.subplots(
        n_rows, 1,
        figsize=(12, 4 * n_rows),
        sharex=True,
        gridspec_kw={"hspace": 0.35},
    )
    if n_rows == 1:
        axes = [axes]

    fig.suptitle(
        f"FFT Spectrum  —  {csv_path.name}",
        fontsize=14,
        fontweight="bold",
        y=1.01 if n_rows == 1 else 0.98,
    )

    # ── Magnitude plot ────────────────────────────────────────────────────────
    ax_mag = axes[0]
    ax_mag.fill_between(freq, mag, alpha=0.18, color="steelblue")
    ax_mag.plot(freq, mag, color="steelblue", linewidth=1.0, label="Magnitude")

    # annotate peaks
    for i in peak_idx:
        ax_mag.axvline(freq[i], color="tomato", linewidth=0.8, linestyle="--", alpha=0.7)
        ax_mag.annotate(
            f"{freq[i]:.1f} Hz",
            xy=(freq[i], mag[i]),
            xytext=(6, 4),
            textcoords="offset points",
            fontsize=8,
            color="tomato",
        )

    ax_mag.set_ylabel("Amplitude", fontsize=11)
    ax_mag.set_title("Magnitude Spectrum", fontsize=11)
    ax_mag.yaxis.set_major_formatter(ticker.FormatStrFormatter("%.3f"))
    ax_mag.grid(True, linestyle=":", linewidth=0.6, alpha=0.7)
    ax_mag.set_xlim(left=0)

    # ── Phase plot ────────────────────────────────────────────────────────────
    if not args.no_phase:
        ax_ph = axes[1]

        # only show phase where magnitude is significant (> 1 % of peak)
        threshold = 0.01 * mag.max()
        phase_masked = np.where(mag >= threshold, phase, np.nan)

        ax_ph.scatter(
            freq, phase_masked,
            s=3, color="darkorange", alpha=0.7, label="Phase",
        )
        ax_ph.set_ylabel("Phase (rad)", fontsize=11)
        ax_ph.set_title("Phase Spectrum  (shown where magnitude > 1 % of peak)", fontsize=11)
        ax_ph.set_yticks([-np.pi, -np.pi / 2, 0, np.pi / 2, np.pi])
        ax_ph.set_yticklabels(["-π", "-π/2", "0", "π/2", "π"])
        ax_ph.set_ylim(-np.pi - 0.3, np.pi + 0.3)
        ax_ph.grid(True, linestyle=":", linewidth=0.6, alpha=0.7)
        ax_ph.set_xlabel("Frequency (Hz)", fontsize=11)

    axes[-1].set_xlabel("Frequency (Hz)", fontsize=11)

    plt.tight_layout()

    # ── Output ────────────────────────────────────────────────────────────────
    if args.save:
        fig.savefig(args.save, dpi=150, bbox_inches="tight")
        print(f"\nFigure saved to '{args.save}'.")
    else:
        plt.show()


if __name__ == "__main__":
    main()