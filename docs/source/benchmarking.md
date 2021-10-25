## Getting started with SkyhookDM

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
./deploy_skyhook.sh osd1,osd2,osd3
```
This will build the plugins as shared libraries and deploy them to the OSD nodes. On CloudLab, you can do,

```bash
./deploy_skyhook.sh node4,node5,node6,node7
````

4. Download a sample dataset from [this](https://github.com/jayjeetc/datasets) repository:

```bash
apt update
apt install git-lfs
git clone https://github.com/jayjeetc/datasets
cd datasets/
git lfs pull
cd ..
```

5. Create and write a sample dataset to the CephFS mount by replicating the 16MB Parquet file downloaded in the previous step:

```bash
./deploy_data.sh [source file] [destination dir] [no. of copies] [stripe unit]
```

For example,

```bash
./deploy_data.sh datasets/16MB.parquet /mnt/cephfs/dataset 1500 16777216
```

This will write 1500 of ~16MB Parquet files to `/mnt/cephfs/dataset` using a CephFS stripe size of 16MB. 

6. Optionally, you can also deploy Prometheus and Grafana for monitoring the cluster by following [this](https://github.com/JayjeetAtGithub/prometheus-on-baremetal#readme) guide.

7. Run the [benchmark script](../../scripts/benchmark/bench.py) to get some initial benchmarks for SkyhookDM performance while using different row selectivities.

```bash
python3 bench.py [format(pq/rpq)] [iterations] [file:///path/to/dataset] [workers] [result file]
```

For example,
```bash
python3 bench.py rpq 10 file:///mnt/cephfs/dataset 16 result.json
```
