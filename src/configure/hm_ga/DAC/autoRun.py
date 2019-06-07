#!/usr/bin/python3

import os
def runJob():
    # run job
    command = "./run.sh"
    result = os.system(command)

    return result

def main():
    total = 3
    failed = 0
    succeed = 0
    for i in range(total):
        print("total: ",total)
        print("succeed:",succeed)
        print("failed",failed)
        result = runJob()
        if result == 0:
            succeed += 1
        else:
            failed +=1

    print("total: ", total)
    print("succeed:", succeed)
    print("failed", failed)

main()