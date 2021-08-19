import cv2
from os.path import join

src = "D:\\Project\\3DFront2Back\\learn\\results\\ortho_chair_sym_small\\test_latest\\images"

if __name__ == '__main__':
	#
	for i in range(1, 101):

		f_depth = join(src, "%d_real_A_depth.png"%i)

		im_depth = cv2.imread(f_depth)
		im_silhouette = (im_depth > 0) * 255

		cv2.imwrite(join(src, "%d_silhou.png")%i, im_silhouette)