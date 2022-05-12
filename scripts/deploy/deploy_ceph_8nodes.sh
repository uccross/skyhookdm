#!/bin/bash
set -ex

./deploy_ceph.sh node1,node2,node3 node1,node2,node3,node4,node5,node6,node7,node8 node1 node2 /dev/nvme0n1p4 3
