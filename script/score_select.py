import numpy as np 
import csv
import subprocess
import os


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

def main_visibility_other():

	src_dir = "D:/Data"
	dst_dir = "D:/Project/3DFront2Back/stats"
	vis_dir = "D:/Project/3DFront2Back"
	ref_dir = "D:/Data/ShapeNet_test/input"
	
	vis_res = {}

	for c in cls_set:
		
		f_vis = os.path.join(vis_dir, "%s_visibility.csv"%c)

		with open(f_vis) as f_csv:
			csv_reader = csv.reader(f_csv, delimiter=',')
			for row in csv_reader:
				if len(row)!=0 and row[0]!= 'Model':
					vis_res[row[0]] = float(row[4])

	method_set = [
		'pixel2mesh',
		'atlas',
		'occnet',
		'imnet'
	]

	method_score_set = [{}, {}, {}, {}]

	for i in range(0, 4):
		m = method_set[i]

		#read scores
		# score_sym = 0
		# num_sym = 0
		# score_nosym = 0
		# num_nosym = 0
		# score = 0
		# num = 0

		dst_file = os.path.join(dst_dir, "%s.csv"%(m))
		with open(dst_file) as dst_csv:
			print(dst_file)
			dst_csv_reader = csv.reader(dst_csv, delimiter=',')
			for row in dst_csv_reader:
				if len(row) != 0 and row[0] != 'No' and row[1] != '':
					method_score_set[i][row[0]] = float(row[1])

				# and row[0] in intersect_names:
					# num += 1
					# score += float(row[1])
					# # check sym
					# if vis_res[row[0]] > 0.8:
					# 	num_sym += 1
					# 	score_sym += float(row[1])
					# else:
					# 	num_nosym += 1
					# 	score_nosym += float(row[1])

		# print("%s has average intersect score: %f and high visibility number %d, avg score %f ; low visibility number %d, avg score %f."%(m, score/num, num_sym, score_sym/num_sym, num_nosym, score_nosym/num_nosym))


	num_less_80_method = [[0] * 13 for i in range(0, 4)]
	num_more_80_method = [[0] * 13 for i in range(0, 4)]
	score_less_80_method = [[0] * 13 for i in range(0, 4)]
	score_more_80_method = [[0] * 13 for i in range(0, 4)]

	for j in range(0, 13):
		c = cls_set[j]
		print(c)
		items_list = os.listdir(os.path.join(ref_dir, c))

		for item in items_list:
			
			item_name = item[:-8]

			for i in range(0, 4):
			
				if item_name in method_score_set[i].keys():

					# judge if less or more
					if vis_res[item_name] >0.8:
						num_more_80_method[i][j] += 1
						score_more_80_method[i][j] += method_score_set[i][item_name]
					else:
						num_less_80_method[i][j] += 1
						score_less_80_method[i][j] += method_score_set[i][item_name]

		for i in range(0, 4):
			# print(score_more_80_method[i][j])
			# print(num_more_80_method[i][j])
			score_more_80_method[i][j] = score_more_80_method[i][j]/(num_more_80_method[i][j]+1e-6)
			score_less_80_method[i][j] = score_less_80_method[i][j]/(num_less_80_method[i][j]+1e-6)



	with open('visibility_other.csv', 'w', newline='') as f:
		f_csv = csv.writer(f)

		for i in range(0, 4):
			print(method_set[i])
			print(num_more_80_method[i])
			print(num_less_80_method[i])
			print(score_more_80_method[i])
			print(score_less_80_method[i])
			f_csv.writerow(num_more_80_method[i])
			f_csv.writerow(num_less_80_method[i])
			f_csv.writerow(score_more_80_method[i])
			f_csv.writerow(score_less_80_method[i])



