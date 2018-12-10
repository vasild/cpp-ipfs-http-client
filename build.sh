[ -d build_dir ] || mkdir build_dir
cd build_dir 
cmake -DJSON_FOR_MODERN_CXX_INCLUDE_DIR:PATH=../json/include ..
make