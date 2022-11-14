#!python3

import sys
import os
import json

def create_dict(input_file_path):
    D = {}

    section = 'init'
    with open(input_file_path, 'r') as f:
        for line in f:
            line = line.strip()
            splt = line.split(' ', 1)
            
            if splt[0] == 'NODE_COORD_SECTION':
                D['node_coord'] = {}
                section = 'coord'
            
            elif splt[0] == 'DEMAND_SECTION':
                D['node_demand'] = {}
                section = 'demand'
            
            elif splt[0] == 'STATIONS_COORD_SECTION':
                D['stations_id'] = []
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
                    
                D[splt[0].strip(':').lower()] = value

            elif section == 'coord':
                D['node_coord'][splt[0]] = [float(x) for x in splt[1].split(' ')]
            
            elif section == 'demand':
                D['node_demand'][splt[0]] = float(splt[1])

            elif section == 'stations':
                D['stations_id'].append(splt[0])
            
            elif section == 'depot':
                id = splt[0]
                if id != '-1':
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
            