def main_sym_other():

	src_dir = "D:/Data"
	dst_dir = "D:/Project/3DFront2Back/stats"
	sym_dir = "D:/Data/ShapeNetRendering_test_res"
	ref_dir = "D:/Data/ShapeNet_test/input"
	
	sym_res = {}

	for c in cls_set:
		
		items_list = os.listdir(os.path.join(ref_dir, "%s"%c))

		for item in items_list:
		
			item_name = item[:-8]
			
			sym_file = os.path.join(sym_dir, "%s/sym/%s.txt"%(c, item_name))

			with open(sym_file, 'r') as f:
			
				line = f.readline()
			
				if len(line) < 4:
					sym_res[item_name] = 0
				else:
					sym_res[item_name] = 1

	method_set = [
		'pixel2mesh',
		'atlas',
		'occnet',
		'imnet'
	]

	for m in method_set:

		#read scores
		score_sym = 0
		num_sym = 0
		score_nosym = 0
		num_nosym = 0
		score = 0
		num = 0

		dst_file = os.path.join(dst_dir, "%s.csv"%(m))
		with open(dst_file) as dst_csv:
			dst_csv_reader = csv.reader(dst_csv, delimiter=',')
			for row in dst_csv_reader:
				if len(row) != 0 and row[0] != 'No' and row[1] != '':
				# and row[0] in intersect_names:
					num += 1
					score += float(row[1])
					# check sym
					if sym_res[row[0]] == 1:
						num_sym += 1
						score_sym += float(row[1])
					else:
						num_nosym += 1
						score_nosym += float(row[1])

		print("%s has average intersect score: %f and sym number %d, avg score %f ; no sym number %d, avg score %f."%(m, score/num, num_sym, score_sym/num_sym, num_nosym, score_nosym/num_nosym))


def main_intersect_sym():

	src_dir = "D:/Data"
	dst_dir = "D:/Project/3DFront2Back/res"
	sym_dir = "D:/Data/ShapeNetRendering_test_res"

	for c in cls_set:
		#read intersect
		intersect_names = []
		src_file = os.path.join(src_dir, "%s_intersect.csv"%c)
		with open(src_file) as src_csv:
			src_csv_reader = csv.reader(src_csv, delimiter=',')
			for row in src_csv_reader:
				if len(row) != 0:
					intersect_names.append(row[0])

		# print(c)
		#read scores

		score_sym = 0
		num_sym = 0
		score_nosym = 0
		num_nosym = 0
		score = 0
		num = 0

		dst_file = os.path.join(dst_dir, "%s_137/pi/metro_%s_dirichlet_2.csv"%(c,c))
		with open(dst_file) as dst_csv:
			dst_csv_reader = csv.reader(dst_csv, delimiter=',')
			for row in dst_csv_reader:
				if len(row) != 0 and row[0] != 'No':
				# and row[0] in intersect_names:
					num += 1
					score += float(row[1])
					# check sym
					sym_file = os.path.join(sym_dir, "%s/sym/%s.txt"%(c, row[0]))
					with open(sym_file, 'r') as f:
						line = f.readline()
						if len(line) < 4:
							score_nosym += float(row[1])
							num_nosym += 1
						else:
							score_sym += float(row[1])
							num_sym += 1


					

		print("%s class has average intersect score: %f and sym number %d, avg score %f ; no sym number %d, avg score %f."%(c, score/num, num_sym, score_sym/num_sym, num_nosym, score_nosym/num_nosym))


def main_intersect():


	src_dir = "D:/Data"
	dst_dir = "D:/Project/3DFront2Back/res/00000000_137/pi"
	score_global = 0
	num_gloabl = 0
	for c in cls_set:
		#read intersect
		intersect_names = []
		src_file = os.path.join(src_dir, "%s_intersect.csv"%c)
		with open(src_file) as src_csv:
			src_csv_reader = csv.reader(src_csv, delimiter=',')
			for row in src_csv_reader:
				if len(row) != 0:
					intersect_names.append(row[0])

		# print(c)
		#read scores

		score = 0
		num = 0

		dst_file = os.path.join(dst_dir, "metro_%s.csv"%c)
		with open(dst_file) as dst_csv:
			dst_csv_reader = csv.reader(dst_csv, delimiter=',')
			for row in dst_csv_reader:
				if len(row) != 0 and row[0] in intersect_names:
					num += 1
					score += float(row[1])

					num_gloabl += 1
					score_global += float(row[1])

		print("%s class has average intersect score: %f and %d data."%(c, score/num, num))

	print("In total %d data, we have %f avg score."%(num_gloabl, score_global/num_gloabl))

