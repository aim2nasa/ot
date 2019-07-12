# ot
Optee Test를 줄여서 붙인 이름

## 동기
optee를 테스트하는 테스트 케이스(https://github.com/OP-TEE/optee_test)에는 TEE스펙을 검증하는 Full테스트들이 있다.
직접 이 소스들을 보기보단, 내가 관심을 갖고 있는 것들만 갖고와서 하나씩 살펴보는데 편한 환경을 구축한다.
추후 여기서 나를 위해 만든 테스트 케이스중 외부에 오픈해도 문제가 없는 부분들은 https://github.com/aim2nasa/optee_test에 merge할 생각이다.

# compile example
1. Set cross compiler
    export CROSS_COMPILE=~/ot/optee/3.6/toolchains/aarch64/bin/aarch64-linux-gnu-

2. For host application
    export TEEC_EXPORT=~/ot/optee/3.6

3. For Trusted Application
    export TA_DEV_KIT_DIR=~/ot/optee/3.6/arm/export-ta_arm64

4. for host/TA under keygen folder
    export OKEY_ROOT=~/ot/keygen/host/libokey

5. Dowload toolchains
    make -f toolchain.mk 

6. make
