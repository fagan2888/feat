import glob

def replace_brackets(f):
    with open(f,'r') as out:
        lines = out.readlines()
    
    for i in range(len(lines)):
        if '#include <shogun' in lines[i] or '#include<shogun' in lines[i]:
            print('\tfixing',lines[i])            
            lines[i] = lines[i].replace('<','\"ml/')
            lines[i] = lines[i].replace('>','\"')
            print('\tlines[i] now:',lines[i])

    with open(f,'w') as out:
        out.writelines(lines)

# recurse through directories
for filename in glob.iglob('shogun/' + '**/*.h', recursive = True):
    print('processing',filename)
    replace_brackets(filename)
for filename in glob.iglob('shogun/' + '**/*.cpp', recursive = True):
    print('processing',filename)
    replace_brackets(filename)

