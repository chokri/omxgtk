#!/bin/bash
# echo "$@"
omxgtkplayer "$@" < /tmp/omxgtk_cmd && killall omxgtk
#xrefresh -display :0
