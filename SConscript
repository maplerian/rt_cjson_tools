import rtconfig
from building import *

cwd     = GetCurrentDir()
CPPPATH = [cwd + '/src']
src = Glob('src/*.c')
src = Split(src)

if GetDepend(['RT_CJSON_TOOLS_ENABLE_EXAMPLE']):
    src += Glob('example/example1.c')

group = DefineGroup('cJSON_Tools', src, depend = ['PKG_USING_RT_CJSON_TOOLS'], CPPPATH = CPPPATH)

Return('group')
