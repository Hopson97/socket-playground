sh scripts/build.sh release 

rm -rf -d chat

mkdir chat

cp bin/release/chat chat
cp -r res chat

echo "Deploy build created."
echo "cd chat to find it"