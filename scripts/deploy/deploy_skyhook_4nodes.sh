#!/bin/bash
set -ex

./deploy_ceph.sh node1,node2,node3 node1,node2,node3 node1 node2 /dev/nvme0n1p4 3
./deploy_skyhook.sh node1,node2,node3 arrow-master true true false 32
