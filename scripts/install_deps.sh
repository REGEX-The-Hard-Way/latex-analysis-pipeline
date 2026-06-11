    sudo apt install wget 
  
    cd /tmp/
    git clone https://github.com/adrian-thurston/colm-suite
    cd colm-suite
    sudo apt install autoconf
    sudo apt install libtool
    ./autogen.sh
    ./configure
    make
    sudo make install
    sudo ldconfig

    cd /tmp/
    git clone https://github.com/igraph/igraph
    cd igraph
    sudo apt install flex bison
    mkdir build
    cd build 
    cmake .. 
    make 
    sudo make install
    sudo ldconfig
