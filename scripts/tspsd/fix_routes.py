#!/usr/bin/python
import os
import json

path = "./log/scp-exact/selected_12h/eil101-18.00.json"

with open(path, 'r') as file:
    data = json.load(file)
    route = data["solution"]["route"]
    route.reverse()
    data["solution"]["route"] = route

with open(path, 'w') as file:
    json.dump(data, file)
