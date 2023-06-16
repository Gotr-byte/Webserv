import os
import sys
#Cities of Bronze
#buildings/resource generators, farm generates food, magi school and food generate magi, 
# eldrich_ziggurat and magi generate mana, 
# magi and mana are needed to summon djiins/effreti, 
# the goal is to build an ancient temple which costs food and mana 
# all buildings cost 300 mana and 300 food to build initially
# need descriptions of the buildings
#resources you get 500 food per farm per turn 10 mana per magi per turn ziggurat gives 500 mana
#citizens magi cost 100 food to build, djiin 100 mana and you need to have 100 magi per djiin produced per turn

#time the number of resources is the number of stored resources plus the number of resources added each turn
# turns = 3
# eldrich_ziggurat = params.get('eldrich_ziggurat')
# magi_school = params.get('magi_school')
# ammount_of_farms = 10
# ancient_temple = params.get('ancient_temple')
# wall = params.get('wall')
# food = ammount_of_farms * turns
# mana = params.get('mana')
# magi = params.get('magi')
# djiin = params.get('djiin')
# effreti = params.get('effreti')
# Use the parameter values as needed
# print('eldrich_ziggurat:', eldrich_ziggurat)
# print('magi_school', magi_school)
# print('ancient_temple', ancient_temple)
# print('food', food)

variable_name = "QUERY_STRING"

if variable_name in os.environ:
    print("The environment variable '{variable_name}' exists. So we use the GET request.")
    # Access the QUERY_STRING environment variable
    query_string = os.environ.get('QUERY_STRING')
    # Parse the query string to retrieve the parameter values
    params = {}
    if query_string:
        pairs = query_string.split('&')
        for pair in pairs:
            key, value = pair.split('=')
            # print ('value:', value)
            params[key] = value
    farm = params.get('farm')
    # print('farm', farm)
    sys.exit(0)
else:
    # Read from standard input
    # num_bytes = int(os.environ.get('CONTENT_LENGTH'))
    num_bytes = 6 + 1
    # Number of bytes to read
    # data = sys.stdin.read(num_bytes)
    print("Content-Type: text/html")  # Required HTTP response header

    print()  # Empty line to separate headers from the body
    data = "farm=1"
    try:
        farm = data.split('=')[1]
        print("<h1>Farm level: ", farm)
        print("</h1>")
    except IndexError:
        print("Invalid data format")
        sys.exit(1)
    # Process the data
    # print('farm', farm)
    sys.exit(0)

