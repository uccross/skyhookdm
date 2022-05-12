import sys

import dask.dataframe as dd
from dask.distributed import Client


if __name__ == "__main__":
    client = Client()
    print(client)

    path = sys.argv[1]
    engine = sys.argv[2]

    print(path)
    print(engine)

    df = dd.read_parquet(path, engine=engine)
    df.compute() 
