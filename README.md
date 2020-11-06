# DAF-Bratva

## Compiling

To compile test executables :

```
cd build
rm -rf *   # be careful with this
cmake ..
make
```

To compile documentation :
```
cd build
rm -rf * # be careful
cmake ../documentation
make
```

You can navigate through the documentation for instance using firefox :
```
firefox ../documentation/html/index.html
```


## Git cheatsheet

Copier le repertoire sur sa machine // Copy the directory on your computer

`git clone "path given on line"`

Si vous cassez tout git don't worry, on supprime son répertoire local et on
reclone // if how have melted down the repo, no worries start again using :

`rm -rf daf-bratva  && git clone git@gitlab.com:mlotta/daf-bratva.git`

Pour récupérer le récupertoire distant // To get the directory whis is on the server :
`git pull`

Pour ajouter des fichiers à la liste de ceux qu'on veut effectivement modifier
sur le répertoire distant, cad ceux qu'on veut mettre à jour avec un commit // To add files to the list of files you want to be updated online :

`git add "file_name"
`
ou si on veut tout mettre sans se prendre la tete // or if you want to add all of them :

`git add .`

Pour valider cette mise à jour // to approve this update :

`git commit -m "some comments"`

Pour mettre en ligne sur le répertoire distant // To put on line on the directory which is on your laptop : 
`git push`

Donc a chaque fois qu'on veut mettre à jour les modifications qu'on a faite // To sum up, each time you want to modify smth :
```
git add .
git commit -m "what i have changed"
git pull
git push
```

Un fichier `.gitignore` permet de lister tout les fichiers qu'on veut toujours
ignorer, cad ceux qui sont sur notre machine mais qu'on ne veut pas mettre en
ligne. Par exemple les fichiers assez lourd (on évite de mettre les fichiers
lourd sur le répertoire genre image ou gros executable parce qu'il faut tout
retraiter à chaque pull/push et c'est chiant) ou qui servent à rien.

A n'importe quel moment, pour connaître le status des fichiers qu'on a changé et
ce qui à changé sur le répertoire distant depuis notre dernier pull : `git status`

A `.gitignore` file allows you to list all the files you always want
ignore, ie those who are on our machine but we do not want to put
line. For example, files that are quite heavy (we avoid putting heavy files
heavy like pics or large executable because you would have to reprocess everything each pull / push and it's time/energy consuming) or those that are useless.

At any time, to know the status of the files that we have changed and
what has changed on the remote directory since our last pull: `git status`


## Opencv install

You might want to install opencv on your computer for the project. As a warning,
even with the instructions it can take quite a while ! I'll just share some 
minimal installation instructions which worked for me but it might be "improvable".
I hope this will be enough for most of us, with no bug whatsoever

I mostly used this link and I'll copy paste the most relevant lines:
https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html

Open a terminal and install the first dependancies (Roman have fun with homebrew):
```
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
```

Chose someplace in your computer, then :
```
wget https://github.com/opencv/opencv/archive/4.2.0.zip 
unzip 4.2.0.zip
```

will download opencv and extract it.
Run this :
```
cd opencv-4.2.0
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
```

and :
```
make -j4 # runs 4 jobs in parallel
```
This might take quite a long time and burn your computer perhaps. If this is the
case, call the 18, then use : `-j2` instead, which should be better but longer.

Then :
```
sudo make install
```

It should be good enough. If no error has appeared until there, you may
download an [empreinte genitale](http://stateuic.habite.la/drawings/517.0.png) and compile this code :

```
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv){
    Mat image = imread("empreinte_genitale.png");
    return 0;
}
```
with the command line :
```
g++ your_cpp_file.cpp -o app_temp `pkg-config --cflags --libs opencv`

```
When executing, if there is no error you're good !

There is [this](https://www.opencv-srf.com/p/introduction.html) real nice website for real nice tutorials.
In particular [this part](https://www.opencv-srf.com/2017/11/load-and-display-image.html) is quite useful to load and display !


## USEFUL DEBUGGER

Useful debugger to locate leaks : valgrind and cgdb

Install :
```
sudo apt-get install valgrind 
sudo apt-get install cgdb
```

valgrind helps you locate where the leaks in your program are, and displays the errors in the file "debug.txt

```
valgrind --leak-check=full --log-file="debug.txt" ./starter_2
```
