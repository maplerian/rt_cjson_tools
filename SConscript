import rtconfig
from building import *

cwd     = GetCurrentDir()
CPPPATH = [cwd, str(Dir('#'))]
src = Glob('*.c')
src = Split(src)

group = DefineGroup('cJSON_Tools', src, depend = [''], CPPPATH = CPPPATH)

Return('group')
