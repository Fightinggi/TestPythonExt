from distutils.core import *
setup(
    name="mymod",
    version="1.0",
    ext_modules=[Extension("mymod", ["mymod.c"])]
)
# cmd F:\WIN10SDK\download\Python-3.7.0\python.exe 执行 python setup install
# 生成build dist mymod.egg-info三个目录
# copying build\lib.win-amd64-3.9\mymod.cp39-win_amd64.pyd -> build\bdist.win-amd64\egg
# copying mymod.egg-info\PKG-INFO -> build\bdist.win-amd64\egg\EGG-INFO
# copying mymod.egg-info\SOURCES.txt -> build\bdist.win-amd64\egg\EGG-INFO
# copying mymod.egg-info\dependency_links.txt -> build\bdist.win-amd64\egg\EGG-INFO
# copying mymod.egg-info\top_level.txt -> build\bdist.win-amd64\egg\EGG-INFO
# byte-compiling build\bdist.win-amd64\egg\mymod.py to mymod.cpython-39.pyc
# creating 'dist\mymod-1.0-py3.9-win-amd64.egg' and adding 'build\bdist.win-amd64\egg' to it
# removing 'build\bdist.win-amd64\egg' (and everything under it)
# creating F:\WIN10SDK\download\Python-3.7.0\Lib\site-packages\mymod-1.0-py3.9-win-amd64.egg
# Extracting mymod-1.0-py3.9-win-amd64.egg to F:\WIN10SDK\download\Python-3.7.0\Lib\site-packages



