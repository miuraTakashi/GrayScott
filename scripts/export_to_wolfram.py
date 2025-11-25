#!/usr/bin/env python3
"""
Export Gray-Scott f-k parameter data to Wolfram Language compatible formats
"""

import os
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'visualization'))
import fk_analysis

# Load data from cache
print("Loading data from cache...")
data = fk_analysis.load_or_process_data(gif_dir='gif', use_cache=True, verbose=True)

# Export to multiple formats for Wolfram Language
print("\n" + "="*60)
print("Exporting data to Wolfram Language compatible formats...")
print("="*60 + "\n")

# Option 1: CSV format (most compatible)
output_dir = os.path.join(os.path.dirname(__file__), '..', 'visualization')
os.makedirs(output_dir, exist_ok=True)
fk_analysis.export_to_csv(data, output_file=os.path.join(output_dir, 'fk_data.csv'))

# Option 2: JSON format
fk_analysis.export_to_json(data, output_file=os.path.join(output_dir, 'fk_data.json'))

# Option 3: Wolfram Language native format (.wl) - Association format (recommended)
fk_analysis.export_to_wolfram(data, output_file='fk_data.wl', format='association')

print("\n" + "="*60)
print("Export completed!")
print("="*60)
print("\nFiles created:")
print("  - fk_data.csv (CSV format)")
print("  - fk_data.json (JSON format)")
print("  - fk_data.wl (Wolfram Language native format)")

