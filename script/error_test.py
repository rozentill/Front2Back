import cv2
import torch
import torch.nn as nn
import numpy.linalg as LA
import numpy as np
if __name__ == '__main__':

	f_gt = "test/gt_normal.png"
	f_pred = "test/pred_normal.png"

	f_silhou = "test/pred_silhou.png"
	
	f_gt_depth = "test/gt_depth.png"
	f_pred_depth = "test/pred_depth.png"

	img_gt = cv2.imread(f_gt)
	img_gt = img_gt[4:4+128, 4:4+128, :]
	img_silhou = cv2.imread(f_silhou)
	total_num = (img_silhou>200)[:, :, 0].sum()
	print(total_num)
	img_pred = cv2.imread(f_pred)
	img_pred = img_pred*(img_silhou>200)
	img_gt = img_gt*(img_silhou>200)
	img_gt = 2.*img_gt/255.-1
	img_pred = 2.*img_pred/255.-1

	im_depth = cv2.imread(f_gt_depth)[4:4+128, 4:4+128, :]

	im_pred_depth = cv2.imread(f_pred_depth)
	print(abs(im_pred_depth[50, 50]- im_depth[50, 50]))
	print(im_depth[50, 50])
	im_err_depth = abs(im_depth-im_pred_depth)

	im_pred_depth = im_pred_depth*(img_silhou>200)
	cv2.imwrite("test/masked_depth.png", im_pred_depth)
	# im_err_depth[:, :, 2] = (255 - im_err_depth[:, :, 2])
	# im_err_depth[:, :, 1] = im_err_depth[:, :, 1] *0
	cv2.imwrite("test/error_depth.png", im_err_depth)
	print(im_err_depth.mean())

	l1_loss = (abs(img_gt - img_pred)).sum()/total_num
	print(l1_loss)

	l2_loss = ((img_gt - img_pred)**2).sum()/total_num
	print(l2_loss)
	num = 0
	cos_loss = 0
	curr_error = 0
	error_map = np.zeros((img_pred.shape[0], img_pred.shape[1], 3))
	for x in range(0, img_pred.shape[0]):
		for y in range(0, img_pred.shape[1]):
			if img_silhou[x, y, 0] <= 200 :
				continue
			num+=1
			pred_vec = img_pred[x, y, :]			
			gt_vec = img_gt[x, y, :]
			pred_vec = pred_vec/(LA.norm(pred_vec))
			gt_vec = gt_vec/(LA.norm(gt_vec))
			curr_error = 1-np.dot(pred_vec, gt_vec)
			ten_err = 10*curr_error
			if ten_err > 1:
				ten_err = 1
			error_map[x, y, 0] = 255*ten_err
			error_map[x, y, 2] = 255*(1-ten_err)
			cos_loss += curr_error

			# print(1-np.dot(pred_vec, gt_vec))
	print(cos_loss/total_num)
	print(num)
	cv2.imwrite("test/error_normal.png", error_map)

