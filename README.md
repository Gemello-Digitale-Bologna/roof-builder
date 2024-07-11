# Struttura
```
assets (Input and Temp files)
├── <buildings_file>.csv
├── lidar
│   ├── <lidar>.las
│   └── [...]
└── temp (IO files C++/Python)
    ├── edge_IO.npy
    ├── edge_mat.npy
    ├── lidar_temp.npy
    └── ridge_IO.npy
output (Output files)
├── build_log.txt
├── exec_log.txt
└── <output_mesh>.stl
config (dotenv config files)
├── config_docker.env
└── config.env
src (contains all code)
├── Main.cpp
├── Program.h
├── Program.cpp
├── Building.cpp
├── Building.h
├── Geometry.h
├── npy.hpp
├── Timer.h
├── Dbscan
│   ├── Dbscan.cpp
│   └── Dbscan.h
├── Grid
│   ├── Grid.cpp
│   └── Grid.h
├── nanoflann
│   └── nanoflann.hpp
├── py
│   ├── cv_pipeline_edge.py
│   ├── cv_pipeline_ridge.py
│   ├── lasToNpy.py
│   └── PyCaller.h
├── Readers
│   ├── CsvReader.cpp
│   ├── CsvReader.h
│   ├── NpyReader.cpp
│   ├── NpyReader.h
│   └── Reader.h
├── Triangle
│   ├── triangle.c
│   ├── triangle.h
│   ├── TriangleWrap.cpp
│   └── TriangleWrap.h
└── Writers
    ├── NpyWriter.h
    ├── StlWriter.h
    └── StreamWriter.h
third_party (contains cpp-dotenv)
```

# Esecuzione
Al momento questa versione di ***test*** esegue unicamente sul file `32_684000_4930000.las`, che va inserito nella cartella `/assets/lidar`.\
Va inoltre inserito il file `.csv` relativo ai buildings nella cartella `/assets`.

Infine, è necessario eseguire il seguente comando nella cartella `third_party`:
```
git clone https://github.com/adeharo9/cpp-dotenv
```

## Locale
Per una run in locale è possibile urtilizzare lo script `build-and-run.sh`.\
**NOTA:** In questo caso è necessario installare le librerie necessarie in locale o in un enviroment specializzato (vedi `apt_requirements.txt` e `py_requirements.txt`).

## Docker
Per una run su Docker basta eseguire il comando:
```
docker compose up --build
```

## Singularity
Per una run Singularity si consiglia di eseguire i file `.sh` predisposti per le fasi di *build* e *run*/*schedule* (quest'ultimo configurato per un esecuzione su Leonardo).