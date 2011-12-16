import os, sys

if __name__ == '__main__':
    """ convert a GLSL shader to a header file, dump to stdout"""

    if len(sys.argv) != 3:
        print("usage: python glsl2h.py <GLSL source> <variable name>")
        exit(0)

    print("const char* %s = \n" % sys.argv[2])
    with open(sys.argv[1]) as fp:
        for line in fp.readlines():
            print('"%s\\n"' % line.rstrip())
    print(";\n")

