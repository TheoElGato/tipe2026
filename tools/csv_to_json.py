import csv
import json

# expected CSV format:
"""     "Id": int,
        "Nom (Nom du fichier)": str,
        "Description": str,
        "Etat": str (Pas fait / En cours / Fait / cpge / Réservé "nom"),
        "Date de simulation": str,
        "Lien du rapport": str,
        "Nombre de post": int,
        "Threads": int,
        "Temps par sous-simulation": int,
        "Evolution": float,
        "Nombre de cerveaux": int,
        "Nombre d'agent (autofill)": int,
        "Nombre d'hidden layer": int,
        "NB sous gen": int,
        "% Agents gardés": int,
        "% Agent mutés": int,
        "use curve":  (FALSE / TRUE),
        "Param A": float,
        "Param B": float,
        "Param C": float,
        "Param D": float
"""

# json format:
"""	
    "SIM_NAME": str,

	"DEVICE": ("cpu" / "gpu"),
	"THREADS": int,

	"LOAD_FROM_FILE": bool,
	"LOAD_NAME": str,

	"SIM_TIME": int,
	"EVOLUTION": float,
	"NB_BRAIN": int,
	"NB_AGENT": "THIS IS AUTOMATIC : NB_BRAIN*THREADS",
	"NB_HIDDEN_LAYER": int,
	"SOUS_SIM": int,
	"BEST_KEEP": int,
	"SELECTION_POL": int,

	"AUTOSAVE": bool,
	"AUTOSAVE_FREQ": int,

	"MAXDIST": int,
	"MINDIST": int,
	"NB_GOAL": int,

	"STARTX": int,
	"STARTY": int,

	"USE_EVOLUTION_CURVE": bool,
	"CURVE_A": float,
	"CURVE_B": float,
	"CURVE_C": float,
	"CURVE_D": float,

	"IS_INFINITE": bool,
	"TIME_ALLOWED": int
"""


with open('data.csv', mode='r', newline='', encoding='utf-8') as csv_file:
    csv_reader = csv.DictReader(csv_file)
    data = [row for row in csv_reader]

n = len(data)
for i in reversed(range(n)):
    if data[i]['Etat'] != 'cpge':
        data.pop(i)
        n -= 1
    else:
        # Convert types 
        # rename fields
        # and set default values where needed
        # this is ugly i know

        data[i]['SIM_NAME'] = 'simu_' + str(data[i]['Id'])

        data[i]['DEVICE'] = 'cpu'
        data[i]['THREADS'] = int(data[i]['Threads'])
        

        data[i]['LOAD_FROM_FILE'] = False
        data[i]['LOAD_NAME'] = ''
        data[i]['SIM_TIME'] = int(data[i]['Temps par sous-simulation'])
        data[i]['EVOLUTION'] = float(data[i]['Evolution'].replace(',','.'))
        data[i]['BRAIN_ACC'] = 4
        data[i]['NB_BRAIN'] = int(data[i]['Nombre de cerveaux'])
        data[i]['NB_AGENT'] = "THIS IS AUTOMATIC : NB_BRAIN*THREADS"
        data[i]['NB_HIDDEN_LAYER'] = int(data[i]['Nombre d\'hidden layer'])
        data[i]['SOUS_SIM'] =  int(data[i]['NB sous gen'])
        data[i]['BEST_KEEP'] = int(data[i]['% Agents gardés'])
        data[i]['SELECTION_POL'] = int(data[i]['% Agent mutés'])
        

        data[i]['AUTOSAVE'] = True
        data[i]['AUTOSAVE_FREQ'] = 4

        data[i]['MAXDIST'] = 300
        data[i]['MINDIST'] = 200
        data[i]['NB_GOAL'] = 3

        data[i]['STARTX'] = 523
        data[i]['STARTY'] = 275


        data[i]['USE_EVOLUTION_CURVE'] = True if data[i]['use curve'].upper() == 'TRUE' else False
        data[i]['PARAM_A'] = float(data[i]['Param A'].replace(',','.'))
        data[i]['PARAM_B'] = float(data[i]['Param B'].replace(',','.'))
        data[i]['PARAM_C'] = float(data[i]['Param C'].replace(',','.'))
        data[i]['PARAM_D'] = float(data[i]['Param D'].replace(',','.'))
        

        data[i]['IS_INFINITE'] = True
        data[i]['TIME_ALLOWED'] = 126000

        # Remove unused fields
        del data[i]['Id']
        del data[i]['Threads']
        del data[i]['Temps par sous-simulation']
        del data[i]['Nombre de post']
        del data[i]['Evolution']
        del data[i]['Description']
        del data[i]['Nombre de cerveaux']
        del data[i]['Etat']
        del data[i]['Nombre d\'hidden layer']
        del data[i]['Date de simulation']
        del data[i]['Lien du rapport']
        del data[i]['Nom (Nom du fichier)']
        del data[i]['NB sous gen']
        del data[i]["Nombre d'agent (autofill)"]
        del data[i]['% Agents gardés']
        del data[i]['use curve']
        del data[i]['% Agent mutés']
        del data[i]['Param A']
        del data[i]['Param B']
        del data[i]['Param C']
        del data[i]['Param D']
        del data[i]['brain_acc']


# write to json file
with open('data.json', mode='w', encoding='utf-8') as json_file:
    json.dump(data, json_file, indent=4)






