import os
import numpy as np
import json
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
from time import sleep


dir_ = 'C:\\Users\\pazou\\Documents\\CVUT\\BAKALARKA\\stats\\'

operators = ["AEX", "APX", "CX" , "ERULX", "ERX", "EULX", "HGreX", "HProX", "HRndX", "MPX", "NBX", "OBX", "OX" , "PBX", "RULX", "SPX", "ULX", "UPMX"]

BKS_EVRP = [  839, 385, 572, 509, 840, 530, 693, 77476,  16028, 11324, 27064, 25371, 52182, 71345, 81002, 164290, 341650 ]

problems_EVRP = [   "E-n101-k8-s9",
                    "E-n22-k4-s8", 
                    "E-n23-k3-s9",
                    "E-n30-k3-s6",
                    "E-n33-k4-s6",
                    "E-n51-k5-s9",
                    "E-n76-k7-s9",
                    "X-n1001-k43-s9",
                    "X-n143-k7-s4",
                    "X-n214-k11-s9",
                    "X-n351-k40-s35",
                    "X-n459-k26-s20",
                    "X-n573-k30-s6",
                    "X-n685-k75-s25",
                    "X-n749-k98-s30",
                    "X-n819-k171-s25", 
                    "X-n916-k207-s9"   ]



times_EVRP = [  3924,
                1044,          
                1116,          
                1260,          
                1368,          
                1980,          
                2952,          
                75168,         
                10512,        
                15984,       
                27720,       
                34416,
                41616,
                51048,
                56016,
                60696,
                66528 ]


problems_ROADEF = [ "A_01.json",
                    "A_02.json",
                    "A_03.json",
                    "A_04.json",
                    "A_05.json",
                    "A_06.json",
                    "A_07.json",
                    "A_08.json",
                    "A_09.json",
                    "A_10.json",
                    "A_11.json",
                    "A_12.json",
                    "A_13.json",
                    "A_14.json",
                    "A_15.json" ]

times_ROADEF_1 = [  900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900,
                    900

]

times_ROADEF_2 = [  5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400,
                    5400 ]



BKS_ROADEF = [ 1768, 4671, 848, 2086, 635, 591, 2273, 744, 1507, 2995, 495, 790, 1999, 2264, 2269 ]

  
def processData(method, problems, times, BKS):

    for i in range(len(problems)):
        problem = problems[i]
        time = times[i]
        print()
        print(problem)

        data = np.zeros((50, time), np.uint64)
        best_solution = np.array([], int)
        last_improvement = np.array([], int)
        first_feasible = np.array([], int)
        is_fealsible = np.zeros(50, int)
        valid = 0
        total = 0
        problem_dir = dir_+method
        config = {}

        for file in os.listdir(problem_dir):

            f = problem_dir + "/" + file
            if method not in f or problem not in f:
                continue
            print(f)
            idx = int(file.split('-')[-1].split('.')[0]) - 1

            with open(f, 'r+') as file:
                # print(f)
                content = json.load(file)
                total += 1

                if content["steps"] is not None:
                    for k, v in content["steps"].items():
                        if int(k) >= time:
                            continue
                        data[idx][int(k)] = np.uint64(v)

                for dataIdx in range(1,len(data[idx])):
                    if data[idx][dataIdx] == 0:
                        data[idx][dataIdx] = data[idx][dataIdx-1]

                
                is_fealsible[idx] = content["solution"]["is_feasible"]
                if content["solution"]["is_feasible"] == 1:
                    best_solution = np.append(best_solution ,content["solution"]["fitness"])
                    last_improvement = np.append(last_improvement ,content["last_improvement"])
                    first_feasible  = np.append(first_feasible ,content["first_feasible"])

        mean = np.zeros(time, float)
        std = np.zeros(time, float)
        for j,x in enumerate(np.transpose(data)):
            temp = np.array([y for y in x if y != 0])
            mean[j] = 0 if len(temp) == 0 else np.mean(temp) 
            std[j] = 0 if len(temp) == 0 else np.std(temp) 


        config["avg_gap"] = 100*(np.mean(best_solution) - BKS[i])/BKS[i]
        config["avg_is_feasible"] = 100*np.mean(is_fealsible)
        config["avg_best_solution"] =  np.mean(best_solution)
        config["avg_last_improvement"] = np.mean(last_improvement)
        config["avg_first_feasible"] = np.mean(first_feasible)
        config["bks"] = BKS[i]
        config["time"] = time
        
        config["is_feasible"] = [bool(i) for i in list(is_fealsible)]
        config["best_solution"] =  [int(i) for i in list(best_solution)]
        config["last_improvement"] = [int(i) for i in list(last_improvement)]
        config["first_feasible"] = [int(i) for i in list(first_feasible)]

        config["mean"] = [float(i) for i in list(mean)]
        config["std"] = [float(i) for i in list(std)]
        config["fitness"] = [[int(j) for j in i] for i in list(data)]

        valid_data = []
        for j in range(50):
            if is_fealsible[j] == 1:
                valid_data.append(data[j][-1])

        m = np.mean(valid_data)

        print("mean:", m)
        print("std:", np.std(valid_data))
        print("gap:", 100*(m-BKS[i])/BKS[i])
        print("f:", config["avg_is_feasible"])
        print("first feasible: ", np.mean(config["first_feasible"]))
        # print("mean:", mean[-1])
        # print("std:", std[-1])
        # print("gap:", 100*(mean[-1]-BKS[i])/BKS[i])
        # print("f:", config["avg_is_feasible"])
        

        file = open(dir_ + "data-" + method + '-' + problem+".json", "w")
        json.dump(config, file, indent=4)
        file.close()

