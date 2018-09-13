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

# build seetaface
echo "############## start build seetaface:"
seetaface_path=${dir_name}/../../src/SeetaFaceEngine

echo "build detect module:"
seetaface_detect_path=${seetaface_path}/FaceDetection
mkdir -p ${seetaface_detect_path}/build
cd ${seetaface_detect_path}/build
cmake ..
make

echo "build align module:"
seetaface_align_path=${seetaface_path}/FaceAlignment
mkdir -p ${seetaface_align_path}/build
cd ${seetaface_align_path}/build
cmake -DBUILD_EXAMPLES=OFF ..
make

echo "build identification module:"
seetaface_identification_path=${seetaface_path}/FaceIdentification
mkdir -p ${seetaface_identification_path}/build
cd ${seetaface_identification_path}/build
cmake ..
make

echo "############## build seetaface finish"

ln -s ${seetaface_detect_path}/build/libseeta_facedet_lib.so  ${new_dir_name}
ln -s ${seetaface_align_path}/build/libseeta_fa_lib.so ${new_dir_name}
cp -a ${seetaface_identification_path}/build/libviplnet.so  ${new_dir_name}
cp -a ${seetaface_identification_path}/build/libviplnet.so.4.5  ${new_dir_name}

cp -a ${seetaface_detect_path}/model/seeta_fd_frontal_v1.0.bin ${dir_name}/${data_dir}
cp -a ${seetaface_align_path}/model/seeta_fa_v1.1.bin ${dir_name}/${data_dir}
cp -a ${seetaface_identification_path}/model/*.rar ${dir_name}/${data_dir}
cd ${dir_name}/${data_dir}
unrar x -e -y seeta_fr_v1.0.part1.rar
unrar x -e -y seeta_fr_v1.0.part2.rar
cp -a ${seetaface_identification_path}/data ${dir_name}/${data_dir} 

cd ${new_dir_name}
cmake ..
make

cd -

