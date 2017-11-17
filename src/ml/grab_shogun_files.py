# grab a file, search it for include paths and recursively copy those files into this directory.
from shutil import copyfile
import os
shogun_dir = '../../../shogun/src/'
dest_dir = ''

fnames = []
copiedFiles = {}

with open('track2','r') as out:
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
