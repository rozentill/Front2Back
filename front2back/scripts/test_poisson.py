import sys
import cv2
import numpy as np
import torch
import torch.nn.functional as F
import math

if __name__ == "__main__":

    file_normal = sys.argv[1]
    file_depth = sys.argv[2]
    file_output = sys.argv[3]

    normal_map = cv2.imread(file_normal)
    depth_map = cv2.imread(file_depth)

    normal_array = np.array(normal_map, dtype=float)
    depth_array = np.array(depth_map, dtype=float)[:, :, :1]

    cv2.imwrite(file_output[:-4]+"_normal_0.png", normal_array[:, :, 0:1])
    cv2.imwrite(file_output[:-4]+"_normal_1.png", normal_array[:, :, 1:2])
    cv2.imwrite(file_output[:-4]+"_normal_2.png", normal_array[:, :, 2:3])

    print(np.sum(normal_array==255))

    normal_array /= 256.0
    normal_array -= 0.5
    normal_array /= 0.5

    depth_array /= 256.0
    depth_array -= 0.5
    depth_array /= 0.5

    depth_array *= 0.6
    depth_array -= 0.1

    # test_array = np.array([
    #     [0, 2, 1],
    #     [4, 3, 5],
    #     [9, 8, 7]
    #     ])
    # test_array = np.expand_dims(test_array, axis=2)

    conv_x = np.zeros((3, 3))
    conv_y = np.zeros((3, 3))

    ##########
    # conv_x
    # 0 0 0
    # -1/2 0 1/2
    # 0 0 0
    ###########
    # conv_y
    # 0 1 0
    # 0 0 0
    # 0 -1/2 0
    ############
    conv_x[1, 0] = -1/2
    conv_x[1, 2] = 1/2
    conv_y[2, 1] = -1/2
    conv_y[0, 1] = 1/2

    tangent_x_z = cv2.filter2D(depth_array, -1, conv_x)
    tangent_y_z = cv2.filter2D(depth_array, -1, conv_y)

    # test_x = cv2.filter2D(test_array, -1, conv_x)
    # test_y = cv2.filter2D(test_array, -1, conv_y)
    # print(test_x)
    # print(test_y)

    tangent_x_z = np.expand_dims(tangent_x_z, axis=2)
    tangent_y_z = np.expand_dims(tangent_y_z, axis=2)



    tangent_x_x = np.ones((depth_array.shape[0], depth_array.shape[1], 1), dtype=float)/256
    tangent_y_y = np.ones((depth_array.shape[0], depth_array.shape[1], 1), dtype=float)/256

    for i in range(0, normal_array.shape[0]):
        for j in range(0, normal_array.shape[1]):
            if normal_array[i, j, 0] == 1 and normal_array[i, j, 1] == 1 and normal_array[i, j, 2] == 1:
                normal_array[i, j, 0] = 0
                normal_array[i, j, 1] = 0
                normal_array[i, j, 2] = 0

    normal_array_sum = np.sqrt(normal_array[:, :, 0:1]**2 + normal_array[:, :, 1:2]**2 + normal_array[:, :, 2:3]**2)

    normal_sum_map = (normal_array_sum*256/1.005).astype(np.uint8)
    cv2.imwrite(file_output[:-4]+"_normalsum.png", normal_sum_map)

    print(((tangent_x_x) * normal_array[:, :, 0:1]).shape)
    print((tangent_x_z * normal_array[:, :, 2:3]).shape)

    poisson_x = (tangent_x_x * normal_array[:, :, 2:3] + tangent_x_z * normal_array[:, :, 0:1]) / np.sqrt(tangent_x_z**2+tangent_x_x**2)
    poisson_y = (tangent_y_y * normal_array[:, :, 1:2] + tangent_y_z * normal_array[:, :, 0:1]) / np.sqrt(tangent_y_z**2+tangent_y_y**2)

    deviation = 0.01/3

    print(np.min(tangent_x_z))
    tangent_x_map = 256*abs(tangent_x_z)
    tangent_y_map = 256*abs(tangent_y_z)
    cv2.imwrite(file_output[:-4]+"_tangent_y.png", tangent_y_map)
    cv2.imwrite(file_output[:-4]+"_tangent_x.png", tangent_x_map)
    weight_x = (depth_array>-0.7)*(abs(tangent_x_z) >= 0.0*1.2/256)*np.exp(-pow(tangent_x_z, 2)/(2*pow(deviation, 2)))/(deviation*math.sqrt(2*math.pi))
    weight_y = (depth_array>-0.7)*(abs(tangent_y_z) >= 0.0*1.2/256)*np.exp(-pow(tangent_y_z, 2)/(2*pow(deviation, 2)))/(deviation*math.sqrt(2*math.pi))
    cv2.imwrite(file_output[:-4]+"_weightmap_x.png", weight_x*255)
    cv2.imwrite(file_output[:-4]+"_weightmap_y.png", weight_y*255)
    # with open(file_output[:-4]+".txt", "w") as f_out:
    #     for i in range(0, poisson_x.shape[0]):
    #         for j in range(0, poisson_x.shape[1]):
    #             f_out.write("%.2f "%(poisson_x[i, j, 0]))
    #         f_out.write("\n")
    poisson_x = weight_x*(abs(poisson_x**2))
    poisson_y = weight_y*(abs(poisson_y**2))
    #r -x, g-y
    poisson_map_x = poisson_x*256
    poisson_map_y = poisson_y*256
    poisson_map_z = np.zeros((poisson_map_x.shape[0], poisson_map_x.shape[1], poisson_map_x.shape[2]))*255
    print("The loss is")
    print(np.sum(poisson_x+poisson_y)/np.sum(weight_x+weight_y))
    print("The sum of weight is")
    print(np.sum(weight_x+weight_y))

    cv2.imwrite(file_output[:-4]+"_poisson_x.png", poisson_map_x)
    cv2.imwrite(file_output[:-4]+"_poisson_y.png", poisson_map_y)

    poisson_map = np.concatenate((poisson_map_x, poisson_map_y, poisson_map_z), axis=2)

    # print(poisson_map.shape)

    cv2.imwrite(file_output, poisson_map)
