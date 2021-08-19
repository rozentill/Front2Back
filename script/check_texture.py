import os

cls_set = [
	# "02691156",
	# "02828884",
	# "02933112",
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

def main():

	src_dir = "D:\\Data\\ShapeNetCore.v1"
	num_global = 0
	num_global_texture = 0
	for c in cls_set:
		num_local_texture = 0
		num_local = 0
		items_list = os.listdir(os.path.join(src_dir, c))

		for item in items_list:
			num_local += 1
			num_global += 1

			items_local = os.listdir(os.path.join(src_dir, c, item))
			if len(items_local) > 2:
				num_local_texture += 1
				num_global_texture += 1
		
		print("%s class has %d total data, and %d has textures, percentage %f."%(c, num_local, num_local_texture, float(num_local_texture)/num_local))



	print("Overall %d total data, and %d has textures, percentage %f."%(num_global, num_global_texture, float(num_global_texture)/num_global))




if __name__ == '__main__':
	main()