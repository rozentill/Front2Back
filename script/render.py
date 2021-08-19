import subprocess
from os import listdir, walk
from os.path import isfile, join, splitext


if __name__ == '__main__':

	renderer_path = "D:/Project/3DFront2Back/render/mitsuba-shapenet/dist/mitsuba.exe"
	
	dataset_dir = "D:/Data/Front2Back"
	cls_name = "03001627_test"
	src_dir = join(dataset_dir, cls_name)

	num = 0
	### for speed up,  dir .\1006be65e7bc937e9141f9b58470d646\*.xml | %{..\..\..\Project\3DFront2Back\render\mitsuba-shapenet\dist\mitsuba.exe -xj 8 $_}
	# for item in listdir(src_dir):
		# num += 1
		# print(item)
		# print(num)
		# model_dir = join(src_dir, item)
		# for root, dirs, files in walk(model_dir):
		# 	for filename in files:
		# 		base, extension = splitext(filename)
		# 		if extension == ".xml" and not isfile(join(root,base+".png")):
		# 			print(base)
		# 			f_xml = join(root, filename)
		# 			ps = subprocess.call([renderer_path, f_xml, "-xq"], shell=True)
	for i in range(0, 200):
		print(i)
		file = "%d_rgb.xml"%i 
		f_xml = join(src_dir, file)
		
		ps = subprocess.call([renderer_path, f_xml, "-xq"], shell=True)

