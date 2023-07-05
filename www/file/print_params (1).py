import cgi
# Create a FieldStorage object to access the parameters
form = cgi.FieldStorage()

# Get the values of the parameters
param1 = form.getvalue('param1')
param2 = form.getvalue('param2')

# Print the parameter values
print("Parameter 1:", param1)
print("Parameter 2:", param2)
-