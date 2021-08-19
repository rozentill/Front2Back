import csv 
import matplotlib.pyplot as plt


loss_file = "epoch_loss.csv"
train_Y = []
eval_Y = []
X = []
with open(loss_file) as csvfile:
	spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
	for row in spamreader:
		# parts = row[0].split(',')
		# if parts[1] == "03001627":
		# 	dict_split[parts[3]] = parts[4]
		# print(row)

		if row[1] == "train":
			X.append(float(row[0]))
			train_Y.append(float(row[2]))
		elif row[1] == " eval":
			eval_Y.append(float(row[2]))

# assert(len(eval_Y)==len(train_Y))


plt.plot(train_Y)
plt.plot(eval_Y)

plt.show()
