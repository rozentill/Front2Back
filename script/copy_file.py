import os
import shutil

sourcedir = r"D:\Data\ShapeNetCore.v2\ShapeNetCore.v2\02828884"
test_dir = r"D:\Project\3DFront2Back\data\model\model_bench\test"
train_dir = r"D:\Project\3DFront2Back\data\model\model_bench\train"

n_models = len(next(os.walk(sourcedir))[1])
n_test = int(n_models * 0.2)
n_train = int(n_models - n_test)
print("# models:" + str(n_models))
print("# test:" + str(n_test))
print("# train:" + str(n_train))

count = 1

for item in os.listdir(sourcedir):
	model_dir = os.path.join(sourcedir, item, "models")
	for root, dirs, files in os.walk(model_dir):
		for filename in files:
			base, extension = os.path.splitext(filename)
			if extension == ".obj":
				old_name = os.path.join(root, filename)
				
				if count <= n_test:
					new_name = os.path.join(test_dir, str(count) + extension)
					print(new_name)
					shutil.copy(old_name, new_name)
					print("copied", old_name, "as", new_name)
				else:
					new_name = os.path.join(train_dir, str(count) + extension)
					print(new_name)
					shutil.copy(old_name, new_name)
					print("copied", old_name, "as", new_name)
				
				count += 1
