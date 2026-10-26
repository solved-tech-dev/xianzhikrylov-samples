# Build all independent samples using their local Makefiles.
SAMPLE_DIRS := 01_cg 02_bicg 03_bicgstab 04_gmres 05_fgmres 06_gcr 07_pcg_jacobi 08_fcg_jacobi 09_bicg_jacobi 10_bicgstab_jacobi 11_gmres_jacobi 12_fgmres_jacobi 13_gcr_jacobi 14_bicg_ilu0 15_bicgstab_ilu0 16_gmres_ilu0 17_fgmres_ilu0 18_gcr_ilu0 19_pcg_ic0 20_fcg_ic0 21_gmres_ic0 22_fgmres_ic0 23_bicgstab_ic0 24_bicg_ic0 25_gcr_ic0 26_bsr4_fgmres_ilu0 27_bsr4_fgmres_jacobi 28_bsr4_fgmres_dilu 29_msr_bicgstab_jacobi 30_ldu_pcg_dic 31_ldu_bicgstab_dilu 32_complex_batch3
.PHONY: all clean $(SAMPLE_DIRS)
all: $(SAMPLE_DIRS)
$(SAMPLE_DIRS):
	$(MAKE) -C $@ all
clean:
	@set -e; for dir in $(SAMPLE_DIRS); do $(MAKE) -C "$$dir" clean; done
