import cv2
from os.path import join
import numpy as np

def old():

	src = "D:\\Data\\onlinepics\\plane"

	for i in range(1, 6):
		if i == 2 or i == 0:
			continue
		f = join(src, "%d.png"%i)
		print(f)
		img = cv2.imread(f)

		# img = 255 - img
		for x in range(img.shape[0]):
			for y in range(img.shape[1]):
				# img
				if img[x, y, 0] == 0 and img[x, y, 1] == 0 and img[x, y, 2] == 0:

					img[x, y, :] = [255, 255, 255]

		# img_l = np.zeros((151, 151, 3))
		# img_l[7:144, 7:144, :] = img
		
		# img = cv2.resize(img_l, (137, 137))
		cv2.imwrite(f, img)

def white():


	# f = join(src, "%d.png"%i)
	f = "D:\\Project\\front2back_paper\\video\\depth.png"
	print(f)
	img = cv2.imread(f)

	# img = 255 - img
	for x in range(img.shape[0]):
		for y in range(img.shape[1]):
			# img
			if img[x, y, 0] == 0 and img[x, y, 1] == 0 and img[x, y, 2] == 0:

				img[x, y, :] = [255, 255, 255]

	# img_l = np.zeros((151, 151, 3))
	# img_l[7:144, 7:144, :] = img
	
	# img = cv2.resize(img_l, (137, 137))
	cv2.imwrite(f, img)
if __name__ == '__main__':
	white()