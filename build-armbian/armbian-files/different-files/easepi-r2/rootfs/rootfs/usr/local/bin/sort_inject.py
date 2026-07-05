#!/usr/bin/env python3
import sys

def main():
    if len(sys.argv) != 3:
        print("Usage: sort_inject.py <input_file> <output_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    with open(input_file, "r") as f:
        content = f.read()
    
    old = "sort((e,t)=>e.index-t.index)"
    new = 'sort((e,t)=>{const o=["eth0","eth1","eth2","eth3","wlan0","4Gnet"];return o.indexOf(e.name)-o.indexOf(t.name)})'
    
    content = content.replace(old, new)
    
    with open(output_file, "w") as f:
        f.write(content)
    
    print("Done")

if __name__ == "__main__":
    main()
