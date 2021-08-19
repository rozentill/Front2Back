import os
from os.path import join
import csv

def main_eval_gt():

	metro = "metro\\metro"
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

	for c in range(0, 13):		


		cls_name = cls_set[c]
		ref_dir = "rot_gt\\%s"%cls_name
		res_dir = "results\\%s"%cls_name

		header = ["No", "Error"]
		with open(join(res_dir, "metro_%s.csv"%cls_name), 'w', newline="") as f:

			f_csv = csv.writer(f)
			f_csv.writerow(header)

			items = os.listdir(ref_dir)

			for item in items:
				if "samples" in item:
					continue

				print(item)
				filename = join(res_dir, item[:-4]+".ply")
				if not os.path.exists(filename):
					continue
				os.system("%s %s %s %s.txt -n10000"%(metro, filename, join(ref_dir, item), join(res_dir,"output", item[:-4])))
				score = 0
				with open(join(res_dir,"output", item[:-4]+".txt"), 'r') as f_score:
					letter = f_score.read()
					if letter == "":
						continue
					score = float(letter)

				f_csv.writerow([item[:-4], score])

if __name__ == '__main__':

	main_eval_gt()