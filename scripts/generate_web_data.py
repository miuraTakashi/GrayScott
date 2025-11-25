import numpy as np
import json
import os

def generate_web_data():
    cache_file = os.path.join(os.path.dirname(__file__), '..', 'visualization', 'fk_data_cache.npz')
    print(f"Loading data from {cache_file}...")
    
    if not os.path.exists(cache_file):
        print(f"Error: {cache_file} not found.")
        return

    # Load data directly from cache
    cache = np.load(cache_file, allow_pickle=True)
    
    f_values = cache['f_values']
    k_values = cache['k_values']
    variations = cache['variations']
    paths = cache['paths']
    
    print(f"Loaded {len(f_values)} points.")
    
    # Extract filenames from paths and change extension to .mp4
    filenames = [os.path.basename(str(p)).replace('.gif', '.mp4') for p in paths]
    
    # Create a dictionary structure
    web_data = {
        'f': f_values.tolist(),
        'k': k_values.tolist(),
        'v': variations.tolist(),
        'filenames': filenames
    }
    
    # Output as a JS file
    output_file = 'fk_web_data.js'
    print(f"Writing to {output_file}...")
    
    with open(output_file, 'w') as f:
        f.write("const fkData = ")
        json.dump(web_data, f)
        f.write(";")
        
    print("Done!")

if __name__ == "__main__":
    generate_web_data()
