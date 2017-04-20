#!/bin/bash
# To Respect the artists whose music we've been allowed to use under their permissions, we
# cannot include copies of their music in this public avaliable repository.

# Run this script to download a copy directly from their websites to support the artists.
cat download_music.txt | xargs -n 3 -P 8 wget -q
