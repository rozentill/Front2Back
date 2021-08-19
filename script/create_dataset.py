import cv2
import numpy as np
from os import listdir
from os.path import isfile, join
from argparse import ArgumentParser
import shutil
import csv
import random
import os 
def parse_args():

    parser = ArgumentParser()
    parser.add_argument('--src', type=str, default=None)
    parser.add_argument('--src_real', type=str, default=None)
    parser.add_argument('--dst', type=str, default=None)
    parser.add_argument('--cls')
    args = parser.parse_args()
    return args


def main_f2b_test():
        # for c in range(1, 3):
    cls_name = "04256520"
    src = "D:/Data/ShapeNetRendering_test_res/02828884_137_%s/processed"%cls_name
    src_sym = "D:/Data/ShapeNetRendering_test_res/02828884_137_%s/sym"%cls_name
    dst_sym = "D:/Project/3DFront2Back/learn/datasets/02828884_137_%s_sym/test"%cls_name
    dst = "D:/Project/3DFront2Back/learn/datasets/02828884_137_%s/test"%cls_name
    ref_dir = "D:/Data/ShapeNet_test/input/%s"%cls_name

    if os.path.isdir(dst):
      pass
    else:
      os.mkdir(dst)

    if os.path.isdir(dst_sym):
      pass
    else:
      os.mkdir(dst_sym)

    items = os.listdir(ref_dir)

    for item in items:
        item_name = item[:-8]
        print(item_name)
        # item_name = "%d"%i
        f_normal = join(src, "%s_normal.png"%item_name)
        f_depth = join(src, "%s_depth.png"%item_name)

        f_sym_depth = join(src_sym, "%s_depth.png"%item_name)
        f_sym_normal = join(src_sym,  "%s_normal.png"%item_name)

        if os.path.exists(f_sym_depth):
            pass
        else:
            continue

        im_depth = np.zeros((256, 256, 3))
        im_normal = np.zeros((256, 256, 3))
        im_sym_depth = np.zeros((256, 256, 3))
        im_sym_normal = np.zeros((256, 256, 3))

        im_depth[60:60+137, 60:60+137, :] = cv2.imread(f_depth)
        im_normal[60:60+137, 60:60+137, :] = cv2.imread(f_normal)
        im_sym_depth[60:60+137, 60:60+137, :] = cv2.imread(f_sym_depth)
        im_sym_normal[60:60+137, 60:60+137, :] = cv2.imread(f_sym_normal)

        # im_depth = cv2.imread(f_depth)
        # im_normal = cv2.imread(f_normal)
        # im_sym_depth = cv2.imread(f_sym_depth)
        # im_sym_normal = cv2.imread(f_sym_normal)

        im_sym = np.concatenate([im_normal, im_normal, im_depth, im_depth, im_sym_normal, im_sym_depth], 1)
        im = np.concatenate([im_normal, im_normal, im_depth, im_depth], 1)
        cv2.imwrite(join(dst_sym, "%s.png"%item_name), im_sym)
        cv2.imwrite(join(dst, "%s.png"%item_name), im)

