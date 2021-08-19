import csv
from argparse import ArgumentParser
from os.path import join

def parse_args():

	parser = ArgumentParser()
	parser.add_argument('--src', type=str, default=None)
	parser.add_argument('--dst', type=str, default=None)
	# parser.add_argument('--dst', type=str, default=None)
	args = parser.parse_args()
	return args.src, args.dst

headers = ['Number','Ours_current','Ours_Saterday','Ours_Saterday-Ours_current']


# f_in, f_out = parse_args()
# f_out = 'D:/Project/occupancy_networks/out/f2b_gt_sym/eval.csv'

# with open(f_out,'w') as f:
#     f_csv = csv.writer(f)
#     f_csv.writerow(headers)

#     with open(f_in, 'r') as fin:
#     	lines = fin.readlines()
#     	for line in lines:
#     		parts = line.split(' ')
#     		f_csv.writerow(parts)

src = "D:\\Project\\3DFront2Back\\3rdparty\\metro\\compare\\output"
dst = "log\\eval_03001627_current_old.csv"
num = 0

with open(dst, 'w') as f:

    f_csv = csv.writer(f)
    f_csv.writerow(headers)

    for i in range(1, 21):

        f_ours = join(src, "%d_ours_test.txt"%i)
        f_their = join(src, "%d_ours_shift.txt"%i)
        
        score_ours = 0
        score_their = 0

        with open(f_ours, 'r') as f_o:
        
            score_ours = float(f_o.read())

        with open(f_their, 'r') as f_t:
        
            score_their = float(f_t.read())

        if score_ours-score_their < 0:
            num+=1

        f_csv.writerow([i, score_ours, score_their, score_their-score_ours])

print("Total %d."%num)
