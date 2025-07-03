from fpdf import FPDF
import json
import stats

def unfs(number):
        return (((50*number+96)%255)/255, ((69*number+24)%255)/255, ((42*number+93)%255)/255)

FPATH = "saved_sims"
FNAME = "test2_20250406_220018-copy-copy"

OPATH = "rapports"
ONAME = FNAME

stats.load_stats(f"{FPATH}/{FNAME}/stats.csv")
stats.generate_scoreplot(f"{FPATH}/{FNAME}/scoreplot.png")
stats.generate_fpsplot(f"{FPATH}/{FNAME}/fpsplot.png")
stats.generate_distanceplot(f"{FPATH}/{FNAME}/distanceplot.png")
stats.generate_distancestartgoalplot(f"{FPATH}/{FNAME}/distancestartgoalplot.png")

for i in range(0, 10):
    stats.generate_brainplot(f"{FPATH}/{FNAME}/brain{i}plot.png", i,unfs(i))

def format_time(seconds):
    """
    Format time in seconds to a string
    :param seconds: int time in seconds
    :return: str formatted time
    """
    if seconds < 60:
        return f"{int(seconds)} s"
    elif seconds < 3600:
        return f"{int(seconds // 60)} min {int(seconds % 60)} s"
    else:
        return f"{int(seconds // 3600)} h {int(seconds % 3600 // 60)} min {int(seconds % 60)} s"
    


def load_sim(path, name):
    """
    load the simulation from disk
    :param path: str path to the folder
    :param name: str name of the file
    :return: dict
    """
    with open(f"{path}/{name}/{name}.json", "r") as file:
        data = json.load(file)
    return data

class PDF(FPDF):
    def __init__(self,name):
        super().__init__()
        self.name = name

    def header(self):
        # Rendering logo:
        #self.image("../docs/fpdf2-logo.png", 10, 8, 33)
        # Setting font: helvetica bold 15
        self.set_font("helvetica", style="B", size=26)
        # Moving cursor to the right:
        #self.cell(60)
        # Printing title:
        self.cell(200, 10, self.name, border=1, align="C")
        # Performing a line break:
        self.ln(20)

    def footer(self):
        # Position cursor at 1.5 cm from bottom:
        self.set_y(-15)
        # Setting font: helvetica italic 8
        self.set_font("helvetica", style="I", size=8)
        # Printing page number:
        self.cell(0, 10, f"Page {self.page_no()}/{{nb}}", align="C")

data = load_sim(FPATH, FNAME)
sg = data["generation"]*11

raw_name = data["name"]
raw_name = raw_name.split("_")
date = raw_name[-2] + " " +raw_name[-1]
del(raw_name[-1])
del(raw_name[-1])
print(raw_name)
name = " ".join(raw_name)


# Instantiation of inherited class
pdf = PDF(name)
pdf.add_page()




# INFO
pdf.set_font("Times", size=20, style="B")
pdf.cell(10)
pdf.cell(30, 10, "Informations globales", border=0, align="C")
pdf.ln(10)

pdf.set_font("Times", size=18)
pdf.cell(0, 10, f"Nom de l'hote : {data['host_name']} ", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Date : {date} ", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Nombre de generations :  {data['generation']}", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Nombre de sous-generations :  {sg}", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Nombre de sessions d'entrainements :  {len(data['train_sessions'])}", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Temps pour chaque session : ", new_x="LMARGIN", new_y="NEXT")
for i in range(0,len(data['train_sessions'])):
    pdf.cell(0, 10, f"            - Session {i+1} : {format_time(data['train_sessions'][i][1]-data['train_sessions'][i][0])}", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Temps total :  {format_time(sum([x[1]-x[0] for x in data['train_sessions']]))}", new_x="LMARGIN", new_y="NEXT")

# PARA
pdf.set_font("Times", size=20, style="B")
pdf.cell(20)
pdf.cell(30, 10, "Parametres de simulation", border=0, align="C")
pdf.ln(10)

pdf.set_font("Times", size=18)
pdf.cell(0, 10, f"Nombre de cerveaux : {data['brains-number']} ", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Nombre d'agents : {data['agents-number']} ", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Temps par simulation : {data['simu_time']} ", new_x="LMARGIN", new_y="NEXT")
pdf.cell(0, 10, f"Facteur d'évolution : {data['evolution']} ", new_x="LMARGIN", new_y="NEXT")

h=30
pdf.add_page()
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/scoreplot.png",w=100)
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/fpsplot.png",x=110,w=100)

h+=80
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/distanceplot.png",w=100)
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/distancestartgoalplot.png",x=110,w=100)

pdf.add_page()
h=30
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain0plot.png",w=100)
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain1plot.png",x=110,w=100)
h+=80
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain2plot.png",w=100)
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain3plot.png",x=110,w=100)
h+=80
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain4plot.png",w=100)
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain5plot.png",x=110,w=100)

pdf.add_page()
h=30
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain6plot.png",w=100)
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain7plot.png",x=110,w=100)
h+=80
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain8plot.png",w=100)
pdf.set_y(h)
pdf.image(f"{FPATH}/{FNAME}/brain9plot.png",x=110,w=100)

pdf.output(OPATH+"/"+ONAME+".pdf")
