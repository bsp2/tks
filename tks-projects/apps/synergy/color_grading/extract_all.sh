
IFS=§

for d in orig/*.png; do
    echo get "$d" `basename "$d"`
    tks color_grading get "$d" `basename "$d"`
done

