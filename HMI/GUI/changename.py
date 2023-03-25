
# import required module
import os
# assign directory
directory = 'D:\\01.Freelancer\\00.Well_washing\\HMI\\GUI'
 
# iterate over files in
# that directory
for i in range (0,10):
    filepath= directory + '\\'+ format(i,"02")+".bmp"
    if os.path.exists(filepath):
        print("remove" ,(format(i,"02")+".bmp"))
        os.remove(filepath)
for i in range (1,10):
    filepath= directory + '\\Slide'+ str(i)+".BMP"
    if os.path.exists(filepath):
        # print("remove" ,(format(i,"02")+".bmp"))
        newname = directory + '\\'+ format(i-1,"02")+".bmp"
        print(filepath)
        os.rename(filepath, newname)
        # os.remove(filepath)
# for filename in os.listdir(directory):
#     f = os.path.join(directory, filename)
#     # checking if it is a file
#     if os.path.isfile(f):
#         print(filename)