def toGap(l, bks):
    return [(100*(x-bks)/bks) for x in l]

def processOpData(method, op, problem, bks):
    problem_dir = dir_+method+'-op'
    for file in os.listdir(problem_dir):
        f = problem_dir + "/" + file
        if method not in f or problem not in f or '-'+op+'-' not in f:
            continue
        print(f)
        with open(f, 'r') as file:
            data = file.readlines()
            total = 0
            c = 0
            valid_data = []
            for line in data:
                fitness, feasible = line.split()
                total += 1
                if '1' in feasible :
                    c += 1
                    valid_data.append(int(fitness))
            print("mean:", np.mean(valid_data))
            print("std:", np.std(valid_data))
            print("f:", int(100*float(c)/total))
            print("gap:", 100*(np.mean(valid_data) -bks)/bks)


def makePlot(name):
    filename = ' '
    for file in os.listdir(dir_):
        if name in file:
            filename = file 

    with open(dir_ + filename, 'r') as file:
        content = json.load(file)
        bks = content["bks"]
        data = list(content["mean"])
        start = len([1 for i in data if i < 0.1])
        
        time = list(range(start, content["time"]))
        data = data[start:]
        std = content["std"][start:]
        mins =  [data[i]+std[i] for i in range(len(data))]
        maxs = [data[i]-std[i] for i in range(len(data))]

        plt.yscale("log")
        plt.grid(axis='y', which='minor')
        plt.plot( time, toGap(data, bks), 'b', label='average GAP')
        plt.fill_between(time, toGap(mins, bks), toGap(maxs, bks) , color='blue', alpha=0.3, linewidth=0, label='\u03C3(GAP)')
        plt.axvline(x = content["avg_first_feasible"], color ='green', label='average feasiblity achieved')
        plt.axvline(x = content["avg_last_improvement"], color ='red', label='average last improvement')
        

        plt.title("GMS-CO: X-n459-k26")


        plt.xlabel("time")
        plt.ylabel("GAP")
        plt.xlim([0, content["time"]])
        plt.gca().xaxis.set_major_formatter(mtick.FormatStrFormatter('%ds'))
        # plt.ylim([0, np.max(np.array(toGap(maxs, bks)))])
        plt.gca().yaxis.set_major_formatter(mtick.PercentFormatter(decimals=1))
        plt.legend(loc='upper right')
        plt.savefig("C:\\Users\\pazou\\Documents\\CVUT\\BAKALARKA\\bachelor-thesis\\images\\" + name +".pdf", format="pdf", bbox_inches="tight")
        plt.clf()
        plt.legend([],loc='upper right')


