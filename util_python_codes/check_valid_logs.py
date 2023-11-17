# -*- coding: utf-8 -*-
"""
Created on Tue Nov 14 21:35:36 2023

@author: HSJ
"""

from glob import glob

log_files = glob("./compiled_files_vmprotect/*.txt")
invalid_count = 0

for log_file in log_files :
    #print(log_file)
    if "Restored" in log_file :
        continue
    if log_file.replace(".txt", ".Restored.txt") not in log_files:
        print(log_file)
        invalid_count += 1
        
    #with open(".exe", ".Restored.exe") as f:
            
print("Total invalid file count : ", invalid_count)