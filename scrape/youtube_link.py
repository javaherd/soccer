import re
import os

for fn in os.listdir('searchresults'):
    with open('searchresults/' + fn) as f:
      for line in f:
        s=re.search(r'watch\?v=.{11}',line)
        if s:
           print(s.group()[-11:])
           break
