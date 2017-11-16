# grab a file, search it for include paths and recursively copy those files into this directory.
from shutil import copyfile

shogun_dir = '../../../../shogun/src/'
dest_dir = '../'

fnames = []

with open('track','r') as out:
    for line in out:
        #print(line)
        if '#include' in line:
            fnames.append(line[line.find('<')+1 : line.find('>')])
            
# function to copy files and recurse on them
def grab_dependencies(f):
    print('f:',f)
    try:
        copyfile(shogun_dir+f,dest_dir+f) 
    except:
        print('could not copy',shogun_dir+f,', skipping...')
        return

    with open(shogun_dir+f,'r') as out:         
        for line in out:
            if '#include<shogun' in line:
                grab_dependencies(line[line.find('<')+1 : line.find('>')])
    # add cpp files
    try:
        copyfile(shogun_dir+f[:-2]+'.cpp',dest_dir+f[:-2]+'.cpp') 
    except:
        print('could not copy',shogun_dir+f[:-2]+'.cpp',', skipping...')
        return
    try: 
        with open(shogun_dir+f[:-2]+'.cpp','r') as out:         
            for line in out:
                if '#include<shogun' in line:
                    grab_dependencies(line[line.find('<')+1 : line.find('>')])
    except:
        print('no',shogun_dir+f[:-2]+'.cpp','file, skipping..')
# run it
for f in fnames:
    grab_dependencies(f)
