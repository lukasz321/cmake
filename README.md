# build
mkdir build   
cd build   
cmake ..  
make

cd ./src
(now copy the lib folder from src to exec directory)
cp -r ../../src/lib .
./pipelog  
