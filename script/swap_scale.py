


if __name__ == '__main__':
	
	src = "D:/Data/test3.obj"
	dst = "D:/Data/test3_test.obj"

	v = []
	face = []
	offset = 0.913124897465
	with open(src, 'r') as f:

		lines = f.readlines()
		for line in lines:
			parts = line.split(' ')

			if parts[0] == 'v':
				
				v.append([(float(parts[1]))/0.57, (float(parts[2]))/0.57, (float(parts[3])+offset)/0.57])
			
			if parts[0] == 'f':

				face.append([float(parts[1]), float(parts[2]), float(parts[3])])

	with open(dst, 'w') as f:

		for i in range(len(v)):
			f.write("v %f %f %f\n"%(v[i][0], v[i][1], v[i][2]))

		for i in range(len(face)):
			f.write("f %d %d %d\n"%(face[i][0], face[i][1], face[i][2]))


