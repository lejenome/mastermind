#!/bin/bash
gource -c 4.0 --user-image-dir .git/avatars/ --seconds-per-day 1 \
	--file-idle-time 0 --key -e 0.5 --title MasterMind -640x480 -o - \
	| ffmpeg -y -r 60 -f image2pipe -vcodec ppm -i - -vcodec libx264 \
	-preset ultrafast -pix_fmt yuv420p -crf 1 -threads 0 -bf 0 gource.mp4
