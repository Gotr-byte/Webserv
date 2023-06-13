import os

# Access the QUERY_STRING environment variable
query_string = os.environ.get('QUERY_STRING')

# Parse the query string to retrieve the parameter values
params = {}
if query_string:
    pairs = query_string.split('&')
    for pair in pairs:
        key, value = pair.split('=')
        print ('value:', value)
        params[key] = value

# Access the parameter values
param1 = params.get('param1')
param2 = params.get('param2')

# Use the parameter values as needed
print('Pararameter 1:', param1)
print('Paraarmeter 2:', param2)
print('Parameter 3:', param2)
