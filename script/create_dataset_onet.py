import os
import shutil

from os.path import join

def main():
	src = "D:\\Data\\ShapeNetRendering_high\\03001627"
	dst = "D:\\Project\\occupancy_networks\\data\\ShapeNet\\03001627"
	folder = "img_choy2016_256"
	item_list = os.listdir(src)

	for item in item_list:
		
		os.mkdir(join(dst, item, folder))
		for view in range(0, 24):
			src_rgb = join(src, item, "%d.png"%view)
			dst_rgb = join(dst, item, folder, "%03d.jpg"%view)
			shutil.copy(src_rgb, dst_rgb)


if __name__ == '__main__':
	main()