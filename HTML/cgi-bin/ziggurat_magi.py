#!/usr/bin/env python3

import cgi
import sys

# Create a FieldStorage object to access the parameters
form = cgi.FieldStorage()

# Check if the request method is POST
if form and form.getvalue('farm'):
    # Retrieve the value of the "farm" parameter from the request body
    farm = form.getvalue('farm')
else:
    # If the "farm" parameter is not provided or the request method is not POST, set a default value
    farm = "Guest"

# Print the HTTP response headers
# print("Content-Type: text/html")
# print()

# Print the HTML response body
print("<html>")
print("<head>")
print("<title>CGI Example</title>")
print("</head>")
print("<body>")
print("<h1>You built, {} farms!</h1>".format(farm))
print("</body>")
print("</html>")

sys.exit(0)