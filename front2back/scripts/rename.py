import cv2
from os import listdir
from os.path import isfile, join
from argparse import ArgumentParser

def parse_args():
    parser = ArgumentParser()
    parser.add_argument('img_folder')
    parser.add_argument('postfix')

    args = parser.parse_args()
    return args.img_folder, args.postfix

if __name__ == '__main__':
    
    path, pre = parse_args()

    files = [f for f in listdir(path) if isfile(join(path, f))]
    for f in files:

        if pre in f:    
            
            img_path = path+f
            # print(img_path)    
            rgb_data = cv2.imread(img_path)

            cv2.imwrite(path+f[:-10]+'.png', rgb_data)
