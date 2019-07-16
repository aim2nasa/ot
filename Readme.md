# ot
Optee Test를 줄여서 붙인 이름

# 동기
optee를 테스트하는 테스트 케이스

# Note
Compatible optee version is 2.6.

# compile example
1. Set cross compiler
    export CROSS_COMPILE=~/ot/optee/2.6/toolchains/aarch64/bin/aarch64-linux-gnu-

2. For host application
    export TEEC_EXPORT=~/ot/optee/2.6

3. For Trusted Application
    export TA_DEV_KIT_DIR=~/ot/optee/2.6/arm/export-ta_arm64

4. for host/TA under keygen folder
    export OKEY_ROOT=~/ot/keygen/host/libokey

5. Dowload toolchains
    make -f toolchain.mk 

6. make