def main_intersect_texture():
	src_dir = "E:\\ShapeNetCore.v1\\ShapeNetCore.v1"
	# src_dir = "D:\\Data\\ShapeNetCore.v1"
	ref_dir = "D:/Data"
	dst_dir = "D:/Project/3DFront2Back/res"

	score_global = 0
	score_global_texture = 0
	num_gloabl = 0
	num_global_texture = 0

	for c in cls_set:
		#read intersect
		intersect_names = []
		src_file = os.path.join(ref_dir, "%s_intersect.csv"%c)
		with open(src_file) as src_csv:
			src_csv_reader = csv.reader(src_csv, delimiter=',')
			for row in src_csv_reader:
				if len(row) != 0:
					intersect_names.append(row[0])

		# print(c)
		#read scores

		score_local = 0
		num_local = 0
		score_local_texture = 0
		num_local_texture = 0

		dst_file = os.path.join(dst_dir, "%s_137/pi/metro_%s_dirichlet_2.csv"%(c,c))
		with open(dst_file) as dst_csv:
			dst_csv_reader = csv.reader(dst_csv, delimiter=',')
			for row in dst_csv_reader:
				if len(row) != 0 and row[0] in intersect_names:
					num_local += 1
					num_gloabl += 1
					score_local += float(row[1])
					score_global += float(row[1])

					#check texture
					items_local = os.listdir(os.path.join(src_dir, c, c, row[0]))
					if len(items_local) > 2:
						num_local_texture += 1
						num_global_texture += 1
						score_global_texture += float(row[1])
						score_local_texture += float(row[1])


		print("%s class has %d data, average score: %f and %d have textures, avg score is %f."%(c, num_local, score_local/num_local, num_local_texture, score_local_texture/num_local_texture))

	print("In total %d data, we have %f avg score and %d texture, %f avg score."%(num_gloabl, score_global/num_gloabl, num_global_texture, score_global_texture/num_global_texture))



def main_intersect_visibility():

	headers = ['Model', 'MD Score' , 'Front', 'Back', 'Front+Back']

	dst_dir = "D:/Project/3DFront2Back/res"
	src_dir = "D:/Data/ShapeNet_test/rot_gt"
	ref_dir = "D:/Data"
	out = "out_visibility.txt"

	with open(out, 'w') as f_out:
		for c in cls_set:
			f_visibility = "%s_visibility.csv"%c
			with open(f_visibility, 'w', newline='') as f:
				f_csv = csv.writer(f)
				f_csv.writerow(headers)
				#read intersection
				intersect_names = []
				src_file = os.path.join(ref_dir, "%s_intersect.csv"%c)
				with open(src_file) as src_csv:
					src_csv_reader = csv.reader(src_csv, delimiter=',')  
					for row in src_csv_reader:
						if len(row) != 0:
							intersect_names.append(row[0])

				num_high = 0
				num_low = 0
				score_high = 0
				score_low = 0

				dict_loss = {}
				f_loss = "D:/Project/3DFront2Back/res/%s_137/pi/metro_%s_dirichlet_2.csv"%(c, c)
				with open(f_loss) as csv_file:
					csv_reader = csv.reader(csv_file, delimiter=',')
					for row in csv_reader:
						if(len(row) != 0) and row[0] != 'No':
							dict_loss[row[0]] = float(row[1])

				exe_path = "D:/Project/3DFront2Back/visibility/visible-surface-area/x64/Release/visible-surface-area.exe"
				txt_path = "tmp.txt"
				src_dir = "D:/Data/ShapeNet_test/rot_gt/%s"%c

				items = os.listdir(src_dir)

				for item in items:
					
					if item[:-4] not in  intersect_names:
						
						continue
					print(item)
					model_path = os.path.join(src_dir, item)
					command=[exe_path, model_path, txt_path]
					subprocess.call(command)

					with open(txt_path, 'r') as f:
				
						lines = f.readlines()
						front_vis = float(lines[0])
						back_vis = float(lines[1])
						full_vis = float(lines[2])
				
						print("Model %s's visibility: front - %f, back - %f, full - %f."%(item, front_vis, back_vis, full_vis))
						item_name = item[:-4]
						f_csv.writerow([item_name, dict_loss[item_name], front_vis, back_vis, full_vis])

						if full_vis <0.8:
							num_low+=1
							score_low += dict_loss[item_name]
						else:
							num_high +=1
							score_high += dict_loss[item_name]
			out_msg = "%s class, > 80 data %d and avg score is %f, < 80 data %d and avg score is %f.\n"%(c, num_high, score_high/(num_high+1e-8), num_low, score_low/(num_low+1e-8))
			print(out_msg)
			f_out.write(out_msg)

if __name__ == '__main__':
	main_visibility_other()
