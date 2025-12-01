import matplotlib.pyplot as plt
import webbrowser
import os
import csv


generation = []

agent0score = []
agent1score = []
agent2score = []
agent3score = []
agent4score = []
agent5score = []
agent6score = []
agent7score = []
agent8score = []
agent9score = []
mean = []
median = []
best_agent_score = []
time_for_one_gen = []

gen_max = 0
current_path = ""
all_paths = []


def clear_all():
    global gen_max, current_path
    gen_max = max(generation[-1],gen_max)
    all_paths.append(current_path)
    
    generation.clear()
    agent0score.clear()
    agent1score.clear()
    agent2score.clear()
    agent3score.clear()
    agent4score.clear()
    agent5score.clear()
    agent6score.clear()
    agent7score.clear()
    agent8score.clear()
    agent9score.clear()
    best_agent_score.clear()
    time_for_one_gen.clear()
    mean.clear()
    median.clear()


def load_stats(path,name=""):
    global current_path

    if not os.path.exists(path):
        print(f"[ERROR] Stats : The file {path} does not exist.")
        return
    
    current_path = name
    with open(path, mode='r') as file:
        reader = csv.DictReader(file)
        next(reader)  # Skip the header row
        for row in reader:
            generation.append((float(row["Generation"])))
            agent0score.append(float(row["Agent0Score"]))
            agent1score.append(float(row["Agent1Score"]))
            agent2score.append(float(row["Agent2Score"]))
            agent3score.append(float(row["Agent3Score"]))
            agent4score.append(float(row["Agent4Score"]))
            agent5score.append(float(row["Agent5Score"]))
            agent6score.append(float(row["Agent6Score"]))
            agent7score.append(float(row["Agent7Score"]))
            agent8score.append(float(row["Agent8Score"]))
            agent9score.append(float(row["Agent9Score"]))
            mean.append(float(row["Mean"]))
            median.append(float(row["Median"]))
            best_agent_score.append(float(row["BestAgentScore"]))
            time_for_one_gen.append(float(row["TimeForOneGen"]))
    
    return generation, mean, best_agent_score, time_for_one_gen


def save_stats(gen, agents, ba_score, ba_distance, goal, start, distance, fps, path):
    print("DO NOT WORK NOR DO I CARE")
    
    
def generate_scoreplot(path):
    gen = generation
    plt.figure()
    plt.plot(gen,best_agent_score,color="orange",label="Meilleur score")
    plt.title("Score du meilleur cerveau en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("Score")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def generate_timeplot(path):
    gen = generation
    plt.figure()
    plt.plot(gen,time_for_one_gen,color="cyan",label="Temps seconde en moyenne")
    plt.title("Temps en moyenne en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("Temps")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def generate_brainplot(path,num,color):
    bb = [agent0score,agent1score,agent2score,agent3score,agent4score,
           agent5score,agent6score,agent7score,agent8score,agent9score]
    gen = generation
    plt.figure()
    plt.plot(gen, bb[num], color=color, label="Brain " + str(num))  # RGB equivalent of cyan
    plt.title("Brain "+str(num)+" score en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("score")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def generate_meanplot(path):
    gen = generation
    plt.figure()
    plt.plot(gen, mean, color="red", label="Moyenne des agents")
    plt.title("Moyenne des agents en fonction de la génération")
    plt.xlabel("Génération")
    plt.ylabel("Moyenne")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()

def generate_medianplot(path):
    gen = generation
    plt.figure()
    plt.plot(gen, median, color="red", label="Mediane des agents")
    plt.title("Mediane des agents en fonction de la génération")
    plt.xlabel("Génération")
    plt.ylabel("Mediane")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()

def generate_total_ultimate_custom_super_cool_plot(path1, path2, path3, tg, tm, tb, tt):  

    plt.figure()
    for i in range(len(all_paths)): plt.plot(tg[i],tb[i],label=all_paths[i])
    plt.title("Score du meilleur cerveau en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("Score")
    plt.legend()
    plt.grid()
    plt.savefig(path1)
    plt.close()
    
    plt.figure()
    for i in range(len(all_paths)): plt.plot(tg[i], tm[i], label=all_paths[i])
    plt.title("Moyenne des agents en fonction de la génération")
    plt.xlabel("Génération")
    plt.ylabel("Moyenne")
    plt.legend()
    plt.grid()
    plt.savefig(path2)
    plt.close()
    
    
    plt.figure()
    for i in range(len(all_paths)): plt.plot(tg[i],tt[i],label=all_paths[i])
    plt.title("Temps en moyenne en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("Temps")
    plt.legend()
    plt.grid()
    plt.savefig(path3)
    plt.close()
    

def open_plot(path):
    webbrowser.get('windows-default').open('file://'+os.path.realpath(path))

if __name__ == "__main__":
    load_stats("/home/ely/Documents/prog/tipe2026/tools/saves/simu_0_20251128_180409/simu_0_20251128_180409.csv")
    print(median)