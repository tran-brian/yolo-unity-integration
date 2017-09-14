import os
from collections import defaultdict

# Extensios to look for
EXTENSIONS = {'.jpg', '.txt'}

# Get current directory
current_dir = os.path.dirname(os.path.abspath(__file__))

grouped_files = defaultdict(int)

# File prefix
prefix = "gene"

# Start of numbering
count = 35

for f in os.listdir(current_dir):
    name, ext = os.path.splitext(os.path.join(current_dir, f))
    if ext in EXTENSIONS:
        grouped_files[name] += 1

for name in grouped_files:
    if grouped_files[name] == len(EXTENSIONS):
        # with open('{}.txt'.format(name)) as txt_file, \
        #         open('{}.jpg'.format(name)) as json_file:
        #     # process files
        #     print(txt_file, json_file)
        os.rename(os.path.join(current_dir, name) + ".jpg", os.path.join(current_dir, prefix + "-" + str(count)) + ".jpg")
        os.rename(os.path.join(current_dir, name) + ".txt", os.path.join(current_dir, prefix + "-" + str(count)) + ".txt")
    	count+= 1