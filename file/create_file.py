#!/usr/bin/env python

import os

# Set the content type header to indicate that the response will be plain text
print("Content-Type: text/plain")

# Create a file and write "Hello World" to it
file_path = "../file/file.txt"
with open(file_path, "w") as file:
    file.write("Hello World")

# Print a success message
print("File created with 'Hello World'")

-