import os
import glob
from PIL import Image
import multiprocessing
from functools import partial

def resize_gif(gif_path, output_dir, target_size=(64, 64), target_frames=64):
    try:
        filename = os.path.basename(gif_path)
        output_path = os.path.join(output_dir, filename)
        
        if os.path.exists(output_path):
            return # Skip if already exists
            
        with Image.open(gif_path) as img:
            frames = []
            n_frames = getattr(img, 'n_frames', 1)
            
            # Calculate frame step to get exactly target_frames
            # If original has 129 frames and we want 64, step is approx 2
            step = max(1, n_frames // target_frames)
            
            for i in range(0, n_frames, step):
                if len(frames) >= target_frames:
                    break
                img.seek(i)
                # Resize frame
                frame = img.copy().resize(target_size, Image.NEAREST)
                frames.append(frame)
            
            if frames:
                # Save as new GIF
                frames[0].save(
                    output_path,
                    save_all=True,
                    append_images=frames[1:],
                    duration=img.info.get('duration', 100),
                    loop=img.info.get('loop', 0),
                    optimize=True
                )
                return True
    except Exception as e:
        print(f"Error processing {gif_path}: {e}")
        return False

def main():
    input_dir = 'gif'
    output_dir = 'gif_half'
    
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        
    gif_files = sorted(glob.glob(os.path.join(input_dir, '*.gif')))
    print(f"Found {len(gif_files)} GIFs to process.")
    
    # Use multiprocessing
    num_cores = multiprocessing.cpu_count()
    print(f"Using {num_cores} cores.")
    
    process_func = partial(resize_gif, output_dir=output_dir)
    
    with multiprocessing.Pool(num_cores) as pool:
        # Process in chunks to show progress
        chunk_size = 1000
        total = len(gif_files)
        for i in range(0, total, chunk_size):
            chunk = gif_files[i:i+chunk_size]
            pool.map(process_func, chunk)
            print(f"Processed {min(i+chunk_size, total)}/{total}")
            
    print("Done!")

if __name__ == "__main__":
    main()
