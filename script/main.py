import argparse

from init import init
from node import node
from unsafe_reset import clear
from info import info


def main():
    parser = argparse.ArgumentParser(description='Band Protocol Layer')
    parser.add_argument('instruction', help='Instruction sets')
    parser.add_argument('--debug', dest='debug', action='store_true')
    parser.add_argument('--key', help='Key to find value')
    args = parser.parse_args()

    if args.instruction == 'node':
        node(args.debug)
    elif args.instruction == 'init':
        init()
    elif args.instruction == 'clear':
        clear()
    elif args.instruction == 'info':
        info(args.key)
    else:
        raise Exception("This instruction is not supported")


if __name__ == "__main__":
    main()
