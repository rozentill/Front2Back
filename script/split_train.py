import csv

split_file = "D:/Data/shapenet_split.csv"
num = 0
dict_split = {}
with open(split_file, newline='') as csvfile:
	spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
	for row in spamreader:
		parts = row[0].split(',')
		if parts[1] == "03001627":
			dict_split[parts[3]] = parts[4]
		# print(', '.join(row))

	print("number of chair")
	print(len(dict_split))