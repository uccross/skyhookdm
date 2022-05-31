#!/bin/bash
set -ex

./deploy_ceph.sh node1,node2,node3 node1,node2,node3,node4,node5,node6,node7,node8,node9,node10,node11,node12,node13,node14,node15,node16 node1 node2 /dev/nvme0n1p4 3
ceph osd pool set cephfs_data pg_num 512
./deploy_skyhook.sh node1,node2,node3,node4,node5,node6,node7,node8,node9,node10,node11,node12,node13,node14,node15,node16 arrow-master true true false 32
