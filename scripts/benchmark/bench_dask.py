import sys
import time

import dask.dataframe as dd
from dask.distributed import Client, LocalCluster


if __name__ == "__main__":
    cluster = LocalCluster(n_workers=4, threads_per_worker=4, memory_limit='64GB')
    client = Client(cluster)
    print(client)

    path = sys.argv[1]
    engine = sys.argv[2]

    print(path)
    print(engine)

    df = dd.read_parquet(path, engine=engine)
    s = time.time()
    df.compute()
    e = time.time()

    print(e - s) 
