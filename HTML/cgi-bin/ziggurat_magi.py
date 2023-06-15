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

#Cities of Bronze
#buildings/resource generators, farm generates food, magi school and food generate magi, 
# eldrich_ziggurat and magi generate mana, 
# magi and mana are needed to summon djiins/effreti, 
# the goal is to build an ancient temple which costs food and mana 
# all buildings cost 300 mana and 300 food to build initially
# need descriptions of the buildings

#time the number of resources is the number of stored resources plus the number of resources added each turn
turns = 3
eldrich_ziggurat = params.get('eldrich_ziggurat')
magi_school = params.get('magi_school')
farm = params.get('farm')
ammount_of_farms = farm
ancient_temple = params.get('ancient_temple')
wall = params.get('wall')
#resources you get 500 food per farm per turn 10 mana per magi per turn ziggurat gives 500 mana
food = ammount_of_farms * turns
mana = params.get('mana')
#citizens magi cost 100 food to build, djiin 100 mana and you need to have 100 magi per djiin produced per turn
magi = params.get('magi')
djiin = params.get('djiin')
effreti = params.get('effreti')
# Use the parameter values as needed
print('eldrich_ziggurat:', eldrich_ziggurat)
print('magi_school', magi_school)
print('farm', farm)
print('ancient_temple', ancient_temple)
print('food', food)
