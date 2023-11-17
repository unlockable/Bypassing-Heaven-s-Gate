# -*- coding: utf-8 -*-
"""
Created on Tue Nov 14 21:35:36 2023

@author: HSJ
"""

from glob import glob

log_files = glob("./compiled_files_vmprotect/logs/*.out")
invalid_count = 0
total_handling_elasped_time = 0
total_bypassing_elasped_time = 0
total_executed = 0 

for log_file in log_files :
    with open(log_file, 'r') as f :
        
        lines = f.readlines()
        oep = lines[0].split("OEP is ")[1].strip()
        if oep != "4012e0" :
            invalid_count += 1
            
        handling_elasped_time = lines[2].split("Handling elasped time : ")[1].strip()
        total_handling_elasped_time += float(handling_elasped_time)
        bypassing_elasped_time = lines[4].split("Bypassing elasped time : ")[1].strip()        
        total_bypassing_elasped_time +=float(bypassing_elasped_time)
        executed_num = lines[5].split("executed : ")[1].strip()
        total_executed += int(executed_num)
            
print("Total invalid OEP count : ", invalid_count)
print("Total total_handling_elasped_time : ", total_handling_elasped_time)
print("Total total_bypassing_elasped_time : ", total_bypassing_elasped_time)
print("Total executed : ", total_executed)