for file in scrape/movies/*.mp4
do
  echo ./soccer.o "$file" debug
  ./soccer.o "$file" debug > frames
  echo cat frames \| ./videoWriter.o "$file" "$file.avi"
  cat frames | ./videoWriter.o "$file" "$file.avi"
done
