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

def clear_all():
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


def load_stats(path):
    if not os.path.exists(path):
        print(f"[ERROR] Stats : The file {path} does not exist.")
        return
    
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


def open_plot(path):
    webbrowser.get('windows-default').open('file://'+os.path.realpath(path))
