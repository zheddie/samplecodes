import argparse
#import torch
from timeit import default_timer as timer

parser = argparse.ArgumentParser()
parser.add_argument("numberOfLoops", help="The number of loops done", type=int)

args = parser.parse_args()

def main():
    print ("testPyTorch main()")
    start = timer()
    for loop in range(args.numberOfLoops):
        a=1
        #imgTensor = torch.Tensor(1, 3, 100, 100)  # 1, C, H, W
    end = timer()
    print ("Execution time: {}".format(end - start))

if __name__ == "__main__":
    main()
