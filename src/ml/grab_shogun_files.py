# grab a file, search it for include paths and recursively copy those files into this directory.
from shutil import copyfile, copytree
import os
import sys
shogun_dir = '../../../shogun/src/'
dest_dir = ''

################################################################## copy rxcpp files
print('===\ncopying rxcpp\n===')
copytree(shogun_dir+'shogun/rxcpp','shogun/rxcpp')

print('===\ngrabbing files\n===')

if len(sys.argv)>1:
    files = sys.argv[1]
else:
    files = 'track'
fnames = []
copiedFiles = {}

with open(files,'r') as out:
    for line in out:
        print('tracing',line)
        fnames.append(line.strip())
            
# function to copy files and recurse on them
def grab_dependencies(f):
    print('copying',f)
    
    if f not in copiedFiles:
        copiedFiles[f] = True
        if not os.path.exists(dest_dir+'/'.join(f.split('/')[:-1])):
            os.makedirs(dest_dir + '/'.join(f.split('/')[:-1]))
        copyfile(shogun_dir+f,dest_dir+f)
    else:
        return
    
   #     print('could not copy',shogun_dir+f,', skipping...')
   #     return

    with open(shogun_dir+f,'r') as out:         
        for line in out:
            if '#include <shogun' in line or "#include<shogun" in line:
                grab_dependencies(line[line.find('<')+1 : line.find('>')])
    # add cpp files
    try:
        if f[:-2]+'.cpp' not in copiedFiles:
            copiedFiles[f[:-2]+'.cpp'] = True
            copyfile(shogun_dir+f[:-2]+'.cpp',dest_dir+f[:-2]+'.cpp') 
        else:
            return
    except:
        print('could not copy',shogun_dir+f[:-2]+'.cpp',', skipping...')
        return
    try: 
        with open(shogun_dir+f[:-2]+'.cpp','r') as out:         
            for line in out:
                if '#include <shogun' in line or "#include<shogun" in line:
                    grab_dependencies(line[line.find('<')+1 : line.find('>')])
    except:
        print('no',shogun_dir+f[:-2]+'.cpp','file, skipping..')
# run it
for f in fnames:
    grab_dependencies(f)

############################################### replace brackets with quotations and relative paths
print("===\nreplacing brackets with relative paths\n===")
import glob

def replace_brackets(f):
    with open(f,'r') as out:
        lines = out.readlines()
    ellipses = ''.join(['../' for n in f.split('/')[:-1]]) 
    for i in range(len(lines)):
        if '#include <shogun' in lines[i] or '#include<shogun' in lines[i] :
            
            print('\tfixing',lines[i])            
            lines[i] = lines[i].replace('<','\"'+ellipses)
            lines[i] = lines[i].replace('>','\"')
            print('\tlines[i] now:',lines[i])
        elif "#include <rxcpp" in lines[i]:
            print('\tfixing',lines[i])            
            lines[i] = lines[i].replace('<','\"'+'../../')
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


