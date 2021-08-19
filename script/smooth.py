import cv2
from os import listdir
from os.path import isfile, join
from argparse import ArgumentParser
from bilateral import *
from cv2.ximgproc import guidedFilter

if __name__ == '__main__':

    parser = ArgumentParser()
    
    parser.add_argument('--input_dir', default=None, required=True)
    parser.add_argument('--output_dir', default=None, required=True)

    args = parser.parse_args()

    input_dir = args.input_dir
    output_dir = args.output_dir

    files = [f for f in listdir(input_dir) if isfile(join(input_dir, f))]

    fake_depth = None
    fake_normal = None
    
    smooth = None
    
    # for f in files:

    for i in range(1,1318):
        f_depth = "%d_fake_B_depth.png" %i
        f_normal = "%d_fake_B.png" %i

        fake_depth = cv2.imread(input_dir + f_depth)
        fake_normal = cv2.imread(input_dir + f_normal)

        smooth_normal = cv2.bilateralFilter(fake_normal, 13, 155, 155)        
        # smooth_depth = cv2.bilateralFilter(fake_depth, 13, 155, 155)
        smooth_depth = guidedFilter(
            guide=fake_normal, src=fake_depth, radius=16, eps=200, dDepth=-1)
        cv2.imwrite(output_dir + f_depth, smooth_depth)
        cv2.imwrite(output_dir + f_normal, smooth_normal)
    # if "fake" in f:
    #     print(f)
    #     if "depth" in f:
    #         fake_depth = cv2.imread(input_dir + f)
    #         smooth = cv2.bilateralFilter(fake_depth, 13, 155, 155)
    #         # smooth = bilateral_filter_own(fake_depth, 13, 155, 155)

    #     else:
    #         fake_normal = cv2.imread(input_dir + f)
    #         smooth = cv2.bilateralFilter(fake_normal, 13, 155, 155)        
    #         # smooth = bilateral_filter_own(fake_normal, 13, 155, 155)
    #     print(output_dir+f)
    #     cv2.imwrite(output_dir + f, smooth)