#!/usr/bin/env python3
"""
Plot f-k parameter map with spatial variation as color
This script creates a static visualization of the Gray-Scott parameter space.
"""

import numpy as np
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend for script execution
import matplotlib.pyplot as plt
import os
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'visualization'))
import fk_analysis


def plot_curve_simple(G, klim, flim, ax, color='crimson', linewidth=2, label=None, n=200):
    """Plot a curve defined by G(f, k) = 0"""
    k = np.linspace(klim[0], klim[1], n)
    f = np.linspace(flim[0], flim[1], n)
    K, F = np.meshgrid(k, f)
    Z = G(F, K)
    if np.iscomplexobj(Z):
        Z = np.where(np.abs(np.imag(Z)) < 1e-10, np.real(Z), np.nan)
    else:
        Z = np.asarray(Z, dtype=float)
    ax.contour(K, F, Z, levels=[0.0], colors=color, linewidths=linewidth, alpha=0.8)
    if label:
        ax.plot([], [], color=color, linewidth=linewidth, label=label)


def main():
    """Create and save the f-k variation plot"""
    import sys
    print("Loading data from cache...", flush=True)
    data = fk_analysis.load_or_process_data(gif_dir='gif', use_cache=True, verbose=True)
    
    f_values = data['f_values']
    k_values = data['k_values']
    variations = data['variations']
    
    print(f"\nCreating f-k variation plot...", flush=True)
    print(f"  Data points: {len(f_values)}", flush=True)
    print(f"  f range: [{f_values.min():.4f}, {f_values.max():.4f}]", flush=True)
    print(f"  k range: [{k_values.min():.4f}, {k_values.max():.4f}]", flush=True)
    print(f"  Variation range: [{variations.min():.2f}, {variations.max():.2f}]", flush=True)
    
    # Create figure
    print("Creating figure...", flush=True)
    fig, ax = plt.subplots(figsize=(8, 6))
    
    # Scatter plot with spatial variation as color
    print("Plotting scatter points...", flush=True)
    sc = ax.scatter(k_values, f_values, c=variations, s=24, alpha=0.7, cmap='viridis')
    # Color bar removed (as in the notebook)
    
    ax.set_xlabel('k', fontsize=12)
    ax.set_ylabel('f', fontsize=12)
    ax.set_title('f-k Map: Spatial Variation', fontsize=14)
    ax.grid(True, alpha=0.3)
    
    # Add theoretical bifurcation curves
    print("Calculating bifurcation curves...", flush=True)
    kmin, kmax = k_values.min(), k_values.max()
    fmin, fmax = f_values.min(), f_values.max()
    
    # Hopf bifurcation curve
    def G_Hopf(f, k):
        return 4*k - (f + np.sqrt(f*(f-4*(f+k)**2)))**2 / ((f + k)**2)
    
    print("  Plotting Hopf curve...", flush=True)
    plot_curve_simple(G_Hopf, (kmin, kmax), (fmin, fmax), ax, 
                      color='green', linewidth=1.5, label='Hopf')
    
    # Numerical solution boundary
    def G_NumSoln(f, k):
        return f - 4*f**2 - 8*f*k - 4*k**2
    
    print("  Plotting NumSoln curve...", flush=True)
    plot_curve_simple(G_NumSoln, (kmin, kmax), (fmin, fmax), ax, 
                      color='red', linewidth=1.5, label='NumSoln')
    
    ax.legend(loc='upper right', fontsize=9)
    
    # Set aspect ratio
    data_range_k = k_values.max() - k_values.min()
    data_range_f = f_values.max() - f_values.min()
    ax.set_aspect(data_range_k / (1.2 * data_range_f), adjustable='box')
    
    fig.tight_layout(pad=2.0)
    
    # Save figure
    print("\nSaving plot...", flush=True)
    output_dir = os.path.join(os.path.dirname(__file__), '..', 'visualization')
    os.makedirs(output_dir, exist_ok=True)
    output_file = os.path.join(output_dir, 'fk_variation_plot.png')
    fig.savefig(output_file, dpi=150, bbox_inches='tight')
    print(f"Plot saved to: {output_file}", flush=True)
    
    # Also save as PDF for high-quality output
    output_pdf = os.path.join(output_dir, 'fk_variation_plot.pdf')
    fig.savefig(output_pdf, bbox_inches='tight')
    print(f"Plot saved to: {output_pdf}", flush=True)
    
    plt.close()
    print("Done!", flush=True)


if __name__ == '__main__':
    main()

