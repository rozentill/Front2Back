import cv2
import os
import shutil

from os.path import join

if __name__ == '__main__':

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
	for c in range(9, 10):
		
		cls_name = cls_set[c]
		in_dir = "D:/Data/ShapeNetRendering_test_res/02828884_137_%s/raw"%cls_name
		out_dir = "D:/Data/ShapeNetRendering_test_res/02828884_137_%s/processed"%cls_name
		ref_dir = "D:/Data/ShapeNet_test/input/%s"%cls_name

		if os.path.isdir(out_dir):
			pass
		else:
			os.mkdir(out_dir)

		items = os.listdir(ref_dir)

		for item in items:
			# print(item)

			item_name = item[:-7]
			# item_name = "%d"%i
			f_normal = item_name+'pred_normal.png'
			f_depth = item_name+'pred_depth.png'
			f_silhou = item_name+'pred_silhou.png'
			# print(join(in_dir, f_normal))
			if os.path.exists(join(in_dir, f_normal)):
				pass
			else:
				continue
			im_normal = cv2.imread(join(in_dir, f_normal))
			im_depth = cv2.imread(join(in_dir, f_depth))
			im_silhou = cv2.imread(join(in_dir, f_silhou))

			im_normal = im_normal * (im_silhou>230)
			im_depth = im_depth * (im_silhou>230)
			#for 137 resolution
			im_normal = im_normal
			im_depth = im_depth
			#for 256 resolution
			# im_normal = im_normal[4:4+256, 4:4+256, :]
			# im_depth = im_depth[4:4+256, 4:4+256, :]
			
			cv2.imwrite(join(out_dir, "%snormal.png"%item_name), im_normal)
			cv2.imwrite(join(out_dir, "%sdepth.png"%item_name), im_depth)
