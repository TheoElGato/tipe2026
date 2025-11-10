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
best_agent_score = []
time_for_one_gen = []

gen_max = 0
current_path = ""
all_generation = []
all_mean = []
all_best_agent_score = []
all_time_for_one_gen = []
all_paths = []


def clear_all():
    global gen_max, current_path
    gen_max = max(generation[-1],gen_max)
    all_generation.append(generation)
    all_mean.append(mean)
    all_best_agent_score.append(best_agent_score)
    all_time_for_one_gen.append(time_for_one_gen)
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
    mean.clear()
    best_agent_score.clear()
    time_for_one_gen.clear()


def load_stats(path,name=""):
    global current_path
    if not os.path.exists(path):
        print(f"[ERROR] Stats : The file {path} does not exist.")
        return
    current_path = name
    with open(path, mode='r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        for row in reader:
            generation.append((float(row[0])))
            agent0score.append(float(row[1]))
            agent1score.append(float(row[2]))
            agent2score.append(float(row[3]))
            agent3score.append(float(row[4]))
            agent4score.append(float(row[5]))
            agent5score.append(float(row[6]))
            agent6score.append(float(row[7]))
            agent7score.append(float(row[8]))
            agent8score.append(float(row[9]))
            agent9score.append(float(row[10]))
            mean.append(float(row[11]))
            best_agent_score.append(float(row[12]))
            time_for_one_gen.append(float(row[13]))


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
    plt.ylabel("FPS")
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


def generate_total_ultimate_custom_super_cool_plot(path1,path2,path3):
    gen = [x for x in range(int(gen_max))]
    
    plt.figure()
    for i in range(len(all_paths)): plt.plot(all_generation[i],all_best_agent_score[i],label=all_paths[i])
    plt.title("Score du meilleur cerveau en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("Score")
    plt.legend()
    plt.grid()
    plt.savefig(path1)
    
    
    plt.figure()
    for i in range(len(all_paths)): plt.plot(all_generation[i], all_mean[i], label=all_paths[i])
    plt.title("Moyenne des agents en fonction de la génération")
    plt.xlabel("Génération")
    plt.ylabel("Moyenne")
    plt.legend()
    plt.grid()
    plt.savefig(path2)
    
    
    plt.figure()
    for i in range(len(all_paths)): plt.plot(all_generation[i],all_time_for_one_gen[i],label=all_paths[i])
    plt.title("Temps en moyenne en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("Temps")
    plt.legend()
    plt.grid()
    plt.savefig(path3)
    
   
    plt.show()
    

def open_plot(path):
    webbrowser.get('windows-default').open('file://'+os.path.realpath(path))
