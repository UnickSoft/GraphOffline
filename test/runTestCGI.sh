if [ "$1" == "-linux" ]; then
    exePath=../bin/Linux/Release/GraphOffline;
else
    exePath=../bin/Mac/Release/GraphOffline;
fi

export REQUEST_METHOD="POST"
export CONTENT_LENGTH=1587
export QUERY_STRING="dsp=cgiInput&start=n0&finish=n7&report=xml"
cat "graph_shortPath_3.xml" | $exePath $2 >graph_shortPath_3.xml.cgi.test

if diff --ignore-all-space graph_shortPath_3.xml.cgi.res graph_shortPath_3.xml.cgi.test >/dev/null ; then
    echo "Ok."
else
    echo "Failed."
fi

# rm *.test >/dev/null