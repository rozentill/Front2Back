import cv2
import numpy as np
from argparse import ArgumentParser

def parse_args():
    
    parser = ArgumentParser()
    
    parser.add_argument('--normal_path', type=str)
    parser.add_argument('--depth_path', type=str)
    parser.add_argument('--silhou_path', type=str)
    parser.add_argument('--output_path', type=str)
    parser.add_argument('--mode', type=int) # 0-combine, 1-rescale depth, 2-resize

    args = parser.parse_args()
    
    return args.normal_path, args.depth_path, args.silhou_path, args.output_path, args.mode

if __name__ == '__main__':

    f_normal, f_depth, f_silhou, f_output, mode = parse_args()
    
    img_normal = cv2.imread(f_normal)
    img_depth = cv2.imread(f_depth)
    img_silhou = cv2.imread(f_silhou)

    if mode == 0:

        pass

    elif mode == 1:
        
        img_depth = 255.*(img_depth/255. + 0.2)/1.2

    img_depth *= (img_silhou>100)
    img_normal *= (img_silhou>100)

    # elif mode == 2:
    img_combine_depth = np.concatenate([img_depth, img_depth], 1)
    img_combine_normal = np.concatenate([img_normal, img_normal], 1)
    img_combine_sym = np.concatenate([img_silhou, img_silhou], 1)
    img_combine = np.concatenate([img_combine_normal, img_combine_depth, img_combine_sym], 1)
    cv2.imwrite(f_output, img_combine)

    
