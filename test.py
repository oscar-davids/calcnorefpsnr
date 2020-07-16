
import glob
import os.path
import argparse
import subprocess
import csv
#from csv import reader
from calcnorefpsnr import calcnorefpsnr

calclib = calcnorefpsnr()

def main():
    parser = argparse.ArgumentParser(description='psnr calculation parser.')
    parser.add_argument('--dir', type=str, help='select directory')
    parser.add_argument('--infile', type=str, help='select csv file')
    args = parser.parse_args()

    infile = args.infile
    srcdir = args.dir

    if infile != None:
        filelist = []
        with open(infile, 'r') as csvfile:
            # pass the file object to reader() to get the reader object
            csv_reader = csv.reader(csvfile)
            # Pass reader object to list() to get a list of lists
            list_of_rows = list(csv_reader)
        for i in range(1, len(list_of_rows)):
            filelist.append(list_of_rows[i][0])
    elif srcdir != None:
        filelist = [file for file in glob.glob(srcdir + "**/*.mp4", recursive=True)]
    else:
        print("empty argument")
        return

    #mepath = os.path.abspath(os.path.dirname(__file__))
    #vpath = os.path.join(mepath, srcdir)

    filepsnr = open('write.csv', 'w', newline='')
    wr = csv.writer(filepsnr)

    wr.writerow([0, 'filepath', 'psnr'])

    for i in range(0, len(filelist)):
        if filelist[i].endswith('.h264'):
            psnr = calclib.calc_norefpsnr(filelist[i])
            print(psnr)
            wr.writerow([i+1, filelist[i], psnr])
        else:
            #convert h264 video file
            #ffmpeg -i test.mp4 -c:v libx264 test.h264
            print(filelist[i])
            video_file = "test.h264"
            try:
                subprocess.call(['ffmpeg',
                             '-i',
                             filelist[i],
                             '-c:v',
                             'libx264',
                             video_file])
            except:
                print('Could not covert h264 file from video file {}'.format(filelist[i]))

            if os.path.isfile(video_file):
                psnr = calclib.calc_norefpsnr(video_file)
                print(psnr)
                wr.writerow([i + 1, filelist[i], psnr])
                os.remove(video_file)

    filepsnr.close()

if __name__== "__main__":
    main()
