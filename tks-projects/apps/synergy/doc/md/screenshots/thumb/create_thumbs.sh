
for d in ../*.png; do 
    echo $d
    convert $d -resize 150x95 -define png:compression-filter=5 -define png:compression-level=9 -define png:compression-strategy=1 -define png:exclude-chunk=all -colors 256 $(basename "$d" .png)_thumb.png 
done
