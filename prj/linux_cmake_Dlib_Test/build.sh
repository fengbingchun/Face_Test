#! /bin/bash

real_path=$(realpath $0)
dir_name=`dirname "${real_path}"`
echo "real_path: ${real_path}, dir_name: ${dir_name}"

data_dir="testdata"
if [ -d ${dir_name}/${data_dir} ]; then
	rm -rf ${dir_name}/${data_dir}
fi

ln -s ${dir_name}/./../../${data_dir} ${dir_name}

new_dir_name=${dir_name}/build
mkdir -p ${new_dir_name}
cd ${new_dir_name}
echo "pos: ${new_dir_name}"
if [ "$(ls -A ${new_dir_name})" ]; then
	echo "directory is not empty: ${new_dir_name}"
	rm -r *
else
	echo "directory is empty: ${new_dir_name}"
fi

# build dlib
echo "############## start build dlib:"
dlib_path=${dir_name}/../../src/dlib
mkdir -p ${dlib_path}/build
cd ${dlib_path}/build
cmake -DDLIB_USE_CUDA=OFF -DDLIB_USE_BLAS=OFF -DDLIB_USE_LAPACK=OFF -DDLIB_JPEG_SUPPORT=ON -DDLIB_PNG_SUPPORT=ON ..
make
echo "############## build dlib finish"

cp -a ${dlib_path}/build/dlib/libdlib.a  ${new_dir_name}
cp -a ${dlib_path}/build/dlib/config.h ${dlib_path}/dlib
#cp -a ${dlib_path}/build/dlib/libdlib.so.19.1.0  ${new_dir_name}
#ln -s ${new_dir_name}/libdlib.so.19.1.0 ${new_dir_name}/libdlib.so
cp -a ${dlib_path}/data/shape_predictor_68_face_landmarks.dat ${dir_name}/${data_dir}

cd ${new_dir_name}
cmake -DDLIB_USE_BLAS=OFF -DDLIB_USE_LAPACK=OFF -DDLIB_JPEG_SUPPORT=ON -DDLIB_PNG_SUPPORT=ON ..
make

cd -

