import cv2
import os
import shutil

from os.path import join

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
	for i in range(len(cls_set)):
		cls_name = cls_set[i]
		ren_dir = "D:/Data/shapenetrendering_compressed/ShapeNetRendering/%s"%cls_name
		ref_dir = "D:/Data/ShapeNet_test/input/%s"%cls_name
		dst_dir = "D:/Data/ShapeNet_test/param/%s"%cls_name
		if not os.path.exists(dst_dir):
			os.mkdir(dst_dir)
		items = os.listdir(ref_dir)

		for item in items:
			item_name = item[:-8]
			src_meta = join(ren_dir, item_name, "rendering/rendering_metadata.txt")
			tgt_meta = join(dst_dir, "%s.txt"%item_name)

			shutil.copy(src_meta, tgt_meta)

if __name__ == '__main__':
	main()