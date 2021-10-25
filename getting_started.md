# Getting started with SkyhookDM

1. Deploy a Ceph cluster. 

2. Build and Deploy SkyhookDM.
```bash
git clone https://github.com/apache/arrow
cd arrow/
mkdir -p build/release
cmake -DARROW_SKYHOOK=ON ..
make -j$(nproc) install
```

3. Copy skyhook shared libraries into the OSD nodes at `/usr/lib/rados-classes`.
```
scp build/release/libcls_skyhook* [hostname]:/usr/lib/rados-classes
```

