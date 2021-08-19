import os
from os.path import join

def main():

	poisson = "3rdparty\\poisson\\PoissonRecon"
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
		ref_dir = "D:\\Data\\ShapeNet_test\\input\\%s"%cls_name
		res_dir = "res\\03001627_137_%s\\pi"%cls_name

		items = os.listdir(ref_dir)

		for item in items:
			# if "5a3228a34ec0572b4b3c42e318f3affc" not in item:
			# 	continue
			item_name = item[:-8]
			if not os.path.exists("%s.npts"%join(res_dir, item_name)):
				continue
			os.system("%s --in %s.npts --out %s.ply --depth 8 --pointWeight 1 --bType 2"%(poisson, join(res_dir, "%s"%item_name), join(res_dir, "%s"%item_name)))
			# os.system("%s --in %s_sym.npts --out %s_sym.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, "%s"%item_name), join(res_dir, "%s"%item_name)))

			# os.system("%s --in %s_nosym_predict.npts --out %s_nosym_predict.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))
			# os.system("%s --in %s_sym_predict.npts --out %s_sym_predict.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))
			# os.system("%s --in %s_sym_after.npts --out %s_sym_after.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))
			# os.system("%s --in %s_sym_recon.npts --out %s_sym_recon.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))

			# os.system("%s --in %s_nosym_predict_sym.npts --out %s_nosym_predict_sym.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))
			# os.system("%s --in %s_sym_predict_sym.npts --out %s_sym_predict_sym.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))
			# os.system("%s --in %s_sym_after_sym.npts --out %s_sym_after_sym.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))
			# os.system("%s --in %s_sym_recon_sym.npts --out %s_sym_recon_sym.ply --depth 8 --pointWeight 2 --bType 2"%(poisson, join(res_dir, item[:-8]), join(res_dir, item[:-8])))

if __name__ == '__main__':
	main()