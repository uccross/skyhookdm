<!---
  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing,
  software distributed under the License is distributed on an
  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied.  See the License for the
  specific language governing permissions and limitations
  under the License.
-->

# Benchmarking SkyhookDM

1. Clone the repository
```bash
git clone https://github.com/uccross/skyhookdm
cd skyhookdm/scripts/deploy/
```

2. Execute `deploy_ceph.sh` script to deploy a Ceph cluster on a set of nodes and to mount CephFS on the client/admin node. On the client node, execute:

```bash
./deploy_ceph.sh mon1,mon2,mon3 osd1,osd2,osd3 mds1 mgr1 /dev/sdb 3
```
where mon1, mon2, osd1, etc. are the internal hostnames of the nodes.

3. Build and install the SkyhookDM CLS plugins and PyArrow (with Skyhook extensions):

```bash
./deploy_skyhook.sh osd1,osd2,osd3
```
This will build the CLS plugins as shared libraries and deploy them to the OSD nodes.

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

7. Run the [benchmark script](../../scripts/benchmark/bench.py) given below to get some initial benchmarks for SkyhookDM performance while using different row selectivities.

```bash
python3 bench.py [format(pq/rpq)] [iterations] [file:///path/to/dataset] [workers] [result file]
```

For example,
```bash
python3 bench.py rpq 10 file:///mnt/cephfs/dataset 16 result.json
```
