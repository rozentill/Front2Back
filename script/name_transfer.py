import cv2
import os
import shutil

from os.path import join

if __name__ == '__main__':
	
	# in_dir = "D:/Data/ShapeNetRendering_test_res/03001627/raw"
	out_dir = "D:/Data/ShapeNetRendering_test_res/03001627/processed"
	ref_dir = "D:/Data/shapenetrendering_compressed/ShapeNetRendering/ShapeNetRendering/03001627"
	f_record = "%s/name.txt"%out_dir

	# items = os.listdir(in_dir)

	# num = 0
	# with open(f_record, 'w') as f:
	# 	for item in items:

	# 		if item.split('.')[0].split('_')[-1] == 'rgb':
				
	# 			num += 1

	# 			f_normal = item[:-7]+'pred_normal.png'		
	# 			f_depth = item[:-7]+'pred_depth.png'
	# 			f_silhou = item[:-7]+'pred_silhou.png'
	# 			print(join(in_dir, f_silhou))
	# 			im_normal = cv2.imread(join(in_dir, f_normal))
	# 			im_depth = cv2.imread(join(in_dir, f_depth))
	# 			im_silhou = cv2.imread(join(in_dir, f_silhou))

	# 			im_normal = im_normal * (im_silhou>230)
	# 			im_depth = im_depth * (im_silhou>230)

	# 			cv2.imwrite(join(out_dir, "%d_normal.png"%num), im_normal)
	# 			cv2.imwrite(join(out_dir, "%d_depth.png"%num), im_depth)
	# 			f.write("%d %s\n"%(num, item[:-8]))

	f_param = "%s/param.txt"%out_dir

	with open(f_param, 'w') as fout:

		with open(f_record, 'r') as f:

			lines = f.readlines()

			for line in lines:

				# print(line)
				
				# item = line.split(' ')[1].split('_')[1]
				# view = int(line.split(' ')[1].split('_')[2][-3:])
				item = line.split(' ')[1].split('\n')[0]
				view = 0
				f_metadata = join(ref_dir, item, "rendering/rendering_metadata.txt")

				fout.write(item)

				with open(f_metadata, 'r') as f_meta:

					lines_meta = f_meta.readlines()
					param_split = lines_meta[view].split(' ')
					# print("first")
					# print(param_split[0])
					# print("second")
					# print(param_split[1])
					# print("second")
					# print(lines_meta[view][1])
					fout.write(' %s %s %s\n'%(param_split[0], param_split[1], param_split[3]))
					# print(lines_meta[view])
					# for line_meta in lines_meta:
					# 	print(line_meta)
					# 	line_meta 






			# no = line.split(' ')[0]
			# prefix = line.split(' ')[1][:-1]
			# # print(prefix)
			# f_rgb_src = join(in_dir, "%srgb.png"%prefix)
			# f_rgb_dst = join(out_dir, "%s_rgb.png"%no)
			# shutil.copy(f_rgb_src, f_rgb_dst)

			
	# src = "D:\\Project\\occupancy_networks\\out\\img\\onet_legacy\\pretrained\\meshes\\03001627"
	# f_name = "%s/name.txt"%out_dir
	# dst = "D:\\Project\\3DFront2Back\\3rdparty\\metro\\compare\\their"
	# print(f_name)
	# with open(f_name, 'r') as fout:
	# 	lines = fout.readlines()
	# 	# print(lines)
	# 	for i in range(0, 1355):
	# 		item = lines[i].split(' ')[1][:-1]
	# 		num = lines[i].split(' ')[0]
	# 		shutil.copy(join(src, "%s.off")%item, join(dst, "%s.off")%num)
