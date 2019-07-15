NFS=/srv/nfs/linaro-stretch-developer-20180416-89
ROOT=$NFS/root
TA_DIR=$NFS/lib/optee_armtz

echo "NFS="$NFS
echo "ROOT="$ROOT
echo "TA_DIR="$TA_DIR

mkdir -p $TA_DIR

cp hello/host/hello $ROOT
cp hello/ta/3c45d356-f9d5-11e7-8c3f-9a214cf093ae.ta $TA_DIR

cp keygen/host/keyDel/keydel $ROOT
cp keygen/host/keyEncDec/ked $ROOT
cp keygen/host/keyEnum/keyenum $ROOT
cp keygen/host/keyGen/keygen $ROOT
cp keygen/host/keyInj/keyinj $ROOT
cp keygen/host/keyOpen/keyopen $ROOT
cp keygen/host/keyOpen/keyopen $ROOT
cp keygen/host/keyXtr/keyxtr $ROOT
cp keygen/host/libokey/libokey.so.1 $NFS/lib
cp keygen/ta/ab201cb2-177d-11e8-b642-0ed5f89f718b.ta $TA_DIR

cp persistentObj/host/persistentObj $ROOT
cp persistentObj/ta/cfe62966-04cb-11e8-ba89-0ed5f89f718b.ta $TA_DIR

cp storage/host/perObj/perObj $ROOT
cp storage/ta/b689f2a7-8adf-477a-9f99-32e90c0ad0a2.ta $TA_DIR
