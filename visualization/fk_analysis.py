"""
Gray-Scott f-k parameter analysis library
Processes GIF files and calculates spatial variations
"""

import os
import re
import glob
import numpy as np
import csv
import json
from PIL import Image as PILImage


def calculate_spatial_variation(gif_path):
    """
    Extract last frame from GIF and calculate spatial variation
    
    Parameters:
    -----------
    gif_path : str
        Path to the GIF file
        
    Returns:
    --------
    float
        Spatial variation (sum of absolute differences)
    """
    try:
        img = PILImage.open(gif_path)
        # Go to the last frame
        n_frames = getattr(img, 'n_frames', 1)
        if n_frames > 1:
            img.seek(n_frames - 1)
        
        # Convert to grayscale numpy array
        img_gray = img.convert('L')
        u = np.array(img_gray, dtype=float) / 255.0
        
        # Calculate spatial variation: sum of absolute differences
        # np.sum(np.abs(u[1:] - u[:-1])) for vertical differences
        variation = np.sum(np.abs(u[1:] - u[:-1])) + np.sum(np.abs(u[:, 1:] - u[:, :-1]))
        
        return variation
    except Exception as e:
        print(f"Error processing {gif_path}: {e}")
        return 0.0


def load_gif_data(gif_dir='gif', verbose=True):
    """
    Load all GIF files from directory and calculate spatial variations
    
    Parameters:
    -----------
    gif_dir : str
        Directory containing GIF files (default: 'gif')
    verbose : bool
        Print progress messages (default: True)
        
    Returns:
    --------
    dict
        Dictionary containing:
        - 'f_values': numpy array of f parameter values
        - 'k_values': numpy array of k parameter values
        - 'variations': numpy array of spatial variations
        - 'paths': list of GIF file paths
    """
    # Find all GIF files in the gif directory
    GIF_DIR = os.path.abspath(gif_dir)
    if not os.path.exists(GIF_DIR):
        raise FileNotFoundError(f"GIF directory not found: {GIF_DIR}")
    
    if verbose:
        print(f"Scanning GIF directory: {GIF_DIR}")
    
    # Match pattern: GrayScott-f0.0100-k0.0500-*.gif
    pattern_re = re.compile(r"GrayScott-f([0-9]+\.[0-9]+)-k([0-9]+\.[0-9]+)-.*\.gif$")
    
    all_gifs = sorted(glob.glob(os.path.join(GIF_DIR, 'GrayScott-f*-k*-*.gif')))
    if verbose:
        print(f"Found {len(all_gifs)} GIF files")
    
    # Parse GIF files and calculate spatial variations
    f_values = []
    k_values = []
    variations = []
    paths = []
    
    if verbose:
        print("Calculating spatial variations from GIF files...")
    
    for i, p in enumerate(all_gifs):
        if verbose and i % 100 == 0:
            print(f"  Processing {i}/{len(all_gifs)}...")
        
        m = pattern_re.search(p)
        if not m:
            continue
        try:
            f_parsed = float(m.group(1))
            k_parsed = float(m.group(2))
        except Exception:
            continue
        
        variation = calculate_spatial_variation(p)
        
        f_values.append(f_parsed)
        k_values.append(k_parsed)
        variations.append(variation)
        paths.append(p)
    
    # Convert to numpy arrays
    f_values = np.asarray(f_values, dtype=float)
    k_values = np.asarray(k_values, dtype=float)
    variations = np.asarray(variations, dtype=float)
    
    if verbose:
        print(f"Processed {len(f_values)} GIF files")
        print(f"f range: [{f_values.min():.4f}, {f_values.max():.4f}]")
        print(f"k range: [{k_values.min():.4f}, {k_values.max():.4f}]")
        print(f"Variation range: [{variations.min():.2f}, {variations.max():.2f}]")
    
    return {
        'f_values': f_values,
        'k_values': k_values,
        'variations': variations,
        'paths': paths
    }


def find_gif_for_fk(f, k, f_values, k_values, paths):
    """
    Find the GIF file path for given f,k parameters (nearest neighbor)
    
    Parameters:
    -----------
    f : float
        f parameter value
    k : float
        k parameter value
    f_values : numpy.ndarray
        Array of all f values
    k_values : numpy.ndarray
        Array of all k values
    paths : list
        List of all GIF file paths
        
    Returns:
    --------
    str or None
        Path to the nearest GIF file, or None if not found
    """
    if len(f_values) > 0:
        d2 = (f_values - f)**2 + (k_values - k)**2
        j = int(np.argmin(d2))
        return paths[j]
    return None


def save_cache(data, cache_file='fk_data_cache.npz'):
    """
    Save processed data to cache file
    
    Parameters:
    -----------
    data : dict
        Dictionary returned by load_gif_data()
    cache_file : str
        Path to cache file (default: 'fk_data_cache.npz')
    """
    np.savez(cache_file,
             f_values=data['f_values'],
             k_values=data['k_values'],
             variations=data['variations'],
             paths=data['paths'])
    print(f"Cache saved to {cache_file}")


