#Kompilacia#

  - v priecinku build je project file pre visual studio 2012
    (build subory dalsich alternativnych vyvojovych prostredi (napr. `Qt Creator`,
     alebo `Visual studio 2010`) by bolo tiez dobre davat sem)
  - okrem toho bude este potrebne nastavit premennu `MY_LIB_PATH` na adresar,
    v ktorom su vsetky potrebne kniznice
  - pokial budu potrebne nejake ine nastavenia, tak bude dobre ak v priecinku
    build vytvorite adresar so svojim build suborom (napr. `msvc2012-NVidia`, ...)
    V tomto pripade vsak treba zeditovat aj `.gitignore`, aby sa sa do repozitara
    nekomitovali hluposti (objektove subory, nastavenia visual studia, ...)
  - u seba som nastavoval tieto veci v properties projektu:
     1.) `Configuration properties` | `Environment`
          - tu som do premennej `PATH` cestu ku vsetkym kniniciam
          
     2.) `C/C++` | `General` | `Additional Include Directories`
          - tu som pridal cestu ku vsetkym hlavickovym suborom
          
     3.) `C/C++` | `General` | `Additional Library Directories`
         - tu som pridal cestu ku vsetkym *.lib suborom 
         
     4.) `C/C++` | `Input` | `Additional Dependancies`
         - tu som pridal vsetky potrebne kniznice
         
  - Zdrojaky z priecinka `src` som pridal cez `Solution Explorer` cez menu `Add`
  
#Pouzite kniznice a frameworky#

  - SDL 2.0.1
  - SDL2_image 2.0.0
  - SDL2_ttf 2.012
  - OpenGL
  - glew 1.9.0
  - glm
  - OpenCL (AMD APP SDK)