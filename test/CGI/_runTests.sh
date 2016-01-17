export REQUEST_METHOD="POST"
export CONTENT_LENGTH=1587
export QUERY_STRING="dsp=cgiInput&start=n0&finish=n7&report=xml"

cat "graph_shortPath_3.xml" | $exePath $2 >graph_shortPath_3.xml.cgi.test

export QUERY_STRING="elloop=cgiInput&report=xml"

cat "graph6.xml" | $exePath $2 >graph6.xml.cgi.test

if diff --ignore-all-space graph_shortPath_3.xml.cgi.res graph_shortPath_3.xml.cgi.test >/dev/null && diff --ignore-all-space graph6.xml.cgi.res graph6.xml.cgi.test >/dev/null ; then
    echo "Ok."
    rm *.test >/dev/null
else
    echo "Failed."
    exit 1
fi