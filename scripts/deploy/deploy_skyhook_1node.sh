#!/bin/bash
set -ex

./deploy_ceph.sh node1 node1 node1 node1 /dev/nvme0n1p4 1
./deploy_skyhook.sh node1 arrow-master true true false 32
