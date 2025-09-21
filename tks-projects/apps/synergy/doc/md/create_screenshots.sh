echo -n "" >screenshots.md
IDX=0
for d in screenshots/*.png; do
    # $(var:img=20191103-121836_pipemap)$(var:alt="PipeMap Page")$(inc!img.html)
    echo -n "\$(var:img=" >>screenshots.md
    echo -n $(basename "$d" .png) >>screenshots.md
    echo -n ")\$(var:alt=Screenshot)\$(inc?img.html)" >>screenshots.md
    echo "idx=$IDX"
    IDX=$((IDX+1))
    IDX=$((IDX%6))
    if [ $IDX -eq 0 ]; then
        echo "" >>screenshots.md
    fi
done
