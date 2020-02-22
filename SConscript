import rtconfig
from building import *

cwd     = GetCurrentDir()
CPPPATH = [cwd, str(Dir('#'))]
src = Glob('src/*.c')
src = Split(src)

if GetDepend(['PKG_USING_RT_CJSON_TOOLS_EXAMPLE']):
    src += Glob('example/example1.c')

group = DefineGroup('cJSON_Tools', src, depend = ['PKG_USING_RT_CJSON_TOOLS'], CPPPATH = CPPPATH)

Return('group')
