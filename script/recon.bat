for /L %%i in (1318, 1, 1323) do %cd%/../3rdparty/poisson/PoissonRecon --in %cd%/../res/pointcloud/%%i.npts --out %cd%/../res/mesh/%%i.ply --depth 8 --pointWeight 1