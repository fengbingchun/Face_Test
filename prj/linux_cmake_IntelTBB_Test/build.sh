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

# build intel tbb
echo "############## start build intel tbb:"
tbb_path=${dir_name}/../../src/Intel_TBB
cd ${tbb_path}
make
echo "############## build intel tbb finish"

cp -a ${tbb_path}/build/linux_intel64_gcc_cc4.9.4_libc2.19_kernel3.16.0_release/*.so  ${new_dir_name}
cp -a ${tbb_path}/build/linux_intel64_gcc_cc4.9.4_libc2.19_kernel3.16.0_release/*.so.2  ${new_dir_name}

cd ${new_dir_name}
cmake ..
make

cd -

