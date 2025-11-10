import matplotlib.pyplot as plt
import webbrowser
import os
import csv


generation = [] # Contain a tuple

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

best_agent_score = []
best_agent_distance = []

goal_pos = [] # Contain a tuple
start_pos = [] # Contain a tuple
distance_start_goal = []

average_fps = []


def load_stats(path):
    if not os.path.exists(path):
        print(f"[ERROR] Stats : The file {path} does not exist.")
        return
    
    with open(path, mode='r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        for row in reader:
            generation.append((int(row[0]), int(row[1])))
            agent0score.append(float(row[2]))
            agent1score.append(float(row[3]))
            agent2score.append(float(row[4]))
            agent3score.append(float(row[5]))
            agent4score.append(float(row[6]))
            agent5score.append(float(row[7]))
            agent6score.append(float(row[8]))
            agent7score.append(float(row[9]))
            agent8score.append(float(row[10]))
            agent9score.append(float(row[11]))
            best_agent_score.append(float(row[12]))
            best_agent_distance.append(float(row[13]))
            goal_pos.append((float(row[14]), float(row[15])))
            start_pos.append((float(row[16]), float(row[17])))
            distance_start_goal.append(float(row[18]))
            average_fps.append(float(row[19]))


def save_stats(gen, agents, ba_score, ba_distance, goal, start, distance, fps, path):
    generation.append(gen)

    agent0score.append(agents[0])
    agent1score.append(agents[1])
    agent2score.append(agents[2])
    agent3score.append(agents[3])
    agent4score.append(agents[4])
    agent5score.append(agents[5])
    agent6score.append(agents[6])
    agent7score.append(agents[7])
    agent8score.append(agents[8])
    agent9score.append(agents[9])

    best_agent_score.append(ba_score)
    best_agent_distance.append(ba_distance)

    goal_pos.append(goal)
    start_pos.append(start)
    distance_start_goal.append(distance)

    average_fps.append(fps)

    with open(path+'/stats.csv', mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Generation', 'SousGeneration','Agent0Score', 'Agent1Score', 'Agent2Score', 'Agent3Score', 
                         'Agent4Score', 'Agent5Score', 'Agent6Score', 'Agent7Score', 'Agent8Score', 
                         'Agent9Score', 'BestAgentScore', 'BestAgentDistance', 'GoalPosX', 'GoalPosY',
                         'StartPosX', 'StartPosY', 'DistanceStartGoal', 'AverageFPS'])
        for i in range(len(generation)):
            writer.writerow([
                generation[i][0], generation[i][1],
                agent0score[i], agent1score[i], agent2score[i], agent3score[i], agent4score[i],
                agent5score[i], agent6score[i], agent7score[i], agent8score[i], agent9score[i],
                best_agent_score[i], best_agent_distance[i],
                goal_pos[i][0],goal_pos[i][1], start_pos[i][0], start_pos[i][1], distance_start_goal[i],
                average_fps[i]
            ])


def generate_scoreplot(path):
    gen = [x[0] for x in generation]
    plt.figure()
    plt.plot(gen,best_agent_score,color="orange",label="Meilleur score")
    plt.title("Score du meilleur cerveau en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("Score")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def generate_fpsplot(path):
    gen = [x[0] for x in generation]
    plt.figure()
    plt.plot(gen,average_fps,color="cyan",label="Images par seconde en moyenne")
    plt.title("FPS en moyenne en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("FPS")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def generate_brainplot(path,num,color):
    bb = [agent0score,agent1score,agent2score,agent3score,agent4score,
           agent5score,agent6score,agent7score,agent8score,agent9score]
    gen = [x[0] for x in generation]
    plt.figure()
    plt.plot(gen, bb[num], color=color, label="Brain " + str(num))  # RGB equivalent of cyan
    plt.title("Brain "+str(num)+" score en fonction de la generation")
    plt.xlabel("Generation")
    plt.ylabel("FPS")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def generate_distanceplot(path):
    gen = [x[0] for x in generation]
    plt.figure()
    plt.plot(gen, best_agent_distance, color="red", label="Distance entre le meilleur agent et le but")
    plt.title("Distance entre le meilleur agent et le but en fonction de la génération")
    plt.xlabel("Génération")
    plt.ylabel("Distance")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def generate_distancestartgoalplot(path):
    gen = [x[0] for x in generation]
    plt.figure()
    plt.plot(gen, distance_start_goal, color="green", label="Distance entre le meilleur debut et le but")
    plt.title("Distance entre le point de départ et le but en fonction de la génération")
    plt.xlabel("Génération")
    plt.ylabel("Distance")
    plt.legend()
    plt.grid()
    plt.savefig(path)
    plt.close()


def open_plot(path):
    webbrowser.get('windows-default').open('file://'+os.path.realpath(path))