def load_cache(cache_file='fk_data_cache.npz'):
    """
    Load processed data from cache file
    
    Parameters:
    -----------
    cache_file : str
        Path to cache file (default: 'fk_data_cache.npz')
        
    Returns:
    --------
    dict
        Dictionary containing f_values, k_values, variations, and paths
    """
    if not os.path.exists(cache_file):
        raise FileNotFoundError(f"Cache file not found: {cache_file}")
    
    cache = np.load(cache_file, allow_pickle=True)
    print(f"Cache loaded from {cache_file}")
    print(f"  {len(cache['f_values'])} data points")
    
    return {
        'f_values': cache['f_values'],
        'k_values': cache['k_values'],
        'variations': cache['variations'],
        'paths': list(cache['paths'])
    }


def load_or_process_data(gif_dir='gif', cache_file='fk_data_cache.npz', 
                         use_cache=True, verbose=True):
    """
    Load data from cache if available, otherwise process GIF files
    
    Parameters:
    -----------
    gif_dir : str
        Directory containing GIF files (default: 'gif')
    cache_file : str
        Path to cache file (default: 'fk_data_cache.npz')
    use_cache : bool
        Whether to use cache if available (default: True)
    verbose : bool
        Print progress messages (default: True)
        
    Returns:
    --------
    dict
        Dictionary containing f_values, k_values, variations, and paths
    """
    if use_cache and os.path.exists(cache_file):
        try:
            return load_cache(cache_file)
        except Exception as e:
            if verbose:
                print(f"Failed to load cache: {e}")
                print("Processing GIF files instead...")
    
    # Process GIF files
    data = load_gif_data(gif_dir, verbose=verbose)
    
    # Save to cache
    if use_cache:
        save_cache(data, cache_file)
    
    return data


def export_to_csv(data, output_file='fk_data.csv'):
    """
    Export data to CSV format for Wolfram Language
    
    Parameters:
    -----------
    data : dict
        Dictionary returned by load_or_process_data() or load_cache()
    output_file : str
        Output CSV file path (default: 'fk_data.csv')
    """
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        # Write header
        writer.writerow(['f', 'k', 'variation'])
        # Write data
        for i in range(len(data['f_values'])):
            writer.writerow([
                data['f_values'][i],
                data['k_values'][i],
                data['variations'][i]
            ])
    print(f"Data exported to {output_file}")
    print(f"  {len(data['f_values'])} data points")
    print(f"\nWolfram Language usage:")
    print(f"  data = Import[\"{output_file}\", \"CSV\"];")
    print(f"  f = data[[2;;, 1]];")
    print(f"  k = data[[2;;, 2]];")
    print(f"  variation = data[[2;;, 3]];")


def export_to_json(data, output_file='fk_data.json'):
    """
    Export data to JSON format for Wolfram Language
    
    Parameters:
    -----------
    data : dict
        Dictionary returned by load_or_process_data() or load_cache()
    output_file : str
        Output JSON file path (default: 'fk_data.json')
    """
    # Convert numpy arrays to lists for JSON serialization
    json_data = {
        'f_values': data['f_values'].tolist(),
        'k_values': data['k_values'].tolist(),
        'variations': data['variations'].tolist()
    }
    
    with open(output_file, 'w') as f:
        json.dump(json_data, f, indent=2)
    print(f"Data exported to {output_file}")
    print(f"  {len(data['f_values'])} data points")
    print(f"\nWolfram Language usage:")
    print(f"  data = Import[\"{output_file}\", \"JSON\"];")
    print(f"  f = data[\"f_values\"];")
    print(f"  k = data[\"k_values\"];")
    print(f"  variation = data[\"variations\"];")


def export_to_wolfram(data, output_file='fk_data.wl', format='list'):
    """
    Export data to Wolfram Language native format (.wl)
    
    Parameters:
    -----------
    data : dict
        Dictionary returned by load_or_process_data() or load_cache()
    output_file : str
        Output .wl file path (default: 'fk_data.wl')
    format : str
        Output format: 'list' (default) or 'association'
        - 'list': Returns {f, k, variation} as nested lists
        - 'association': Returns <|"f" -> {...}, "k" -> {...}, "variation" -> {...}|>
    """
    f_vals = data['f_values'].tolist()
    k_vals = data['k_values'].tolist()
    var_vals = data['variations'].tolist()
    
    with open(output_file, 'w') as f:
        if format == 'association':
            f.write("<|\n")
            f.write('  "f" -> ' + str(f_vals).replace("'", '"') + ',\n')
            f.write('  "k" -> ' + str(k_vals).replace("'", '"') + ',\n')
            f.write('  "variation" -> ' + str(var_vals).replace("'", '"') + '\n')
            f.write("|>")
        else:  # list format
            f.write("{\n")
            f.write("  " + str(f_vals) + ",\n")
            f.write("  " + str(k_vals) + ",\n")
            f.write("  " + str(var_vals) + "\n")
            f.write("}")
    
    print(f"Data exported to {output_file}")
    print(f"  {len(data['f_values'])} data points")
    print(f"  Format: {format}")
    print(f"\nWolfram Language usage:")
    if format == 'association':
        print(f"  data = Get[\"{output_file}\"];")
        print(f"  f = data[\"f\"];")
        print(f"  k = data[\"k\"];")
        print(f"  variation = data[\"variation\"];")
    else:
        print(f"  data = Get[\"{output_file}\"];")
        print(f"  f = data[[1]];")
        print(f"  k = data[[2]];")
        print(f"  variation = data[[3]];")

