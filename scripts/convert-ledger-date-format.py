import sys
import re

file = sys.argv[1]
reg = re.compile(r'^(\d\d\d\d)/(\d?\d)/(\d?\d) (.*)')
fp = open(file, "r")
for line in fp.readlines():
    g = re.match(reg, line)
    if g:
        line = "%04d-%02d-%02d %s\n" % (int(g.group(1)), int(g.group(2)), int(g.group(3)), g.group(4))
    sys.stdout.write(line)
fp.close()
