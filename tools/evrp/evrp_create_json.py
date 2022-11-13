#!python3

import sys
import os
import json

def create_dict(input_file_path):
    D = {}
    node_coord = []
    node_demand = []
    stations = []

    section = 'init'
    with open(input_file_path, 'r') as f:
        for line in f:
            line = line.strip()
            splt = line.split(' ', 1)
            
            if splt[0] == 'NODE_COORD_SECTION':
                D['node_coord'] = []
                section = 'node_coord'
            
            elif splt[0] == 'DEMAND_SECTION':
                D['node_demand'] = []
                section = 'demand'
            
            elif splt[0] == 'STATIONS_COORD_SECTION':
                D['stations'] = []
                section = 'stations'
            
            elif splt[0] == 'DEPOT_SECTION':
                section = 'depot'

            elif splt[0] == 'EOF':
                break

            elif section == 'init':
                value = splt[1]
                try:
                    value = int(value)
                except ValueError:
                    try:
                        value = float(value)
                    except:
                        pass
                    
                D[splt[0].lower()] = value

            elif section == 'node_coord':
                D['node_coord'].append({
                    'id' : int(splt[0]),
                    'coords' : [float(x) for x in splt[1].split(' ')]
                })
            
            elif section == 'demand':
                D['node_demand'].append({
                    'id' : int(splt[0]),
                    'demand' : float(splt[1])
                })

            elif section == 'stations':
                D['stations'].append(int(splt[0]))
            
            elif section == 'depot':
                id = int(splt[0])
                if id >= 0:
                    D['depot'] = id

        return D

if __name__ == '__main__':
    raw_file_dir = sys.argv[1]
    json_file_dir = sys.argv[2]

    for filename in os.listdir(raw_file_dir):
        root, ext = os.path.splitext(filename)
        
        if ext == '.evrp':
            raw_file = os.path.join(raw_file_dir, filename)
            print('Parsing ERVP file \'{}\''.format(raw_file))
            D = create_dict(raw_file)
            json_file = os.path.join(json_file_dir, 'evrp-'+root+'.json')
            print('Creating JSON file \'{}\''.format(json_file))
            with open(json_file, 'w') as f:
                f.write(json.dumps(D, indent=2))
            

