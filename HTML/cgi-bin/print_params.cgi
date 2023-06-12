#!/usr/bin/env python

import os

# Set the content type header
print("Content-Type: text/html")
print()

# Print the CGI environment variables
print("<h1>CGI Environment Variables:</h1>")
print("<ul>")
for key, value in os.environ.items():
    print("<li><b>{}</b>: {}</li>".format(key, value))
print("</ul>")

# Print a simple message
print("<h2>Hello from CGI script!</h2>")