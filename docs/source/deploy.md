# Getting Started

## Installing Ceph and SkyhookDM

1. Execute [this](https://github.com/uccross/skyhookdm/blob/master/scripts/deploy/deploy_ceph.sh) script to deploy a Ceph cluster on a set of nodes and to mount CephFS on the client/admin node. For example, on the client node, execute
```bash
./deploy_ceph.sh node1,node2,node3 node4,node5,node6,node7 node1 node2
```
where node[1..7] are the internal hostnames of the nodes. 

2. Build and install the SkyhookDM CLS plugins and PyArrow (with Skyhook extensions) using [this](https://github.com/uccross/skyhookdm/blob/master/scripts/deploy/deploy_skyhook.sh) script. For example,
```bash
./deploy_skyhook.sh osd1,osd2,osd3
```
This will build the CLS plugins as shared libraries and deploy them to the OSD nodes.

3. Optionally, we can also deploy Prometheus and Grafana for monitoring the cluster by following [this](https://github.com/JayjeetAtGithub/prometheus-on-baremetal) guide.

## Interacting with SkyhookDM

1. Download some sample Parquet files to the admin node.
```bash
wget https://skyhook-ucsc.s3.us-west-1.amazonaws.com/128MB.uncompressed.parquet
``` 

2. Create and write a sample dataset to the CephFS mount using [this](https://github.com/uccross/skyhookdm/blob/master/scripts/deploy/deploy_data.sh) script by replicating the 128MB Parquet file downloaded in Step 1.
```bash
./deploy_data.sh 128MB.uncompressed.parquet /mnt/cephfs/dataset 100 134217728
```
This will write 100 of ~128MB Parquet files to `/mnt/cephfs/dataset` using a CephFS stripe size of 128MB.

3. Build and run the example client code.
```bash
g++ scripts/example.cc -larrow_skyhook_client -larrow_dataset -larrow -o example
./example file:///mnt/cephfs/dataset
```
