import os

# Specify the file path to be deleted
file_path = "../file/file.txt"

try:
    # Check if the file exists
    if os.path.exists(file_path):
        # Delete the file
        os.remove(file_path)
        print("File deleted successfully.")
    else:
        print("File does not exist.")
except Exception as e:
    print(f"An error occurred: {str(e)}")