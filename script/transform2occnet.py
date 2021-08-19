import cv2
from os.path import join

if __name__ == '__main__':
	src = "D:\\Data\\Test\\real2inter_ortho"

	for i in range(1, 301):

		src_file = join(src, "%d_rgb.png"%i)
		im= cv2.imread(src_file)

		for y in range(im.shape[0]):
			for x in range(im.shape[1]):
				if im[y, x, 0] == 0 and im[y, x, 1] == 0 and im[y, x, 2] == 0:
					im[y, x, 0] = 255
					im[y, x, 1] = 255
					im[y, x, 2] = 255


		im = cv2.resize(im, (224, 224))
		dst_file = join(src, "input", "%d.png"%i)
		cv2.imwrite(dst_file, im)
