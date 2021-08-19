import os
from os.path import join
import cv2

def main():
	src_dir = "D:\\Data\\eval_data_front2back\\test_genre\\table"
	dst_dir = "D:\\Data\\eval_data_front2back\\test_genre\\table_resize"

	if not os.path.isdir(dst_dir):
		os.mkdir(dst_dir)

	src_items = os.listdir(src_dir)

	for item in src_items:

		im_item = cv2.imread(join(src_dir, item))
		im_resize = cv2.resize(im_item, (137, 137), interpolation=cv2.INTER_NEAREST)

		cv2.imwrite(join(dst_dir, item), im_resize)

if __name__ == '__main__':
	main()