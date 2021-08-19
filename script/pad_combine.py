import cv2
import numpy as np
import os
from os.path import join
from os import listdir
from argparse import ArgumentParser
import os
import random
import csv

def parse_args():

	parser = ArgumentParser()
	parser.add_argument('src', type=str, default=None)
	args = parser.parse_args()
	return args.src

if __name__ == '__main__':
	
	#pad test
	# cls_set = [
	# 	"02691156",
	# 	# "02828884",
	# 	# "02933112",
	# 	"02958343",
	# 	# "03001627",
	# 	# "03211117",
	# 	"03636649",
	# 	# "03691459",
	# 	# "04090263",
	# 	# "04256520",
	# 	"04379243",
	# 	# "04401088",
	# 	# "04530566"
	# ]
	# for c in range(1, 3):
	# 	cls_name = cls_set[c]
	# 	src = "D:/Data/ShapeNetRendering_test_res/%s_256/processed"%cls_name
	# 	src_sym = "D:/Data/ShapeNetRendering_test_res/%s_256/sym"%cls_name
	# 	dst_sym = "D:/Project/3DFront2Back/learn/datasets/%s_256_sym/test"%cls_name
	# 	dst = "D:/Project/3DFront2Back/learn/datasets/%s_256/test"%cls_name
	# 	ref_dir = "D:/Data/ShapeNet_test/input/%s"%cls_name

	# 	if os.path.isdir(dst):
	# 		pass
	# 	else:
	# 		os.mkdir(dst)

	# 	if os.path.isdir(dst_sym):
	# 		pass
	# 	else:
	# 		os.mkdir(dst_sym)
		
	# 	items = os.listdir(ref_dir)

	# 	for item in items:
	# 	# for i in range(1, 1356):
	# 		item_name = item[:-8]
	# 		print(item_name)
	# 		# item_name = "%d"%i
	# 		f_normal = join(src, "%s_normal.png"%item_name)
	# 		f_depth = join(src, "%s_depth.png"%item_name)

	# 		f_sym_depth = join(src_sym, "%s_depth.png"%item_name)
	# 		f_sym_normal = join(src_sym,  "%s_normal.png"%item_name)

	# 		# im_depth = np.zeros((256, 256, 3))
	# 		# im_normal = np.zeros((256, 256, 3))
	# 		# im_sym_depth = np.zeros((256, 256, 3))
	# 		# im_sym_normal = np.zeros((256, 256, 3))

	# 		# im_depth[60:60+137, 60:60+137, :] = cv2.imread(f_depth)
	# 		# im_normal[60:60+137, 60:60+137, :] = cv2.imread(f_normal)
	# 		# im_sym_depth[60:60+137, 60:60+137, :] = cv2.imread(f_sym_depth)
	# 		# im_sym_normal[60:60+137, 60:60+137, :] = cv2.imread(f_sym_normal)

	# 		im_depth = cv2.imread(f_depth)
	# 		im_normal = cv2.imread(f_normal)
	# 		im_sym_depth = cv2.imread(f_sym_depth)
	# 		im_sym_normal = cv2.imread(f_sym_normal)

	# 		im_sym = np.concatenate([im_normal, im_normal, im_depth, im_depth, im_sym_normal, im_sym_depth], 1)
	# 		im = np.concatenate([im_normal, im_normal, im_depth, im_depth], 1)
	# 		cv2.imwrite(join(dst_sym, "%s.png"%item_name), im_sym)
	# 		cv2.imwrite(join(dst, "%s.png"%item_name), im)
	
	# for c in range(4, 5):
	# 	cls_name = cls_set[c]
	# 	src = "D:/Data/%s/processed"%cls_name
	# 	src_sym = "D:/Data/%s/sym_old"%cls_name
	# 	dst_sym = "D:/Project/3DFront2Back/learn/datasets/%s_137_test_sym/test"%cls_name
	# 	dst = "D:/Project/3DFront2Back/learn/datasets/%s_137_test/test"%cls_name

	# 	if os.path.isdir(dst):
	# 		pass
	# 	else:
	# 		os.mkdir(dst)

	# 	if os.path.isdir(dst_sym):
	# 		pass
	# 	else:
	# 		os.mkdir(dst_sym)
		

	# 	for i in range(429, 430):
	# 	# for i in range(1, 1356):
	# 		f_normal = join(src, "%d_normal.png"%i)
	# 		f_depth = join(src, "%d_depth.png"%i)

	# 		f_sym_depth = join(src_sym, "%d_depth.png"%i)
	# 		f_sym_normal = join(src_sym,  "%d_normal.png"%i)

	# 		im_depth = np.zeros((256, 256, 3))
	# 		im_normal = np.zeros((256, 256, 3))
	# 		im_sym_depth = np.zeros((256, 256, 3))
	# 		im_sym_normal = np.zeros((256, 256, 3))

	# 		im_depth[60:60+137, 60:60+137, :] = cv2.imread(f_depth)
	# 		im_normal[60:60+137, 60:60+137, :] = cv2.imread(f_normal)
	# 		im_sym_depth[60:60+137, 60:60+137, :] = cv2.imread(f_sym_depth)
	# 		im_sym_normal[60:60+137, 60:60+137, :] = cv2.imread(f_sym_normal)

	# 		im_sym = np.concatenate([im_normal, im_normal, im_depth, im_depth, im_sym_normal, im_sym_depth], 1)
	# 		im = np.concatenate([im_normal, im_normal, im_depth, im_depth], 1)
	# 		cv2.imwrite(join(dst_sym, "%d.png"%i), im_sym)
	# 		cv2.imwrite(join(dst, "%d.png"%i), im)

	# crop
	for c in range(0, 1):
		# cls_name = cls_set[c]
		cls_name = "02828884"
		cls2_name = "04256520"
		print(cls_name)
		src = "learn/results/%s_137_real/test_15/images_%s_class"%(cls_name, cls2_name)
		item_list = os.listdir(src)

		for item in item_list:

			f = join(src, item)
			im = cv2.imread(f)[60:60+137, 60:60+137, :]
			cv2.imwrite(f, im)

		src = "learn/results/%s_137_sym_real/test_15/images_%s_class"%(cls_name, cls2_name)
		item_list = os.listdir(src)

		for item in item_list:

			f = join(src, item)
			im = cv2.imread(f)[60:60+137, 60:60+137, :]
			cv2.imwrite(f, im)

	# items = listdir(src)

	# for item in items:


	# 	im = np.zeros((256, 256, 3))


	# 	im[60:60+137, 60:60+137, :] = cv2.imread(join(src, item))

	# 	cv2.imwrite(join(src, item), im)



