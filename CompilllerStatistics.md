## Different Compilller Statistics ##

## UrtConnector compiling speed ##

| **Compiller** | **Speed** | **Size** | **Comment** |
|:--------------|:----------|:---------|:------------|
| cl            | 8 min     | 2 MB     |             |
| mingw         | 7 min     | 11 MB    |             |
| gcc           | 3.47 min     | 3 MB     | one thread but CPU little powerful|

## Boost static library size ##

| **Compiller** | **Files** | **Size** | **Comment** |
|:--------------|:----------|:---------|:------------|
| cl            | 27        | 139 MB   |python present - 5 Mb|
| mingw         | 26        | 24 MB    |python missing|
| cygwin        | 25        | 20 MB    |**math\_c99l** and **math\_tr1l** missing|
| gcc           | 27        | 22 MB    |compiling **5** times faster than other|

## Qt4 static library size ##

| **Compiller** | **Files** | **Size** | **Comment** |
|:--------------|:----------|:---------|:------------|
| cl            | 20        | 22 MB    |             |
| mingw         | 21        | 26 MB    |             |
| cygwin        | 7         | 32 MB    | Qt3Support, Core, Gui, Network, Xml, Sql, Text |

## Qt4 dynamic library size ##

| **Compiller** | **Files** | **Size** | **Comment** |
|:--------------|:----------|:---------|:------------|
| cl            | 16        | 31 MB    |             |
| mingw         | 16        | 34 MB    |             |
| cygwin        | ??        | ??       |             |
| gcc           | 16        | 39 MB    |             |