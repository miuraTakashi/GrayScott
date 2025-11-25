import os
import glob
import imageio
from PIL import Image
import numpy as np
import multiprocessing
from functools import partial

def resize_gif_small(gif_path, output_dir, target_size=(64, 64), target_frames=32):
    try:
        filename = os.path.basename(gif_path)
        output_path = os.path.join(output_dir, filename)
        
        if os.path.exists(output_path):
            return # Skip if already exists
            
        # Use imageio for reading as it proved reliable
        reader = imageio.get_reader(gif_path)
        original_frames = [frame for frame in reader]
        n_frames = len(original_frames)
        
        step = max(1, n_frames // target_frames)
        
        new_frames = []
        for i in range(0, n_frames, step):
            if len(new_frames) >= target_frames:
                break
            
            # Convert to PIL for resizing
            img = Image.fromarray(original_frames[i])
            img = img.resize(target_size, Image.NEAREST)
            new_frames.append(np.array(img))
            
        if new_frames:
            # Save using imageio
            imageio.mimsave(output_path, new_frames, duration=0.1, loop=0)
            return True
            
    except Exception as e:
        print(f"Error processing {gif_path}: {e}")
        return False

def main():
    input_dir = 'gif'
    output_dir = 'gif_small'
    
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        
    gif_files = sorted(glob.glob(os.path.join(input_dir, '*.gif')))
    print(f"Found {len(gif_files)} GIFs to process.")
    
    # Use multiprocessing
    num_cores = multiprocessing.cpu_count()
    print(f"Using {num_cores} cores.")
    
    process_func = partial(resize_gif_small, output_dir=output_dir)
    
    with multiprocessing.Pool(num_cores) as pool:
        # Process in chunks
        chunk_size = 1000
        total = len(gif_files)
        for i in range(0, total, chunk_size):
            chunk = gif_files[i:i+chunk_size]
            pool.map(process_func, chunk)
            print(f"Processed {min(i+chunk_size, total)}/{total}")
            
    print("Done!")

if __name__ == "__main__":
    main()
