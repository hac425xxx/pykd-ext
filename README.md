# Pykd-Ext - bootstarpper to run any python with WinDBG

Pykd-Ext is not Pykd. Pykd-Ext is a windbg extension that runs python that runs pykd. With Pykd-Ext you can choose and run any version of python. 

Using Pykd-Ext - is recommended way to use pykd with windbg. 

#### Download

[All releases](/../wikis/Downloads)

#### Installation pykd.dll
1. Locate your windbg installation place ( for example: C:\Program Files (x86)\Windows Kits\10\Debuggers\x64 )
2. Unpack pykd.dll to the 'ext' directory.

#### Loading pykd.dll:
Run command:
.load pykd

#### Showing help for pykd bootstrapper:

```
0:000> !pykd.help

usage:

!help
	print this text

!info
	list installed python interpreters

!select version
	change default version of a python interpreter

!py [version] [options] [file]
	run python script or REPL

	Version:
	-2           : use Python2
	-2.x         : use Python2.x
	-3           : use Python3
	-3.x         : use Python3.x

	Options:
	-g --global  : run code in the common namespace
	-l --local   : run code in the isolated namespace
	-m --module  : run module as the __main__ module ( see the python command line option -m )

	command samples:
	"!py"                          : run REPL
	"!py --local"                  : run REPL in the isolated namespace
	"!py -g script.py 10 "string"" : run a script file with an argument in the commom namespace
	"!py -m module_name" : run a named module as the __main__

!pip [version] [args]
	run pip package manager

	Version:
	-2           : use Python2
	-2.x         : use Python2.x
	-3           : use Python3
	-3.x         : use Python3.x

	pip command samples:
	"pip list"                   : show all installed packagies
	"pip install pykd"           : install pykd
	"pip install --upgrade pykd" : upgrade pykd to the latest version
	"pip show pykd"              : show info about pykd package


```

#### List all available python core

```
0:000> !pykd.info

Installed python

Version:        Status:     Image:
------------------------------------------------------------------------------
* 2.7 x86-64    Unloaded    C:\Windows\SYSTEM32\python27.dll
  3.5 x86-64    Unloaded    C:\Users\user\AppData\Local\Programs\Python\Python35\python35.dll
```

#### Running scripts or REPL:

* To run REPL with the default python:
```
!py
```
* To run REPL with the specified python:
```
!py -3
```
* To run script with the default python:
```
!py script.py arg1 arg2
```
* To run script with the specified python: 
```
!py -3.5 script.py arg1 arg2
```
* To run  REPL in local scope ( all objects will be destruct after quit() ):
```
!py --local
```
* To run script in global scope ( all objects stay to live ): 
```
!py --global script.py 
```
* You can use "shebang line" to note python version. Insert first line to a script file:
```
 #! python2
```
Or
```
 #! python3.5
```
* To run module or package as a script 
```
!py -m timeit
!py -m my_package.module
```

#### Managing python packages:

Show package list
```
0:000> !pip -3.5 list
pip (7.1.2)
pykd (0.3.1.1)
setuptools (18.2)
wheel (0.29.0)
```
Show package info:
```
0:000> !pip -3.5 show pykd
---
Metadata-Version: 2.0
Name: pykd
Version: 0.3.1.1
Summary: python windbg extension
Home-page: UNKNOWN
Author: UNKNOWN
Author-email: UNKNOWN
License: UNKNOWN
Location: c:\users\user\appdata\local\programs\python\python35\lib\site-packages
Requires: 
```
Install or upgrade package:
```
0:000> !pip -3.5 install --upgrade pykd
Collecting pykd
  Downloading pykd-0.3.1.3-cp35-none-win_amd64.whl (3.3MB)
Installing collected packages: pykd
  Found existing installation: pykd 0.3.1.1
    Uninstalling pykd-0.3.1.1:
      Successfully uninstalled pykd-0.3.1.1
Successfully installed pykd-0.3.1.3
```