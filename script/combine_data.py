import cv2
import os
from os.path import join
from os import listdir
import random
import csv
import numpy as np
from argparse import ArgumentParser

def parse_args():

    parser = ArgumentParser()
    parser.add_argument('--cls', type=str, default=None)
    # parser.add_argument('--src_real', type=str, default=None)
    # parser.add_argument('--dst', type=str, default=None)
    args = parser.parse_args()
    return args

def combine_train_real(cls_name):
    
    # get front from real data
    # cls_name = "02828884"

    src_front = "D:\\Data\\ShapeNetRendering_test_res\\%s_train\\processed"%cls_name
    src_sym = "D:\\Data\\ShapeNetRendering_test_res\\%s_train\\sym"%cls_name
    src_back = "E:\\front2back\\data\\Front2Back\\%s"%cls_name
    dst = "D:/Project/3DFront2Back/learn/datasets/%s_137_real"%cls_name
    dst_sym = "D:/Project/3DFront2Back/learn/datasets/%s_137_sym_real"%cls_name

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

    num = 7265
    for item in items:

        if item not in dict_split.keys() :
            continue
        else:
            _set = dict_split[item]

        if _set == "train":
            num_view = 5
        else:
            continue

        for i in range(0, num_view):
            view = i

            #front
            f_normal = join(src_front, "%s_view%03d_normal.png"%(item, view))
            f_depth = join(src_front, "%s_view%03d_depth.png"%(item, view))

            #back
            f_normal_back = join(src_back, item, "%s_%s_view%03d_normal_back.png"%(cls_name, item, view))
            f_depth_back = join(src_back, item, "%s_%s_view%03d_depth_back.png"%(cls_name, item, view))

            #sym
            f_normal_sym = join(src_sym, "%s_view%03d_normal.png"%(item, view))
            f_depth_sym = join(src_sym, "%s_view%03d_depth.png"%(item, view))

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
            with open(join(src_sym, "%s_view%03d.txt"%(item, view)), 'r') as f_sym_plane:
                line = f_sym_plane.readline()
                if len(line.split(' ')) == 4:
                    cv2.imwrite(join(dst_sym, _set, "%s.png"%num), im_sym)

            num += 1


def combine_train_137(cls_name):

    # src = "E:\\front2back\\data\\Front2Back"
    src = "D:\\Data\\Front2Back"
    dst = "D:/Project/3DFront2Back/learn/datasets/%_137"%cls_name
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

    item_list = listdir(join(src, cls_name+"_137")) 

    num = 0
    num_view = 0
    _set = ""
    for item in item_list:
        
        if item not in dict_split.keys() :
            continue
        else:
            _set = dict_split[item]

        if _set == "train":
            num_view = 5
        else:
            num_view = 1

        l = list(range(0, 24))
        random.shuffle(l)
        
        for i in range(0, num_view):
            view = l[i]

            f_normal = join(src, cls_name+"_137", item, "%s_%s_view%03d_normal.png"%(cls_name,item, view))
            f_depth = join(src, cls_name+"_137", item, "%s_%s_view%03d_depth.png"%(cls_name,item, view))
            f_normal_back = join(src, cls_name+"_137", item, "%s_%s_view%03d_normal_back.png"%(cls_name,item, view))
            f_depth_back = join(src, cls_name+"_137", item, "%s_%s_view%03d_depth_back.png"%(cls_name,item, view))

            f_normal_sym = join(src, cls_name+"_137", item, "%s_%s_view%03d_normal_sym.png"%(cls_name,item, view))
            f_depth_sym = join(src, cls_name+"_137", item, "%s_%s_view%03d_depth_sym.png"%(cls_name,item, view))

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
            num+=1


def combine_train_256(cls_name):

    # src = "E:\\front2back\\data\\Front2Back"
    src = "D:\\Data\\Front2Back"
    dst = "D:/Project/3DFront2Back/learn/datasets/%s_256"%cls_name
    dst_sym = "D:/Project/3DFront2Back/learn/datasets/%s_256_sym"%cls_name

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

    item_list = listdir(join(src, cls_name+"_256")) 

    num = 0
    num_view = 0
    _set = ""
    for item in item_list:
        
        if item not in dict_split.keys() :
            continue
        else:
            _set = dict_split[item]

        if _set == "train":
            num_view = 5
        else:
            num_view = 1

        l = list(range(0, 24))
        random.shuffle(l)
        
        for i in range(0, num_view):
            view = l[i]

            f_normal = join(src, cls_name+"_256", item, "%s_%s_view%03d_normal.png"%(cls_name,item, view))
            f_depth = join(src, cls_name+"_256", item, "%s_%s_view%03d_depth.png"%(cls_name,item, view))
            f_normal_back = join(src, cls_name+"_256", item, "%s_%s_view%03d_normal_back.png"%(cls_name,item, view))
            f_depth_back = join(src, cls_name+"_256", item, "%s_%s_view%03d_depth_back.png"%(cls_name,item, view))

            f_normal_sym = join(src, cls_name+"_256", item, "%s_%s_view%03d_normal_sym.png"%(cls_name,item, view))
            f_depth_sym = join(src, cls_name+"_256", item, "%s_%s_view%03d_depth_sym.png"%(cls_name,item, view))

            if not os.path.exists(f_normal) or \
            not os.path.exists(f_depth) or \
            not os.path.exists(f_normal_back) or \
            not os.path.exists(f_depth_back) or \
            not os.path.exists(f_normal_sym) or \
            not os.path.exists(f_depth_sym):
              continue

            im_depth = cv2.imread(f_depth)
            im_normal= cv2.imread(f_normal)
            im_depth_back = cv2.imread(f_depth_back)
            im_normal_back = cv2.imread(f_normal_back)
            im_sym_depth= cv2.imread(f_depth_sym)
            im_sym_normal = cv2.imread(f_normal_sym)

            im_sym = np.concatenate([im_normal_back, im_normal, im_depth_back, im_depth, im_sym_normal, im_sym_depth], 1)
            im = np.concatenate([im_normal_back, im_normal, im_depth_back, im_depth], 1)

            cv2.imwrite(join(dst_sym, _set, "%s.png"%num), im_sym)
            cv2.imwrite(join(dst, _set, "%s.png"%num), im)
            num+=1


def main():

    args = parse_args()
    cls_name = args.cls
    combine_train_real(cls_name)
    

if __name__ == '__main__':

    main()
