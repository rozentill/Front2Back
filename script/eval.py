import os
from os.path import join
import csv

def main_eval_gt():

	metro = "3rdparty\\metro\\metro"
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

	for c in range(9, 10):		


		cls_name = cls_set[c]
		ref_dir = "D:\\Data\\ShapeNet_test\\rot_gt\\%s"%cls_name
		res_dir = "res\\03001627_137_%s\\pi"%cls_name
		# res_dir = "D:\\Project\\occupancy_networks\\demo\\generation\\meshes"

		header = ["No", "Error"]
		with open(join(res_dir, "metro_%s.csv"%cls_name), 'w', newline="") as f:

			f_csv = csv.writer(f)
			f_csv.writerow(header)

			items = os.listdir(ref_dir)

			for item in items:
				if "samples" in item:
					continue
				# if os.path.exists(join(res_dir, item[:-4]+".txt")):
				# 	pass
				# else:
				# f_samples = 
				print(item)
				filename = join(res_dir, item[:-4]+".ply")
				filename_sym = join(res_dir, item[:-4]+".ply")
				if not os.path.exists(filename):
					continue
					# join(ref_dir, item)
				os.system("%s %s %s %s.txt -n10000"%(metro, filename, join(ref_dir, item), join(res_dir,"output", item[:-4])))
				score = 0
				with open(join(res_dir,"output", item[:-4]+".txt"), 'r') as f_score:
					letter = f_score.read()
					if letter == "":
						print(item)
						continue
					score = float(letter)

				f_csv.writerow([item[:-4], score])

def main_eval_symmetry():

	metro = "3rdparty\\metro\\metro"
	cls_name = "03001627"
	ref_dir = "D:\\Data\\ShapeNet_test\\rot_gt\\%s"%cls_name
	res_dir = "res\\%s_137_sym"%cls_name

	header = ["No", "MD(w/o sym)", "MD(w/ sym)"]

	with open(join(res_dir, "metro_sym_%s.csv"%cls_name), 'w', newline='') as f_write:
		
		f_csv = csv.writer(f_write)
		f_csv.writerow(header)

		item_list = os.listdir(ref_dir)

		for item in item_list:
			if "samples" in item:
				continue

			f_nosym = join(res_dir, item[:-4]+'_nosym.obj')

			if not os.path.exists(f_nosym):
				continue
			f_nosym_sym = join(res_dir, item[:-4]+'_nosym_sym.obj')
			
			# compute nosym score
			score_nosym = 0
			os.system("%s %s %s %s.txt -n10000"%(metro, f_nosym, f_nosym_sym, join(res_dir,"output", item[:-4])))
			with open(join(res_dir,"output", item[:-4]+".txt"), 'r') as f_score:
				letter = f_score.read()
				if letter == "":
					continue
				score_nosym = float(letter)

			score_sym = 0
			f_sym = join(res_dir, item[:-4]+'_sym.obj')
			f_sym_sym = join(res_dir, item[:-4]+'_sym_sym.obj')
			os.system("%s %s %s %s.txt -n10000"%(metro, f_sym, f_sym_sym, join(res_dir,"output", item[:-4])))
			with open(join(res_dir,"output", item[:-4]+".txt"), 'r') as f_score:
				letter = f_score.read()
				if letter == "":
					continue
				score_sym = float(letter)

			f_csv.writerow([item[:-4], score_nosym, score_sym])



if __name__ == '__main__':

	main_eval_gt()