# Ubuntu 18.04 environment setup
```bash
sudo apt-get install git build-essential cmake
```

# build
mkdir build   
cd build   
cmake ..  
make

cd ./src   
(now copy the lib folder from src to exec directory)    
cp -r ../../src/lib .   
./pipelog  
