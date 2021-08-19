import cv2
from os import listdir
from os.path import isfile, join
from argparse import ArgumentParser

def parse_args():
    parser = ArgumentParser()
    parser.add_argument('img_file')
    parser.add_argument('--obj_path', default=None, required=False)

    args = parser.parse_args()
    return args.img_file, args.obj_path

if __name__ == '__main__':

    # parser = ArgumentParser()
    # parser.add_argument('--img', default=None, required=True)
    path, _ = parse_args()

    if ".png" in path:

        rgb_data = cv2.imread(path)

        rgba = cv2.cvtColor(rgb_data, cv2.COLOR_RGB2RGBA)

        rgba[:, :, 3] = 255*(1 - (rgb_data[:, :, 0] == 0 ) * (rgb_data[:, :, 1]==0) *(rgb_data[:, :, 2] ==0 ))

        cv2.imwrite(path[:-4]+'_a.png', rgba)
    else:
        files = [f for f in listdir(path) if isfile(join(path, f))]
        for f in files:
            if ".png" in f and '_a' not in f:    
                
                img_path = path+f
                print(img_path)
                rgb_data = cv2.imread(img_path)

                rgba = cv2.cvtColor(rgb_data, cv2.COLOR_RGB2RGBA)

                rgba[:, :, 3] = 255*(1 - (rgb_data[:, :, 0] == 0 ) * (rgb_data[:, :, 1]==0) *(rgb_data[:, :, 2] ==0 ))

                cv2.imwrite(img_path[:-4]+'.png', rgba)

    # args = parser.parse_args()
    # path = args.img
    # print(path)

    
