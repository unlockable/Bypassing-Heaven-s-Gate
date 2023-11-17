import glob
import os
import shutil
folder_path = "./compiled_files_vmprotect/"  # Replace with the actual folder path

# Get a list of all *.txt files in the folder
txt_files = glob.glob(folder_path + "/*.txt")

exception_num = 0
total_num = 0
diff_num = 0
third_case = 0

for txt_file in txt_files:
    # Get the corresponding *.Restored.txt file
    if "Restored" in txt_file :
        continue
    total_num += 1
    try :
        restored_file = txt_file.replace(".txt", ".Restored.txt")
    
        # Check if the *.txt file size is zero
        txt_size = os.path.getsize(txt_file)
        if txt_size == 0:
            print(f"File {txt_file} has a size of zero.")
            continue
    
        # Check if the *.Restored.txt file size is zero
        restored_size = os.path.getsize(restored_file)
        if restored_size == 0:
            print(f"File {restored_file} has a size of zero.")
            third_case += 1
            continue
            
        # Read the content of the *.txt file
        with open(txt_file, "r") as file1:
            txt_content = file1.read()
    
        # Read the content of the *.Restored.txt file
        with open(restored_file, "r") as file2:
            restored_content = file2.read()
    
        # Compare the content of the two files
        if txt_content != restored_content:
            print(f"Content of {txt_file} and {restored_file} is different.")
            diff_num += 1

    except :
        print(txt_file)
        #exe_file_path = txt_file.replace(".txt", ".exe")
        #moved_path = exe_file_path.replace("compiled_files_aspack_0518", "aspack_fail_samples")
        #print(moved_path)
        #shutil.move(exe_file_path, moved_path)
        exception_num += 1
        
        
print("exception files num " + str(exception_num))
print("total_num  " + str(total_num))
print("diff_num  " + str(diff_num))
print("third_case  " + str(third_case))