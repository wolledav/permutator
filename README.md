# metaopt

Compilation and running tests
```
cd path/to/metaopt
cmake -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles" -S . -B"./cmake-build-release"
cmake --build cmake-build-release --target all -- -j 6
cd tests
./test_cvrp_small.sh
```


Gurobi installation.

Add to .bashrc:
```
export GUROBI_HOME="/opt/gurobi951/linux64"
export PATH="${PATH}:${GUROBI_HOME}/bin"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${GUROBI_HOME}/lib"
```

then 

```
cd /opt/gurobi951/linux64/src/build
make
cp libgurobi_c++.a ../../lib/
```