def main_f2b_train():

    args = parse_args()
    
    cls_name = args.cls
    
    src = args.src
    
    dst = "D:/Project/3DFront2Back/learn/datasets/%s_137"%cls_name
    dst_sym = "D:/Project/3DFront2Back/learn/datasets/%s_137_sym"%cls_name

    if not os.path.isdir(join(dst, "train")):

        os.mkdir(join(dst, "train"))
        os.mkdir(join(dst_sym, "train"))
    
    if not os.path.isdir(join(dst, "val")):

        os.mkdir(join(dst, "val"))
        os.mkdir(join(dst_sym, "val"))

    if not os.path.isdir(join(dst, "test")):
        
        os.mkdir(join(dst, "test"))
        os.mkdir(join(dst_sym, "test"))

    split_file = "D:/Data/shapenet_split.csv"

    dict_split = {}
    with open(split_file, newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
        for row in spamreader:
            parts = row[0].split(',')
            dict_split[parts[3]] = parts[4]

    if cls_name=="00000000": # all classes 

        cls_set = [
            "02691156",
            "02828884",
            "02933112",
            "02958343",
            "03001627",
            "03211117",
            "03636649",
            "03691459",
            "04090263",
            "04256520",
            "04379243",
            "04401088",
            "04530566"
        ]
        num = 0
        for cls in cls_set:
            
            item_list = listdir(join(src, cls))

            for item in item_list:
                
                if item not in dict_split.keys() :
                    continue
                else:
                    _set = dict_split[item]

                if _set != "train":
                    continue

                l = list(range(0,24))
                random.shuffle(l)
                view = l[0]
                #read maps
                
                f_normal = join(src, cls, item, "%s_%s_view%03d_normal.png"%(cls,item, view))
                f_depth = join(src, cls, item, "%s_%s_view%03d_depth.png"%(cls,item, view))
                
                f_normal_back = join(src, cls, item, "%s_%s_view%03d_normal_back.png"%(cls,item, view))
                f_depth_back = join(src, cls, item, "%s_%s_view%03d_depth_back.png"%(cls,item, view))

                f_normal_sym = join(src, cls, item, "%s_%s_view%03d_normal_sym.png"%(cls,item, view))
                f_depth_sym = join(src, cls, item, "%s_%s_view%03d_depth_sym.png"%(cls,item, view))
                # print(f_normal)
                if not os.path.exists(f_normal) or \
                not os.path.exists(f_depth) or \
                not os.path.exists(f_normal_back) or \
                not os.path.exists(f_depth_back) or \
                not os.path.exists(f_normal_sym) or \
                not os.path.exists(f_depth_sym):
                      continue

                im_depth = np.zeros((256, 256, 3))
                im_normal = np.zeros((256, 256, 3))
                im_depth_back = np.zeros((256, 256, 3))
                im_normal_back = np.zeros((256, 256, 3))
                im_sym_depth = np.zeros((256, 256, 3))
                im_sym_normal = np.zeros((256, 256, 3))

                im_depth[60:60+137, 60:60+137, :] = cv2.imread(f_depth)
                im_normal[60:60+137, 60:60+137, :] = cv2.imread(f_normal)
                im_depth_back[60:60+137, 60:60+137, :] = cv2.imread(f_depth_back)
                im_normal_back[60:60+137, 60:60+137, :] = cv2.imread(f_normal_back)
                im_sym_depth[60:60+137, 60:60+137, :] = cv2.imread(f_depth_sym)
                im_sym_normal[60:60+137, 60:60+137, :] = cv2.imread(f_normal_sym)

                im_sym = np.concatenate([im_normal_back, im_normal, im_depth_back, im_depth, im_sym_normal, im_sym_depth], 1)
                im = np.concatenate([im_normal_back, im_normal, im_depth_back, im_depth], 1)

                cv2.imwrite(join(dst_sym, _set, "%s.png"%num), im_sym)
                cv2.imwrite(join(dst, _set, "%s.png"%num), im)
                # print(join(dst_sym, _set, "%s.png"%num))
                num+=1

def main_f2b_test_real():
    
    cls_name = "02828884"

    src_front = "D:\\Data\\ShapeNetRendering_test_res\\%s\\processed"%cls_name
    src_sym = "D:\\Data\\ShapeNetRendering_test_res\\%s\\sym"%cls_name
    src_back = "E:\\front2back\\data\\Front2Back\\%s"%cls_name
    dst = "D:/Project/3DFront2Back/learn/datasets/%s_137"%cls_name
    dst_sym = "D:/Project/3DFront2Back/learn/datasets/%s_137_sym"%cls_name

    if not os.path.isdir(join(dst, "train")):

        os.mkdir(join(dst, "train"))
        os.mkdir(join(dst_sym, "train"))
    
    if not os.path.isdir(join(dst, "val")):

        os.mkdir(join(dst, "val"))
        os.mkdir(join(dst_sym, "val"))

    if not os.path.isdir(join(dst, "test")):
        
        os.mkdir(join(dst, "test"))
        os.mkdir(join(dst_sym, "test"))

    split_file = "D:/Data/shapenet_split.csv"

    dict_split = {}
    
    with open(split_file, newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
        for row in spamreader:
            parts = row[0].split(',')
            if parts[1] == cls_name:
                dict_split[parts[3]] = parts[4]

    items = os.listdir(src_back)

    num = 0
    for item in items:

        if item not in dict_split.keys() :
            continue
        else:
            _set = dict_split[item]

        if _set == "test":
            pass
        else:
            continue

        #front
        f_normal = join(src_front, "%s_normal.png"%(item))
        f_depth = join(src_front, "%s_depth.png"%(item))

        #back
        f_normal_back = join(src_back, item, "%s_%s_view%03d_normal_back.png"%(cls_name, item, 0))
        f_depth_back = join(src_back, item, "%s_%s_view%03d_depth_back.png"%(cls_name, item, 0))

        #sym
        f_normal_sym = join(src_sym, "%s_normal.png"%(item))
        f_depth_sym = join(src_sym, "%s_depth.png"%(item))

        if not os.path.exists(f_normal) or \
        not os.path.exists(f_depth) or \
        not os.path.exists(f_normal_back) or \
        not os.path.exists(f_depth_back) or \
        not os.path.exists(f_normal_sym) or \
        not os.path.exists(f_depth_sym):
            continue

        im_depth = np.zeros((256, 256, 3))
        im_normal = np.zeros((256, 256, 3))
        im_depth_back = np.zeros((256, 256, 3))
        im_normal_back = np.zeros((256, 256, 3))
        im_sym_depth = np.zeros((256, 256, 3))
        im_sym_normal = np.zeros((256, 256, 3))

        im_depth[60:60+137, 60:60+137, :] = cv2.imread(f_depth)
        im_normal[60:60+137, 60:60+137, :] = cv2.imread(f_normal)
        im_depth_back[60:60+137, 60:60+137, :] = cv2.imread(f_depth_back)
        im_normal_back[60:60+137, 60:60+137, :] = cv2.imread(f_normal_back)
        im_sym_depth[60:60+137, 60:60+137, :] = cv2.imread(f_depth_sym)
        im_sym_normal[60:60+137, 60:60+137, :] = cv2.imread(f_normal_sym)

        im_sym = np.concatenate([im_normal_back, im_normal, im_depth_back, im_depth, im_sym_normal, im_sym_depth], 1)
        im = np.concatenate([im_normal_back, im_normal, im_depth_back, im_depth], 1)

        cv2.imwrite(join(dst, _set, "%s.png"%num), im)
        with open(join(src_sym, "%s.txt"%(item)), 'r') as f_sym_plane:
            line = f_sym_plane.readline()
            if len(line.split(' ')) == 4:
                cv2.imwrite(join(dst_sym, _set, "%s.png"%num), im_sym)

        num += 1

def main_r2f_train_rgb():
    args = parse_args()
    cls_name = args.cls
    dst = "D:/Data/Front2Back/%s_256"%cls_name
    # dst = "E:\\front2back\\data\\Front2Back\\%s_256"%cls_name
    src = "D:\\Data\\ShapeNetRendering_high\\%s"%cls_name

    item_list = os.listdir(src)

    for item in item_list:
        if not os.path.isdir(join(dst, item)):
            os.mkdir(join(dst, item))
        for view in range(0, 24):
        
            f_src = join(src, item, "%d.png"%view)
            f_dst = join(dst, item,"%s_%s_view%03d_rgb.png"%(cls_name, item, view))
            shutil.copy(f_src, f_dst)

def main_r2f_test():

    args = parse_args()
    cls_name = args.cls
    split_file = "D:/Data/shapenet_split.csv"
    num = 0
    dict_split = {}
    with open(split_file, newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
        for row in spamreader:
            parts = row[0].split(',')
            if parts[1] == cls_name:
                dict_split[parts[3]] = parts[4]

    src = "D:\\Data\\shapenetrendering_high"
    dst = "D:\\Data\\ShapeNet_test"
    items_list = listdir(join(src, cls_name))
    num_test = 0
    for item in items_list:
        if item in dict_split.keys() and dict_split[item] == 'test':
            
            f_src = join(src, cls_name, item, "0.png")
            f_dst = join(dst, "input_256", cls_name, "%s_rgb.png"%(item))
            shutil.copy(f_src, f_dst)

if __name__ == '__main__':
    
    main_f2b_test()  
    

