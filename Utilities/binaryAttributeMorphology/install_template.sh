#!/bin/sh

for NAME in $@ ; do
  echo -n "$NAME... "
  for f in `/bin/ls itk*XXX*.h` ; do
    dest=../`echo "$f" | sed -e "s/XXX/$NAME/g"`
    cp $f $dest
    perl -e "s/XXX/$NAME/g" -pi $dest
  done
  
  for f in `/bin/ls wrap_*XXX*.cmake` ; do
    dest=../Wrapping/`echo "$f" | sed -e "s/XXX/$NAME/g"`
    cp $f $dest
    perl -e "s/XXX/$NAME/g" -pi $dest
  done
  echo "done"
done
