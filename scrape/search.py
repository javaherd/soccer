import os
import urllib

fname = "teams"

with open(fname) as f:
  teams = f.readlines()

for team1 in teams[::2]:
  for team2 in teams[1::2]:
    if team1 != team2:
      os.system("wget https://www.youtube.com/results?search_query=" + urllib.quote_plus(team1 + " " + team2 + " highlights 2014") + " -O searchresults/" + team1.replace(" ", "").strip() + "_" + team2.replace(" ", "").strip())
