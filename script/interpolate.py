import cv2
from os.path import join

src_sym = "D:\\Data\\Test\\real2inter_ortho_sym"
src_back = "D:\\Project\\3DFront2Back\\learn\\results\\ortho_chair_sym_small\\test_latest\\images"

if __name__ == '__main__':
	
	for i in range(1, 101):
		
		sym_depth = cv2.imread(join(src_sym, "%d_depth.png"%i))
		sym_normal = cv2.imread(join(src_sym, "%d_normal.png"%i))
		back_depth = cv2.imread(join(src_back, "%d_fake_B_depth.png"%i))
		back_normal = cv2.imread(join(src_back, "%d_fake_B.png"%i))

		for y in range(1, sym_depth.shape[0]-1):
			for x in range(1, sym_depth.shape[1]-1):

				center_value = int(sym_depth[y, x, 0])
				left_value = int(sym_depth[y, x-1, 0])
				right_value = int(sym_depth[y, x+1, 0])
				top_value = int(sym_depth[y-1, x, 0])
				bottom_value = int(sym_depth[y+1, x, 0])

				if abs(right_value-left_value) < 5 and right_value != 0 and left_value != 0 and right_value != 255 and left_value != 255:
					
					if abs(center_value-right_value) >= 5:
						center_value = (right_value+left_value)/2
						sym_depth[y, x, 0] =  sym_depth[y, x, 1]  = sym_depth[y, x, 2] = center_value
						sym_normal[y, x, :] = (sym_normal[y, x-1, :] + sym_normal[y, x+1, :])/2
				
				if abs(bottom_value-top_value) < 5 and bottom_value != 0 and top_value != 0 and bottom_value != 255 and top_value != 255:
					
					if abs(center_value-bottom_value) >= 5:
						center_value = (bottom_value/2+top_value/2)
						sym_depth[y, x, 0] =  sym_depth[y, x, 1]  = sym_depth[y, x, 2] = center_value
						sym_normal[y, x, :] = (sym_normal[y-1, x, :]/2 + sym_normal[y+1, x, :]/2)


		for y in range(0, back_depth.shape[0]):
			for x in range(0, back_depth.shape[1]):

				if sym_depth[y,x,0] < back_depth[y,x,0]:
					back_depth[y,x,:] = sym_depth[y, x, :]
					back_normal[y,x,:] = sym_normal[y,x,:]


		cv2.imwrite(join(src_sym, "%d_depth_interpolate.png"%i), sym_depth)
		cv2.imwrite(join(src_sym, "%d_normal_interpolate.png"%i), sym_normal)
		cv2.imwrite(join(src_back, "%d_fake_B_depth_interpolate.png"%i), back_depth)
		cv2.imwrite(join(src_back, "%d_fake_B_interpolate.png"%i), back_normal)





