g++ box_box_scalar.cpp -O2 -std=c++11 -march=native -o scalar
g++ box_box_simd.cpp -O2 -std=c++11 -march=native -msse -o simd
