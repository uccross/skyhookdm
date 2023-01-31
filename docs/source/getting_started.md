# Getting Started

1. Launch a cluster of VMs/bare-metal nodes, choose one of them as the client node and clone the repository in there.
```bash
git clone https://github.com/uccross/skyhookdm
cd skyhookdm/scripts/deploy/
```

2. Execute the `deploy_ceph.sh` script to deploy a Ceph cluster on a set of nodes and also to mount CephFS on the client node. On the client node, execute:

```bash
./deploy_ceph.sh mon1,mon2,mon3 osd1,osd2,osd3 mds1 mgr1 /dev/sdb 3
```
where mon1, mon2, osd1, etc. are the internal hostnames of the nodes. Similarly on CloudLab, you can execute,

```bash
./deploy_ceph.sh node1,node2,node3 node4,node5,node6,node7 node1 node2 /dev/nvme0n1p4 3
```

3. Build and install Arrow along with the SkyhookDM object class plugins.

```bash
./deploy_skyhook_upstream.sh osd1,osd2,osd3
```
This will build the plugins as shared libraries and deploy them to the OSD nodes. On CloudLab, you can do,

```bash
./deploy_skyhook_upstream.sh node4,node5,node6,node7
```

4. Download an example Parquet file with NYC Taxi data.
```bash
wget https://skyhook-ucsc.s3.us-west-1.amazonaws.com/128MB.uncompressed.parquet
```

5. Write a sample dataset to the CephFS mount by replicating the 128MB Parquet file downloaded in the previous step.

```bash
./deploy_data.sh [source file] [destination dir] [no. of copies] [stripe unit]
```

For example,
```bash
./deploy_data.sh 128MB.uncompressed.parquet /mnt/cephfs/dataset 10 134217728
```

This will write 10 of ~128MB Parquet files to `/mnt/cephfs/dataset` using a CephFS stripe size of 128MB. 

6. Build and run the example client code.
```bash
g++ -std=c++17 ../example.cc -larrow_skyhook -larrow_dataset -larrow -o example
export LD_LIBRARY_PATH=/usr/local/lib
./example file:///mnt/cephfs/dataset
```

You should get a stringified Arrow table as the output.
