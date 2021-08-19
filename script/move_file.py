import os
from os.path import join
import csv
import shutil

def main():

	cls_set = [
		"02691156",
		"02828884",
		"02933112",
		"02958343",
		"03001627",
		"03211117",
		"03636649",
		"03691459",
		"04090263",
		"04256520",
		"04379243",
		"04401088",
		"04530566"
	]

	for c in range(7, 13):		


		cls_name = cls_set[c]
		ref_dir = "D:\\Data\\ShapeNet_test\\rot_gt\\%s"%cls_name
		src_dir = "E:\\front2back\\res\\%s_137\\pi"%cls_name
		res_dir = "E:\\front2back\\res_filtered\\%s"%cls_name
		if os.path.isdir(res_dir):
			pass
		else:
			os.mkdir(res_dir)
		items = os.listdir(ref_dir)

		for item in items:
			if "samples" in item:
				continue

			print(item)
			filename = join(src_dir, item[:-4]+"_dirichlet.ply")
			filename_out = join(res_dir, item[:-4]+".ply")
			if not os.path.exists(filename):
				continue

			shutil.move(filename, filename_out)


if __name__ == '__main__':

	main()