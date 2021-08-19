import cv2
from os import listdir
from os.path import isfile, join
from argparse import ArgumentParser

def parse_args():

	parser = ArgumentParser()
	parser.add_argument('image_dir', default=None)
	args = parser.parse_args()
	return args.image_dir

if __name__ == '__main__':

	path = parse_args()

	for num in range(0, 200):
		
		item = "%04d.png"%num
		
		img = cv2.imread(join(path, item))
		
		# print(img.shape)#256, 1024, 3

		depth_part = img[:, 256*2:, :]
		
		new_depth_part = (298 - depth_part) * (depth_part > 0)
		
		img[:, 256*2:, :] = new_depth_part

		cv2.imwrite(join(path, item), img)
