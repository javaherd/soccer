while read l; do
  youtube-dl "$l" -f 135
done < ../list
