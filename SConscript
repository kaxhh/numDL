# For UltraAI SDK Compiling
import os
from build_tools import *

objs = []
pwd  = PresentDir()
list = os.listdir(pwd)

for item in list:
    if os.path.isfile(os.path.join(pwd, item, 'SConscript')):
        objs = objs + SConscript(os.path.join(item, 'SConscript'))

Return('objs')
